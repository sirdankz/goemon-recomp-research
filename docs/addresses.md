# Addresses

This page is meant to be **copy/paste friendly** (Obsidian/Notion) and stays focused on “things you can read/write right now”.

---

## Confirmed key addresses

### Input

- `0x800C7D3A` *(u16)* — **Buttons held bitfield**
  - `BTN_A = 0x8000`
  - `BTN_B = 0x4000`
  - `BTN_L = 0x0020` *(from your logs)*

### Player combat / status

- `0x8015C604` *(u32)* — **Current weapon slot** (`wpn.` on your overlay; observed values `1/2/3`)
- `0x8015C5E7` *(u8)* — **HP in half-hearts**
  - Your referenced GameShark code `8015C5E7 0028` matches “infinite health” behavior.

---

## Confirmed engine entrypoints (hooks)

These are the main “tap points” used by `template_debug.c` and your tracing notes:

- `func_801DD830_599740` — **ENTRY hook**
  - Your overlay label: `DD830`
  - Counts up during: attacking, jumping, changing characters.
  - Acts like an **action classifier**: the `$a1` value changes with what the player is doing.  
  - See: **Hooks → DD830** on the website.

- `func_801E3B9C_59FAAC` — **ENTRY + RETURN hook**
  - Your overlay label: `E3B9C`
  - Counts up during: “sudden stop” transitions and also during weapon 1/2 attacks (both paths call into this function).
  - Used as the **safe RETURN-point** to apply pushback/recoil (writes to `ctl+0x04` in the example mod).
  - See: **Hooks → E3B9C** on the website.

- `func_801D98E0_5957F0` — **RETURN hook**
  - Used as the per-frame place to draw overlay + run B-hold / pending timers.

- `func_800042AC_4EAC(entry, str, x, y)` — **draw text callee**
  - Used by the overlay to print lines.

---

## Confirmed “ctx / ctl” pointers (runtime tracing)

These are from your confirmed runtime observations:

- `ctx` stable signature (often): `ctx = 0x8016A090`
- `ctl` observed from that ctx: `ctl = 0x8020C860` (via `*(ctx + 0x5C)`)

### “These ctl offsets are camera/movement-ish (not attack lock)”

- `ctl+0x398` changes with camera left/right + zoom; freezes when camera still.
- `ctl+0x3A0` affects camera pitch; in look-only zoom mode it freezes at `0x41A00000`.
- `ctl+0x39C` and `ctl+0x3A4` change with character movement; freeze when not moving.

---

## Confirmed stage/data plumbing (not combat, but important)

- `D_80231300_5EC7D0` — pointer table indexed with `(stage_id * 4)` to load a pointer entry.
- Stage actor metadata access pattern:
  - `u16` field at `+0x14`
  - function pointer at `+0x18` (called via `jalr`)
- `func_80013AC4_146C4` iterates a `u16` list terminated by `0` and calls a loader for each entry.
- `func_801F71D0_5B30E0` appears global/init (touches globals, calls `osViSetSpecialFeatures`), not stage-specific.

See: **Reference → Stage plumbing**.
