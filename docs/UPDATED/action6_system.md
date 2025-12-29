# Action6 System

This page documents the **Action6** handler architecture we’ve verified in `mnsg.us.0.z64`: the **index byte**, wrapper callsites, table layout, and the confirmed `jr`-based dispatch path.

Verified end-to-end proof strategy:

`ctx+0x60 index → wrapper tables → dispatch path → jr jump-table → handler entry`

---

## What to remember

- Action6 uses an **8-bit index** at **`ctx+0x60`** to select behavior.
- Wrappers compute `idx_off = (ctx+0x60 << 2)` and load pointers from multiple tables.
- Actual handler execution is via a **`jr` jump-table**, so it won’t show up as a clean JAL callgraph.

---

## Field map

### A) Action6 index byte
- **Field:** `ctx + 0x60` (u8)
- **Meaning (verified):** selects which Action6 handler path runs
- **Notes:**
  - There is also a `ctx+0x61` subindex used by at least one state machine writer.

---

## Wrapper callsites (verified)

These wrappers all follow the same pattern:
- load `idx = *(u8*)(ctx+0x60)`
- compute `idx_off = idx << 2`
- load `a1` from main table
- load `a2` from companion tables
- call `FUN_801E54E0` with `v0=idx_off`

**Verified wrapper addresses:**
- `0x801E132C`
- `0x801E13D0`
- `0x801E1414`

---

## Table layout (verified bases)

Wrappers index into these tables:
- **Primary table (a1):** `0x80204340 + idx_off`
- **Companion tables:**  
  - `0x80204370 + idx_off`  
  - `0x80204350 + idx_off`  
  - `0x80204390 + idx_off`

---

## Dispatch path (verified)

### A) The “not obvious” part
- `FUN_801E54E0` lives in a window where we did **not** see the direct `jalr/jr t9` style execution you’d expect.
- The real handler selection occurs via a **jr-based jump-table** elsewhere.

### B) Confirmed `jr` jump-table dispatch site
- **Dispatch site:** `0x801E70D0`
- Pattern (simplified):  
  `sll t7, v0, 2` → load from table → `jr t7`
- **Inferred table base:** `0x8020B5B0`
- **Known entry count:** `6`
- **Known entries:**
  - `[0] 0x801E70D8`
  - `[1] 0x801E715C`
  - `[2] 0x801E7178`
  - `[3] 0x801E70F0`
  - `[4] 0x801E7114`
  - `[5] 0x801E7140`

### C) Selector helper
- Selector function used in this region: `FUN_801E71D0`

---

## Known writers of `ctx+0x60` (verified)

- `FUN_801ED968` (state machine uses `ctx+0x61` subindex and counters)
- `FUN_801EAC24`
- `FUN_801EA0D8` (explicit set index=1)

---

## Heuristics that matter for decompilers

- Don’t search only for `jalr t9`. Action6 dispatch is `jr`-based; scan for **`jr <any reg>`** patterns.
- Pointer tables can exist with **0 xrefs** (loaded indirectly/gp-relative).  
  Example: orphan function-pointer table at `0x80204870`.

---

## Practical mod hooks (safe entry points)

- **Observe selection:** hook any wrapper (`0x801E132C / 0x801E13D0 / 0x801E1414`) and print `ctx+0x60`.
- **Force an action:** patch `ctx+0x60` just before wrapper runs.
- **Trace actual handler:** hook `0x801E70D0` and log computed table target.

---

## Tiny Action6 debug checklist

### 1) Is `ctx+0x60` changing?
- Print `ctx+0x60` and `ctx+0x61` each frame (or on change).
- Confirm known writer contexts (combat state, menus, etc.) shift it.

### 2) Are wrappers being hit?
- Put a “hit counter” on the wrapper addresses.
- If the index changes but wrappers aren’t called, you’re watching the wrong ctx instance.

### 3) Are we hitting the `jr` dispatch?
- Hook `0x801E70D0` and log the resolved target.
- If wrappers hit but dispatch doesn’t, there’s an intermediate path you haven’t followed yet.

### 4) Does the target match one of the 6 entries?
- If you see targets outside the 6-entry table:
  - you’re on a different Action6 table, or
  - v0 scaling differs (not `idx<<2`), or
  - you hit a different dispatch site.
