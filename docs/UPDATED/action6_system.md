# Action6 System

Verified: wrappers load tables using `ctx+0x60`, call `FUN_801E54E0`, then execution tail-jumps via a jr jump-table.

## Index & wrappers
- **Index byte:** `ctx+0x60` (u8) selects handler entry
- Wrapper call sites:
  - `0x801E132C`
  - `0x801E13D0`
  - `0x801E1414`

## Tables
- `0x80204340` : a1 table (index\*4)
- `0x80204370`, `0x80204350`, `0x80204390` : companion tables (a2 / params)

## Dispatch
- `FUN_801E54E0` (0x801E5400..0x801E5800) contains no `jalr/jr t9` → handler execution occurs elsewhere
- jr-table dispatch at `0x801E70D0`:

```
sll t7, v0, 2
lui at, 0x8021
addu at, at, t7
lw  t7, -0x4A50(at)   ; table base ≈ 0x8020B5B0
jr  t7
```

- Inferred jump-table base: `0x8020B5B0`, count=6

## Known ctx+0x60 writers
- `FUN_801ED968` (state machine using ctx+0x61 subindex + counters)
- `FUN_801EAC24`
- `FUN_801EA0D8` (explicitly sets index=1)

## Heuristic: orphan function-pointer table
Table at `0x80204870` had 0 xrefs in our snapshot, but still contains valid function pointers.
Automation should scan for pointer tables even if xrefs are missing (gp-relative access, indirect construction).
