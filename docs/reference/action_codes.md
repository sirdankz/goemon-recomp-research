# Action codes (`$a1`) map (DD830)

This page consolidates every `$a1` observation you’ve logged **in one place**, including character-specific exceptions.

> Source of truth: your runtime overlay + notes while hooking `func_801DD830_599740` (“DD830”).

---

## The “base” mappings you confirmed (most consistent)

- `a1 = 0x00000003` — default during **character swap / spawn smoke**
- `a1 = 0x00000001` — **jump** (any character)
- `a1 = 0x00000004` — **weapon 1 / weapon 2 attack** for:
  - Goemon
  - Sasuke
  - Ebisumaru
  - Yae (wpn1)
- `a1 = 0x00000006` — **weapon 3 attack** (Goemon + Sasuke)

---

## Character / weapon exceptions you explicitly noted

### Ebisumaru
- Camera use: **no change** (doesn’t flip to the typical attack codes)

### Yae
- `a1 = 0x00000004` — Yae wpn1 attack
- Flute use: **no change**

### Goemon combo notes (important)
You observed combo-specific deviations:
- `a1 = 0x00000000` — Goemon **2nd attack** during combo (session observation)
- `a1 = 0x00000006` — Goemon **3rd attack** during combo (session observation)

---

## Other `$a1` sequences you saw in a different session (keep, don’t lose)

In another run you reported a different set while tracking weapon-1 sequences:

- wpn1 (no combo): `a1 = 0x00000005`
- stop combo on 2nd hit: `a1 = 0x00000006`
- finish combo + do 3rd: `a1 = 0x00000007`
- jump-attack (weapon 1 short attack): `a1 = 0x00000008`

**Status:** not yet reconciled with the newer “0/4/6” combo notes.  
This could be:
- a different callsite / different phase of the state machine,
- a different hook point / build,
- or an overlay update where the meaning of “a1” changed.

We keep both until you re-test them in one clean session.

---

## Why this matters (practical modding)

If you want “do X only on *this* action”, `$a1` is the best high-level discriminator you currently have — but it’s **not globally unique** across all phases.

- Best practice: use `$a1` **plus** one of:
  - `weapon_slot` (`0x8015C604`)
  - `ctx+0x4C` (mode discriminator)
  - a short timing window (like your `pending` window)

So your trigger conditions stay stable across characters and transitions.
