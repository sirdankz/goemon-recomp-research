## Key addresses & engine entrypoints

> Goal of this page: the “one screen” list of the most reliable, confirmed addresses and entrypoints we keep coming back to.

### Raw memory addresses (confirmed)

#### Input
- `0x800C7D3A` *(u16)* — **Buttons held bitfield**
  - `BTN_A = 0x8000`
  - `BTN_B = 0x4000`
  - `BTN_L = 0x0020`

#### Player combat / status
- `0x8015C604` *(u32)* — **Current weapon slot** (`wpn.`)  
  Observed values: `1 / 2 / 3` (slot 3 includes Goemon’s coin throw; other characters map to their “slot 3” tool/attack).
- `0x8015C5E7` *(u8)* — **HP in half-hearts**  
  (Matches the GameShark-style “infinite health” behavior you referenced: `8015C5E7 0028`.)

### Key runtime pointers / structs (confirmed)

#### “ctx” (player/actor context pointer used by our hooks)
- Known stable ctx (often): `0x8016A090`
- `ctx + 0x4C` *(u8)* — **Mode/state discriminator** you observed flipping with attacks/tools.
- `ctx + 0x5C` *(u32 ptr)* — points to `ctl` (controller-ish block used for camera/movement/pushback knobs)

#### “ctl” block (from your logs: `ctl = 0x8020C860` when `ctx = 0x8016A090`)
These are *confirmed to be camera/movement-related* (not attack-lock flags):
- `ctl + 0x398` — camera left/right + zoom changes (freezes when camera still)
- `ctl + 0x39C` / `ctl + 0x3A4` — changes with character movement (freezes when not moving)
- `ctl + 0x3A0` — camera pitch (holding L nudges toward top-down); in “look-only” zoom mode it freezes at `0x41A00000`
- `ctl + 0x04` *(float)* — **pushback scalar** we write for recoil/pushback experiments (e.g., `0xBF4CCCCD` ≈ `-0.8f`)

### Engine entrypoints we hook / observe (confirmed)

- `func_801DD830_599740` — “DD830”
  - High-frequency action path: counts up when **attacking / jumping / changing characters**.
  - $a1 acts like an **action classifier** (see `reference/action_codes.md`).
- `func_801E3B9C_59FAAC` — “E3B9C”
  - Counts up on **sudden stop / braking** and is hit by **weapon 1/2 attack paths**.
  - Good probe point for **movement lock / braking / attack transition** behavior.
- `func_801D98E0_5957F0` — per-frame-ish return hook used for overlays + timers.
- `func_800042AC_4EAC(entry, str, x, y)` — overlay draw helper (callee).

### Stage/data plumbing (confirmed)
- `D_80231300_5EC7D0` — pointer table indexed by `(stage_id * 4)` to load a stage entry pointer.
- StageActorMetadata fields:
  - `u16` at `+0x14`
  - function pointer at `+0x18` (called via `jalr`)
- `func_80013AC4_146C4` — iterates a `u16` ID list terminated by `0`, calls a loader for each entry.
- `func_801F71D0_5B30E0` — global/init-ish (touches globals, calls `osViSetSpecialFeatures`), not stage-specific.

---

### “Context gate” discriminator (useful, but has gotchas)

Sometimes we gate writes to only run when:
- `ctx == 0x8016A090`
- `rd8(ctx+0x4C) == 0x14`

This can prevent crashes, but can also **exclude real gameplay cases** (see gotchas page).
---

## Also captured in Master Summarized

If you’re looking for the “everything on one page” version (including gotchas), see:
- [Master summarized](master_summarized.md)

This Addresses page stays “just the raw facts,” while Master Summarized keeps the narrative + gotchas.
