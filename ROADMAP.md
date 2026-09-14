# AmiHeuristics Roadmap

AmiHeuristics provides reusable heuristic detection for classic Amiga systems while keeping signature matching, dynamic execution and forensic investigation as separable concerns.

## M0 — Foundation

Status: **ESTABLISHED**

- Define AmigaOS 2.04+ / 68000 baseline.
- Define defensive, passive-first project scope.
- Establish common finding, severity and scoring concepts.
- Establish scanner boundaries: file, bootblock, disk and memory.
- Establish CLI/API/ARexx direction.
- Add build skeleton and repository checks.
- Document integration boundaries with AmiGuard, AmiSandbox and AmiForensics.

Exit criterion: repository structure and architecture are explicit and mechanically checkable. No detection claim is required.

## M1 — Bootblock heuristics

- Parse 1024-byte Amiga bootblocks without executing them.
- Validate DOS type/header and boot checksum.
- Identify executable-looking 68k content.
- Introduce weighted bootblock rules.
- Detect suspicious direct disk access and bootblock rewrite patterns where statically observable.
- Add known-benign fixtures to control false positives.
- Produce explainable findings and score.

## M2 — HUNK/file heuristics

- Parse Amiga HUNK binaries defensively.
- Validate HUNK structure and bounds.
- Identify suspicious code/data relationships.
- Recognise selected Exec/DOS/device call patterns.
- Flag self-modification/decryptor-like patterns conservatively.
- Add file-level scoring and explainable evidence.

## M3 — Live memory and vector scanner

- Inspect Exec/DOS structures using supported OS interfaces where possible.
- Inventory residents, tasks/processes, libraries and devices.
- Inspect selected library/device vectors and interrupt-related structures.
- Introduce vector provenance: expected module/ROM versus unknown executable RAM.
- Distinguish known/allowed patches from unexplained modifications.
- Support snapshot and diff reports.

## M4 — Disk scanner

- Combine bootblock and filesystem/file scanning.
- Inspect floppy images/media and supported filesystem objects.
- Detect suspicious metadata/layout anomalies conservatively.
- Support read-only whole-media reports.
- Never repair or modify media unless a later explicit remediation component is designed.

## M5 — AmiSandbox integration

- Export machine-readable findings.
- Escalate uncertain samples to isolated dynamic analysis.
- Compare pre/post execution state.
- Consume observed behaviour: disk writes, vector patches, residents, tasks and file changes.
- Correlate static and dynamic evidence.

## M6 — Shared engine and integrations

- Stabilise reusable C API.
- Add ARexx command surface where useful.
- Integrate with AmiGuard for protection/detection workflows.
- Integrate with AmiForensics for investigation/reporting.
- Keep signature engines pluggable rather than duplicating xvs-style databases.

## M7 — Hardening and release qualification

- 68000 compatibility audit.
- AmigaOS 2.04 runtime qualification.
- False-positive corpus qualification across games, demos, loaders and system utilities.
- Resource/memory footprint measurements.
- Fuzz malformed file/bootblock inputs on host tooling where practical.
- Reproducible release packaging and documentation.

## Design principles

1. Explain every heuristic verdict with concrete evidence.
2. Prefer multiple correlated indicators over single-rule malware declarations.
3. Treat unusual Amiga techniques as suspicious evidence, not automatic malware.
4. Keep parsing passive and bounds-checked.
5. Never execute untrusted code in the static scanner.
6. Keep the 68000 / AmigaOS 2.04 baseline unless a feature explicitly requires more.
