---
marp: true
title: Goemon Mod Debug Notes
paginate: true
---

# Goemon Mod Debug Notes
### Table-of-Contents / Slide Walkthrough

---

## What this doc does
- Tracks **DD830** and **E3B9C** behavior
- Detects airborne **fire coin throw moment**
- Arms a **recoil window**
- Applies **pushback** by writing a negative float on the right return hook

---

## Key addresses
- WEAPON_SLOT_ADDR = 0x8015C604 (wpn. 1–3)
- BUTTONS_HELD_ADDR = 0x800C7D3A
- BTN_B = 0x4000
- REQUIRED_CTX_PTR = 0x8016A090
- REQUIRED_CTX_4C = 0x14

---

## Hook map
- **DD830 (ENTRY)**: capture (a0/a1), classify a1 transitions, arm recoil
- **E3B9C (ENTRY/RETURN)**: find ctl, apply recoil at RETURN, decrement ticks
- **D98E0 (RETURN)**: per-frame overlay + timers

---

## State machine (high level)
- Gate context (ctx pointer + ctx+0x4C)
- Watch a1 transition
- Set recoil ticks
- Apply recoil at correct return hook

---

## Call graph snapshot
(See docs → Call graph page)

---

## Practical debugging checklist
- Verify ctx gate still valid after teleports/character swaps
- Confirm hooks still firing (counters increment)
- Confirm ctl pointer is valid before writing

---

## Next additions
- Add “ctx signature drift” logging
- Log writer sites for ctl fields if needed
- Split per-character state behavior
