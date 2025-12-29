# Door Transition System

Verified end-to-end proof strategy:

`obj+0xDA consumer → field77 writer → door gate → door enter chain`

## Key bytes
- **Transition request:** `g_system.field77` @ `+0x3AE29` (written with `0x80`)
- **Transition state:** `g_system.field444` @ `+0x3B01D`

## Writers / gates
- Writers of request (`0x80`): `FUN_80006140`, `FUN_800061B0`, `FUN_80037284`
- Gate: `FUN_801F7B0C` checks `field77 & 0x80` then branches to `FUN_801F7C14`
- Key state writer: `FUN_801F7F78` sets transition state = `4`

## Main chain & door-enter subchain
```
main scheduled chain:
801F71D0 → 728C → 750C → 75C0 → 7690 → 770C → 77F4 → 7AA4 → 7B0C → 8428

door-enter subchain:
7C14 → 7E1C → 7F78 → 8290 → 82FC → back to 7690
```

## Overlay door trigger pipeline
- Producers (write `obj+0xDA`): `FUN_801FB1C0` (param1==0x10 gate), `FUN_8021D724`, `FUN_802239B0`
- Consumer: `FUN_803BC020`:
  - if `obj+0xDA != 0`: sets `obj+0xEC |= 4`, clears `obj+0xDA`
  - schedules coroutines via `FUN_8003521C` with A0 targets `0x80001240` (twice) and `0x80001020` (once)

!!! note "Why this matters"
    This is a clean “system boundary” where mods can hook a single point (consumer, gate, state writer)
    and reliably affect transitions without chasing dozens of stage-specific call sites.
