"""Regenerate the UI font subset from the Noto Sans CJK 2.004 regular TTC."""

import sys
from pathlib import Path

from fontTools import subset
from fontTools.ttLib import TTFont

root = Path(__file__).resolve().parent
font = TTFont(sys.argv[1], fontNumber=2)
if font["name"].getDebugName(1) != "Noto Sans CJK SC":
    raise ValueError("Expected the Simplified Chinese face of Noto Sans CJK")
text = "".join(path.read_text(encoding="utf-8") for path in root.glob("*.cc"))
unicodes = set(range(32, 256)) | set(range(0x370, 0x400)) | {ord(character) for character in text}
options = subset.Options()
options.name_IDs = ["*"]
subsetter = subset.Subsetter(options=options)
subsetter.populate(unicodes=unicodes)
subsetter.subset(font)
names = {
    1: "Feihu Calculator Sans",
    2: "Regular",
    3: "FeihuCalculatorSans-Regular-1.0",
    4: "Feihu Calculator Sans Regular",
    6: "FeihuCalculatorSans-Regular",
    16: "Feihu Calculator Sans",
    17: "Regular",
}
for record in font["name"].names:
    if record.nameID in names:
        record.string = names[record.nameID].encode(record.getEncoding())
cff = font["CFF "].cff
cff.fontNames = [names[6]]
cff.topDictIndex[0].FamilyName = names[1]
cff.topDictIndex[0].FullName = names[4]
font.save(root / "CalculatorSans.otf")
