#!/usr/bin/env python3
"""Check pinned source hashes, patch applicability and actual fence policy branches.

The host harness compiles the patched fence.cpp and extracts the actual inline
capability accessor and mode-lockout expression. Vehicle, logger and scheduler
interfaces are test doubles; this is not a full firmware, SITL or hardware test.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import subprocess
import tempfile
import urllib.request
from pathlib import Path


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(command: list[str], directory: Path) -> None:
    subprocess.run(command, cwd=directory, check=True)


def validate(source: Path | None, compiler: str) -> None:
    artifact = Path(__file__).resolve().parent
    manifest = json.loads((artifact / "manifest.json").read_text())
    patch = artifact / manifest["patch"]
    if digest(patch) != manifest["patch_sha256"]:
        raise ValueError("Patch SHA-256 mismatch")
    with tempfile.TemporaryDirectory(prefix="caac-arducopter-policy-") as temporary:
        work = Path(temporary)
        for relative, expected in manifest["source_files"].items():
            destination = work / relative
            destination.parent.mkdir(parents=True, exist_ok=True)
            if source is not None:
                shutil.copyfile(source / relative, destination)
            else:
                url = (
                    "https://raw.githubusercontent.com/ArduPilot/ardupilot/"
                    f"{manifest['upstream_commit']}/{relative}"
                )
                with urllib.request.urlopen(url, timeout=30) as response:
                    destination.write_bytes(response.read())
            if digest(destination) != expected:
                raise ValueError(f"Pinned source SHA-256 mismatch: {relative}")
        run(["git", "init", "-q"], work)
        run(["git", "apply", "--check", str(patch)], work)
        run(["git", "apply", str(patch)], work)
        for relative, expected in manifest["patched_files"].items():
            if digest(work / relative) != expected:
                raise ValueError(f"Patched source SHA-256 mismatch: {relative}")
        print(f"PASS: exact {manifest['upstream_tag']} hashes and git apply --check", flush=True)

        fence_source = (work / "libraries/AC_Fence/AC_Fence.cpp").read_text()
        groups = re.findall(r'AP_GROUPINFO(?:_FRAME)?\("[^"]+",\s*(\d+)', fence_source)
        if len(groups) != len(set(groups)):
            raise ValueError("AC_Fence parameter indices must remain unique")
        if not re.search(r'AP_GROUPINFO_FRAME\("NOLD", 16, AC_Fence, _no_land, 0,', fence_source):
            raise ValueError("FENCE_NOLD metadata must preserve index 16 and default 0")
        fence_header = (work / "libraries/AC_Fence/AC_Fence.h").read_text()
        accessor = re.search(r"bool brake_no_land\(\) const \{[^}]+\}", fence_header)
        if accessor is None or "AP_Int8         _no_land;" not in fence_header:
            raise ValueError("FENCE_NOLD capability accessor or AP_Int8 storage missing")
        (work / "actual_accessor.h").write_text(
            accessor[0].replace("bool brake_no_land", "bool AC_Fence::brake_no_land")
        )

        mode_source = (work / "ArduCopter/mode.cpp").read_text()
        guard = re.search(
            r"// may not be allowed to change mode if recovering from fence breach\s+if \((.*?)\) \{",
            mode_source,
            re.DOTALL,
        )
        if guard is None:
            raise ValueError("Could not find pinned Copter fence mode-lockout condition")
        (work / "actual_mode_lockout.h").write_text(
            "bool Copter::fence_mode_locked(bool ignore_checks) const { return "
            + guard[1]
            + "; }\n"
        )
        rejection = mode_source.index("if (!new_flightmode->init(ignore_checks))")
        update = mode_source.index("flightmode = new_flightmode;", rejection)
        if "return false;" not in mode_source[rejection:update]:
            raise ValueError("Pinned mode entry rejection must retain the existing flightmode")

        shutil.copyfile(artifact / "tests/Copter.stub.h", work / "Copter.h")
        shutil.copyfile(artifact / "tests/FencePolicyHarness.cpp", work / "FencePolicyHarness.cpp")
        shutil.copyfile(work / "ArduCopter/fence.cpp", work / "fence.cpp")
        executable = work / "fence-policy-test"
        run(
            [
                compiler,
                "-std=c++17",
                "-Wall",
                "-Wextra",
                "-Werror",
                "FencePolicyHarness.cpp",
                "-o",
                str(executable),
            ],
            work,
        )
        run([str(executable)], work)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--source-root", type=Path, help="Exact upstream checkout; omit to download pinned files"
    )
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    validate(args.source_root, args.compiler)


if __name__ == "__main__":
    main()
