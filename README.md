# AmiHeuristics

AmiHeuristics is a lightweight heuristic malware-analysis engine for classic Amiga systems.

The project is designed to detect suspicious behaviour and structures even when no known virus signature exists. It complements signature scanners such as xvs.library and is intended to become a reusable detection engine for AmiGuard, AmiForensics, AmiSandbox and related tooling.

## Target

- AmigaOS 2.04+
- Motorola 68000 baseline
- No FPU required
- Low memory footprint
- Native CLI first
- ARexx integration where useful
- Host-side analysis tools may be added for development, testing and sandbox workflows

## Detection domains

AmiHeuristics is planned around four scanners sharing one scoring engine:

- **File scanner** — Amiga HUNK executables, libraries, devices and related binaries
- **Boot scanner** — bootblock structure, checksum, executable patterns and suspicious disk activity
- **Disk scanner** — whole-disk inspection, filesystem anomalies, boot sectors and suspicious hidden content
- **Memory scanner** — Exec/DOS vectors, residents, tasks, interrupt servers, patched functions and executable RAM provenance

A finding is not malware merely because one unusual operation is present. AmiHeuristics uses weighted evidence and contextual scoring to reduce false positives from legitimate demos, games, loaders and system patches.

## Planned architecture

```text
                    AmiHeuristics
                         |
          +--------------+--------------+
          |              |              |
       scanners        scoring        reporting
          |              |              |
   +------+------+     rules       CLI / ARexx / API
   |      |      |
 files  boot   memory
   |
  disk

Known malware signatures remain the responsibility of signature engines.
Unknown or suspicious samples can later be escalated to AmiSandbox for dynamic analysis.
```

## M0 status

M0 establishes the project contract and repository baseline:

- project scope and target platform
- modular architecture
- stable public finding/severity model
- initial source tree and build system
- host-side baseline checks
- roadmap through bootblock, file, memory, disk and sandbox integration milestones

M0 does **not** claim malware-detection capability yet.

See [ROADMAP.md](ROADMAP.md) and [docs/M0_ARCHITECTURE.md](docs/M0_ARCHITECTURE.md).

## Build

Host baseline:

```sh
make
make check
```

The Amiga build will use the Bebbo GCC/m68k-amigaos toolchain once native runtime code is introduced.

## Safety

AmiHeuristics is defensive software. Analysis code should prefer passive parsing and inspection. Potentially malicious samples must not be executed by the static scanner; dynamic execution belongs in an isolated AmiSandbox workflow.

## License

MIT. See [LICENSE](LICENSE).
