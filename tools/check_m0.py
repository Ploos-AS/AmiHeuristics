#!/usr/bin/env python3
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]

checks = {
    "README documents 68000 baseline": (ROOT / "README.md", "Motorola 68000 baseline"),
    "README documents four domains": (ROOT / "README.md", "Memory scanner"),
    "roadmap has M1 bootblock": (ROOT / "ROADMAP.md", "M1 — Bootblock heuristics"),
    "roadmap has memory milestone": (ROOT / "ROADMAP.md", "Live memory and vector scanner"),
    "architecture documents ARexx": (ROOT / "docs/M0_ARCHITECTURE.md", "ARexx"),
    "public finding type exists": (ROOT / "include/amiheuristics/amiheuristics.h", "AmiHeurFinding"),
    "scoring implementation exists": (ROOT / "src/core/score.c", "amiheur_score_add"),
}

failed = False
for label, (path, needle) in checks.items():
    if not path.is_file():
        print(f"FAIL: {label}: missing {path.relative_to(ROOT)}")
        failed = True
        continue
    text = path.read_text(encoding="utf-8")
    if needle not in text:
        print(f"FAIL: {label}: missing marker {needle!r}")
        failed = True
    else:
        print(f"PASS: {label}")

if failed:
    sys.exit(1)

print("M0 STATIC PASS")
