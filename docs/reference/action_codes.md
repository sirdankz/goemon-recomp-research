# $a1 action code mapping (from func_801DD830_599740)

This is your most valuable “what is the player doing right now?” classifier.

> **Status:** Confirmed by your live observations/logging. (We’ll promote to “code-confirmed” once we tie it to decomp/asm writes.)

## Common values (all characters)

- **`a1 = 3`** — default during **character change spawn smoke** (any character)
- **`a1 = 1`** — **jumping** (any character)

## Attacks / tools (weapon-slot dependent)

### Weapon slot 1 / 2 attacks
- **`a1 = 4`** — **Goemon / Sasuke / Ebisumaru** weapon 1 **or** weapon 2 attack (observed)
- **`a1 = 4`** — **Yae** weapon 1 attack (observed)

### Weapon slot 3 attacks (Goemon/Sasuke)
- **`a1 = 6`** — **Goemon / Sasuke** weapon 3 attack (observed)

### Tools (camera / flute)
- **No change observed** — **Ebisumaru camera** (tool)
- **No change observed** — **Yae flute** (tool)

## Goemon combo detail (observed)

- **`a1 = 0`** — Goemon 2nd attack during combo
- **`a1 = 6`** — Goemon 3rd attack during combo

## Extra older-session combo values (keep, but treat as “maybe different tap”)

From an earlier session you logged:
- weapon 1 solo: `a1 = 5`
- combo stop at 2nd: `a1 = 6`
- combo finish / 3rd: `a1 = 7`
- weapon 1 jump attack (“short”): `a1 = 8`

### Gotcha: “no change” isn’t “no event”
If camera/flute doesn’t produce a new `a1`, it might be:
1) not traversing DD830, OR
2) DD830 runs but `a1` remains constant, OR
3) that action is classified elsewhere.

**Don’t rely on `a1` alone** to detect camera/flute.
