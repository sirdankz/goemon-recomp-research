# `ctx + 0x4C` map (and why teleports break your gates)

This page consolidates every `ctx+0x4C` value you logged and calls out the traps.

---

## Your confirmed observations

### Defaults / “spawn smoke”
- Default when any character is spawned in behind smoke:
  - `4C = 0x14`

### Weapon attacks (per your logs)

- **Weapon 1 attacks**
  - Ebisumaru + Goemon: `4C = 0x16`
  - Sasuke + Yae: `4C = 0x23`

- **Weapon 2 / Weapon 3 attacks**
  - Sasuke + Goemon: `4C = 0x14` (stays at default during wpn2 or wpn3 attacks)

- **Ebisumaru weapon 2**
  - `4C = 0x1D`

### “No change” actions
- Ebisumaru camera / Yae flute: **no change**
  - (Meaning: 4C stays whatever it was, often `0x14`.)

---

## GOTCHAS (the stuff that makes mods “randomly stop”)

### 1) The “ctx gate” excludes weapon-1 (by design)

If your mod uses the strict gate:

- `ctx == 0x8016A090` AND `ctx+0x4C == 0x14`

…then **weapon 1 attacks** for Goemon/Ebisumaru (`4C=0x16`) and Sasuke/Yae (`4C=0x23`) will fail the gate.

That explains symptoms like:
- “pushback stays applied but moonjump/dmg stops”
- “only works until I teleport / swap / do weapon 1”

### 2) Debug teleport / boss warp can change `ctx` identity

You’ve observed that debug teleport / boss auto-teleport can make your “HardModeEnabled-gated” mods stop working.

Most likely causes (based on your own notes):
- `ctx` pointer changes (not equal to `0x8016A090` anymore), even if it still “looks like a ctx”
- `ctx+0x4C` value changes and never returns to `0x14` in the same room instance
- you’re temporarily in a different player/room controller object during warp

### 3) Door transitions “fix” it because pointers rebuild

When you walk through a door:
- room loads → objects rebuild → your expected ctx signature often returns
- so the mod “magically works again”

---

## Safer gating options (recommended)

If you still want safety *without* breaking half the game:

### Option A — Keep pointer safety, remove the strict signature
- Keep `is_rdram_ptr_u32(ctx)` and `is_rdram_ptr_u32(ctl)` checks
- Stop requiring `ctx == 0x8016A090`

### Option B — Gate on “valid ctl family”, not a single ctx
- Read `ctl = *(ctx+0x5C)` only if ctx is valid
- Require ctl to be valid AND have sensible changing fields (like `ctl+0x398` moves when camera moves)

### Option C — Expand the allowed `4C` set
If you want to keep `4C` as a mode discriminator:

- allow `{0x14, 0x16, 0x23, 0x1D}` (the values you’ve actually seen)
- optionally “lock” the allowed set per character after you detect who is active

---

## Quick test plan (to confirm which gotcha is hitting you)

1. Print `ctx`, `4C`, and `ctl` every frame (or when they change)
2. Teleport using debug menu
3. Watch:
   - Did `ctx` change permanently?
   - Did `4C` get stuck at a new value?
   - Did `ctl` become null/invalid (bad pointer)?
4. If only weapon-1 breaks things: it’s almost always the `4C != 0x14` gate.
