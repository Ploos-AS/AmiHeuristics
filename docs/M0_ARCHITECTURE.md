# M0 Architecture

## Purpose

AmiHeuristics is a reusable heuristic analysis engine for classic Amiga malware detection. Its purpose is to identify and explain suspicious properties that can indicate previously unknown malware without requiring an exact signature match.

M0 deliberately implements infrastructure rather than claiming detection capability.

## Platform contract

Native target:

- AmigaOS 2.04 or later
- Motorola 68000 instruction-set baseline
- no FPU requirement
- C89-friendly implementation style where practical
- low-memory operation suitable for classic systems

Development and qualification may also use Linux host tools, cross-compilation and emulation.

## Components

### Core

`src/core/` owns common data structures, scoring and result aggregation. Scanner-specific code must not invent incompatible severity models.

### File scanner

Responsible for passive parsing and inspection of files, initially Amiga HUNK binaries. It may classify structures and instruction patterns but must not load or execute the target as code.

### Boot scanner

Responsible for Amiga bootblocks. Input is treated as untrusted bytes. The scanner will validate structure/checksum independently from heuristic scoring.

### Disk scanner

Responsible for media-wide orchestration. It composes boot and file analysis and may add media/layout findings. Read-only operation is the default contract.

### Memory scanner

Responsible for live-system observations: residents, tasks/processes, libraries/devices, selected vectors/patches and memory provenance. This component necessarily has a different trust boundary from passive file parsing and must fail safely on unsupported layouts.

### Reporting

All scanners emit findings through a common representation. Human-readable CLI output and machine-readable output should be generated from the same findings rather than separate detection logic.

## Finding model

A finding consists conceptually of:

- stable rule identifier
- scanner/domain
- severity
- signed score contribution
- short summary
- optional location/address/offset
- evidence suitable for explanation

Initial severity levels:

- INFO
- LOW
- MEDIUM
- HIGH
- CRITICAL

A final score is an aid to triage, not proof of infection. Thresholds remain policy and must be tunable as the benign corpus grows.

## False-positive policy

Classic Amiga software frequently uses techniques that resemble malware behaviour: direct hardware access, custom loaders, unusual memory use, vector patches and self-modifying/decompression code. Therefore:

- one generic indicator should rarely produce a malware verdict;
- correlated findings should increase confidence;
- known-benign/context rules may reduce score;
- reports must expose the contributing rules;
- qualification must include games, demos, utilities, filesystem tools and legitimate patches.

## Integration boundaries

### Signature scanners

AmiHeuristics does not need to duplicate a virus-signature database. A signature result may be consumed as external evidence in a higher-level product such as AmiGuard.

### AmiGuard

AmiGuard can consume the engine for on-demand or protective scanning and combine heuristics with signatures.

### AmiSandbox

AmiSandbox is the escalation path for samples whose static evidence warrants dynamic observation. Static AmiHeuristics code must not execute suspicious samples itself.

### AmiForensics

AmiForensics can consume detailed findings, snapshots and provenance data for investigation.

## ARexx

ARexx is part of the integration contract from the beginning. A later milestone should expose stable commands such as scan-file, scan-boot, scan-disk, scan-memory and query-result once their underlying engines exist. ARexx must wrap the same core API as the CLI rather than fork detection logic.

## Security rules

- All lengths and offsets are untrusted.
- Parsers reject truncation/overflow rather than guessing.
- Static scanning never transfers control to target bytes.
- Disk scanning defaults to read-only access.
- Unknown memory structures produce an incomplete/unsupported result rather than unsafe traversal.
- Rules describe suspicious properties; they do not attempt to reproduce malware functionality.

## M0 exit criteria

M0 passes when the repository documents this architecture, exposes the common public types, builds a minimal core on a normal host compiler, and `make check` verifies the baseline structure. Runtime malware detection is explicitly outside M0.
