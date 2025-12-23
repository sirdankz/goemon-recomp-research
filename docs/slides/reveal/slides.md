# Mystical Ninja Starring Goemon (Recomp) — Research Deck
### Quick tour of the **current confirmed facts** and where to look next

---

## Where to read the “full story”
- Website: https://sirdankz.github.io/goemon-recomp-research/
- Deep dive: https://sirdankz.github.io/goemon-recomp-research/deep_dive/
- Hooks: https://sirdankz.github.io/goemon-recomp-research/hooks/dd830/ • https://sirdankz.github.io/goemon-recomp-research/hooks/e3b9c/ • https://sirdankz.github.io/goemon-recomp-research/hooks/d98e0/

---

## Confirmed key addresses
- Buttons held: `0x800C7D3A` *(u16)*  
  - A=`0x8000` • B=`0x4000` • L=`0x0020`
- Weapon slot: `0x8015C604` *(u32)* (`1/2/3`)
- HP (half-hearts): `0x8015C5E7` *(u8)*

---

## Your two most important “behavior taps”
- `func_801DD830_599740` (DD830)  
  - counts up when attacking / jumping / changing characters  
  - `$a1` is your action code classifier
- `func_801E3B9C_59FAAC` (E3B9C)  
  - counts up on sudden stop + wpn1/2 attack paths  
  - clean RETURN-point to apply pushback

---

## `$a1` map highlights (DD830)
- `1` = jump
- `3` = spawn smoke / character swap
- `4` = wpn1/2 attacks (varies per character)
- `6` = wpn3 attacks (Goemon/Sasuke)
- Extra sessions: `5/6/7/8` sequence also observed for wpn1 phases

---

## `ctx+0x4C` map highlights
- default: `0x14` (spawn smoke)
- Goemon/Ebisumaru wpn1: `0x16`
- Sasuke/Yae wpn1: `0x23`
- Ebisumaru wpn2: `0x1D`

---

## The big gotcha: strict ctx gate breaks weapon-1 + teleports
If you require:
- `ctx == 0x8016A090` AND `ctx+0x4C == 0x14`

Then:
- weapon-1 (4C=16 or 23) fails gate
- debug teleports can change ctx identity
- door transitions “fix it” by rebuilding objects

---

## Next “best” research targets
- Identify the writer(s) for movement-lock flags during ground attacks
- Find a more stable ctx discriminator (vtable/tag/ctl-family check)
- Extend action code map with clean re-tests (one session, all chars)

---

## Repo quick links
- Repo: https://github.com/sirdankz/goemon-recomp-research
- `template_debug.c`: https://github.com/sirdankz/goemon-recomp-research/blob/main/template_debug.c
