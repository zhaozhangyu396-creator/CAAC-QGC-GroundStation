#!/usr/bin/env python3
"""Exclude build SDK directories from PATH before testing an installed Windows app."""

from __future__ import annotations

import ntpath
import os
import sys
from pathlib import Path


def filter_sdk_paths(path: str, sdk_roots: list[str]) -> str:
    """Preserve Windows PATH entries outside the supplied SDK trees."""
    if any(character in path for character in "\r\n"):
        raise ValueError("Windows PATH contains a line break")
    roots = [
        ntpath.normcase(ntpath.normpath(root.strip().strip('"')))
        for root in sdk_roots
        if root.strip()
    ]
    kept = []
    for entry in path.split(";"):
        if not entry.strip():
            continue
        normalized = ntpath.normcase(ntpath.normpath(entry.strip().strip('"')))
        if any(
            normalized == root or normalized.startswith(root.rstrip("\\") + "\\") for root in roots
        ):
            continue
        kept.append(entry)
    return ";".join(kept)


def main() -> int:
    qt_root = os.environ.get("QT_ROOT", "")
    github_env = os.environ.get("GITHUB_ENV", "")
    if not qt_root.strip() or not github_env:
        print("QT_ROOT and GITHUB_ENV are required for installed verification", file=sys.stderr)
        return 1
    try:
        sanitized = filter_sdk_paths(
            os.environ.get("PATH", ""), [qt_root, os.environ.get("GST_ROOT", "")]
        )
        if not sanitized:
            raise ValueError("No executable search paths remain after removing build SDKs")
        with Path(github_env).open("a", encoding="utf-8") as output:
            output.write(f"QGC_VERIFY_INSTALLED_PATH={sanitized}\n")
    except (OSError, ValueError) as error:
        print(f"Unable to prepare installed verification: {error}", file=sys.stderr)
        return 1
    print("Installed verification PATH prepared without the Qt and GStreamer build SDKs")
    return 0


if __name__ == "__main__":
    sys.exit(main())
