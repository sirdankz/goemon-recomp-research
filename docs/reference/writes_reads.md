# Writes / Reads Index

This page is split into:

1. **Verified** write/read facts we have proven end-to-end.
2. **What the evidence pack can and cannot automatically prove** (yet), and how to extend it.

## 1) Verified flags / fields

### Door / transition request flag (0x80)
- Field: `g_system.field77` @ `+0x3AE29`
- Verified writers (write value `0x80`):
  - `FUN_80006140`
  - `FUN_800061B0`
  - `FUN_80037284`
- Verified gate / consumer chain:
  - `FUN_801F7B0C` gates on `field77 & 0x80` → branches to `FUN_801F7C14`

### Transition state
- Field: `g_system.field444` @ `+0x3B01D`
- Key writer:
  - `FUN_801F7F78` sets transition state = `4`

### Overlay door trigger pipeline
- Producers write `obj+0xDA`:
  - `FUN_801FB1C0` (param1==0x10 gate)
  - `FUN_8021D724`
  - `FUN_802239B0`
- Consumer:
  - `FUN_803BC020` latches `obj+0xEC |= 4`, clears `obj+0xDA`, schedules `0x80001240` (x2) and `0x80001020` (x1)

### Action6 index
- Field: `ctx+0x60` (u8)
- Wrapper readers:
  - `FUN_801E132C`, `FUN_801E13D0`, `FUN_801E1414`
- Known writers:
  - `FUN_801ED968`, `FUN_801EAC24`, `FUN_801EA0D8`

## 2) What’s missing for “full writes/reads”

The current evidence pack includes **instruction dumps**, **symbols**, and **callgraph edges**, but it does not yet include a full IR-level dataflow “who writes which field” map.

### How to extend this (recommended)
Add a second exporter pass that:
- recognizes MIPS address build patterns (`lui`/`addiu`/`ori`) feeding `sb/sh/sw`
- tracks base registers over short basic-block windows
- emits:
  - `absolute_writes.csv` (addr, size, writer func, callsite)
  - `absolute_reads.csv` (addr, size, reader func, callsite)
  - `struct_relative_rw.csv` (base symbol + offset patterns)

If you want, we can generate that exporter script next so the index becomes truly exhaustive.
