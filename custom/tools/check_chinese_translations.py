"""Validate Chinese UI coverage, variables, enum separators and rich-text structure."""

from __future__ import annotations

import argparse
import json
import re
import sys
import xml.etree.ElementTree as ET
from collections import Counter
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
CATALOGS = ("qgc_source_zh_CN.ts", "qgc_json_zh_CN.ts")
PLACEHOLDERS = re.compile(r"%(?:L?[1-9][0-9]?|L?n)")
CHINESE = re.compile(r"[\u4e00-\u9fff]")
RICH_TAGS = re.compile(r"</?\s*(?:b|i|u|br|p|pre|font|span|a|ul|li|h[1-6])\b[^>]*>", re.I)


def messages(path: Path) -> dict[tuple[str, str, str], ET.Element]:
    result = {}
    for context in ET.parse(path).getroot().findall("context"):
        for message in context.findall("message"):
            key = (
                context.findtext("name", ""),
                message.findtext("source", ""),
                message.findtext("comment", ""),
            )
            if key in result:
                raise ValueError(f"Duplicate message in {path}: {key}")
            result[key] = message
    return result


def audit(source_paths: list[Path]) -> dict:
    identifiers = json.loads(
        (REPO / "custom/tools/chinese-identifiers.json").read_text(encoding="utf-8")
    )
    allowed = {(entry["context"], entry["source"]) for entry in identifiers}
    errors = []
    counts = Counter()
    translated = {}
    for name in CATALOGS:
        catalog = messages(REPO / "translations" / name)
        translated.update(catalog)
        for (context, source, _comment), message in catalog.items():
            if not source:
                counts["empty_source"] += 1
                continue
            counts["messages"] += 1
            element = message.find("translation")
            value = "" if element is None else "".join(element.itertext()).strip()
            label = f"{context}: {source[:100]}"
            if (
                not value
                or element is None
                or element.get("type") in {"unfinished", "obsolete", "vanished"}
            ):
                errors.append(f"Missing finished translation: {label}")
            if re.search(r"[A-Za-z]{2}", source) and not CHINESE.search(value):
                if (context, source) in allowed:
                    counts["technical_identifiers"] += 1
                else:
                    errors.append(f"Untranslated English: {label}")
            if set(PLACEHOLDERS.findall(source)) != set(PLACEHOLDERS.findall(value)):
                errors.append(f"Placeholder mismatch: {label}")
            if "Only use english comma" in message.findtext("translatorcomment", "") and (
                source.count(",") != value.count(",") or "\uff0c" in value
            ):
                errors.append(f"Enum/keyword separator mismatch: {label}")
            if RICH_TAGS.findall(source) != RICH_TAGS.findall(value):
                errors.append(f"Rich-text markup mismatch: {label}")
            if re.search(r"\[\s*1\d{3}|第1\d{3}服|双线|新服", value):
                errors.append(f"Invalid translation artifact: {label}")
    for source_path in source_paths:
        for key in messages(source_path):
            if key[1] and key not in translated:
                errors.append(f"Missing current source message: {key}")
    counts["errors"] = len(errors)
    return {"counts": dict(counts), "errors": errors}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-ts", type=Path, action="append", default=[])
    parser.add_argument("--output", type=Path)
    parser.add_argument("--nsis-script", type=Path)
    args = parser.parse_args()
    result = audit(args.source_ts)
    if args.nsis_script:
        script = args.nsis_script.read_text(encoding="utf-8-sig")
        required = (
            "MUI_CUSTOMFUNCTION_GUIINIT CaacChineseGuiInit",
            "MUI_CUSTOMFUNCTION_UNGUIINIT un.CaacChineseGuiInit",
            "StrCpy $LANGUAGE 2052",
            "显卡兼容模式",
            "正在卸载旧版本",
            "安装选项",
            'Section "运行文件" Runtime',
        )
        result["errors"].extend(
            f"Missing compiled NSIS localization: {label}"
            for label in required
            if label not in script
        )
        result["errors"].extend(
            f"Untranslated NSIS label: {label}"
            for label in (
                'MUI_HEADER_TEXT "Install Options"',
                'DetailPrint "Set install registry entry:',
            )
            if label in script
        )
        result["counts"]["errors"] = len(result["errors"])
        result["nsis_script"] = str(args.nsis_script)
    output = json.dumps(result, ensure_ascii=False, indent=2) + "\n"
    if args.output:
        args.output.write_text(output, encoding="utf-8")
    print(output, end="")
    return bool(result["errors"])


if __name__ == "__main__":
    sys.exit(main())
