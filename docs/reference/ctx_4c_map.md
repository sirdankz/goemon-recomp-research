# ctx + 0x4C mapping (observed)

`ctx+0x4C` behaves like a small mode/substate discriminator.
It **defaults to 0x14** on character spawn smoke, and flips for certain attacks/tools.

> **Status:** Confirmed by your live observations/logging.

## Observed values

### Default / spawn
- **`0x14`** — default when any character is spawned behind smoke (character swap)

### Weapon slot 1
- **`0x16`** — Ebisumaru & Goemon weapon 1 attacks
- **`0x23`** — Sasuke & Yae weapon 1 attacks

### Weapon slot 2 / 3
- **`0x14`** — Sasuke & Goemon weapon 2 or weapon 3 attack (tends to stay 0x14 here)

### Other
- **`0x1D`** — Ebisumaru weapon 2 attack

### Tools
- **No change observed** — Ebisumaru camera
- **No change observed** — Yae flute

## Context gate in your mod (stability filter)

Your current gate is:
- `REQUIRED_CTX_PTR = 0x8016A090`
- `REQUIRED_CTX_4C  = 0x14`
- pass only if `(ctx == 0x8016A090 && rd8(ctx+0x4C) == 0x14)`

## Gotchas (important)

### 1) Teleports / boss-return can break the signature
Debug teleports or scripted teleports can preserve “gameplay” while changing the underlying `ctx` pointer
(or the 4C byte), causing the gate to fail silently.

### 2) Weapon 1 flips 4C away from 0x14
If you gate on `4C == 0x14`, weapon 1 attacks that set **0x16** or **0x23** get excluded.
This can produce “only some effects work” behaviors.

### 3) Prefer a soft gate while debugging
Instead of hard-failing, prefer:
- pointer sanity checks (valid RDRAM range)
- and logging `ctx` + `4C` during the broken case

Then you can promote the *new* signature once proven.
