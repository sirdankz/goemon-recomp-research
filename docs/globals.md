## Global state variables (what each on-screen field really is)

> This page maps the on-screen overlay fields to what they represent in memory / logic.

### Counters / last-seen snapshots
- `DD830` → counts entries into `func_801DD830_599740`
- `E3B9C` → counts entries/returns for `func_801E3B9C_59FAAC`
- `wpn.` → last read of `0x8015C604` (weapon slot 1–3)
- `buttons` → last read of `0x800C7D3A` (held input bitfield)
- `a1_prev / a1` → `$a1` transition tracking captured in DD830
- `ctx` → `$a0` captured in DD830 (treated as context pointer)
- `4C` → `rd8(ctx + 0x4C)` (if ctx looks valid)

### Fire coin / recoil pipeline (observed + matches the current debug overlay mod)
- `BHold` → counts up while **weapon slot 3** is active and **B is held**
- `ready.` → becomes `1` after ~3 seconds of holding B (charge threshold)
- `fire.` → becomes `1` on the actual “throw” moment (your detector uses an a1 transition)
- `recoil` → becomes nonzero after `fire.=1`, then counts down when `func_801E3B9C_59FAAC` runs (because recoil is decremented when the write is applied)

### What “B” means on the overlay
- `B = 1` whenever **B is pressed or held** (derived from the held bitfield’s `BTN_B` flag)

### “ctl” pointers / scalars shown in the overlay
- `ctl` → dereferenced from `ctx + 0x5C`
- `pre4 / post4` → value at `ctl + 0x04` before/after our write (pushback scalar)
- `ap` → “applied” flag (did we actually write in the return hook this frame?)

---

## What we *do not* claim as confirmed yet
Some labels you’ve used in earlier overlays (examples: `k`, `mv`, `t`, `atk`, “A/B offsets”) clearly behave like timers/counters,
but we **haven’t pinned them to exact struct offsets** with a reliable writer/reader chain yet.
When we do, they should move from “notes” into `addresses.md` / `hooks/*`.

---

See also: [Master summarized](master_summarized.md)
