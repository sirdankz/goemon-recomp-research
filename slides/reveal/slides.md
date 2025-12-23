# Goemon Mod Debug Notes

---

## What this doc does
- Tracks **DD830** and **E3B9C**
- Detects airborne **fire coin throw**
- Arms a **recoil window**
- Applies **pushback** at the correct return hook

---

## Key addresses
- WEAPON_SLOT_ADDR = 0x8015C604
- BUTTONS_HELD_ADDR = 0x800C7D3A
- BTN_B = 0x4000
- REQUIRED_CTX_PTR = 0x8016A090
- REQUIRED_CTX_4C = 0x14

---

## Hooks
- DD830 (ENTRY): capture a0/a1 + classify
- E3B9C (ENTRY/RETURN): ctl + recoil apply
- D98E0 (RETURN): overlay + timers

---

## State machine
Gate → Detect transition → Arm ticks → Apply recoil

---

## Where to read next
- docs/deep_dive.md
- docs/hooks/dd830.md
- docs/hooks/e3b9c.md
