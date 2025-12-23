# Gotchas & stability notes (things that break mods)

This is the “why did my mod turn off?” page.

---

## 1) Debug teleport / boss warp breaks strict pointer signatures

If you rely on a hard-coded ctx signature like:

- `ctx == 0x8016A090`
- `ctx+0x4C == 0x14`

…teleports can easily invalidate it.

**Symptom you described:**  
HardModeEnabled-gated mods stop working after teleport, but then “come back” after walking through a door.

**Likely reason:**  
Warp temporarily uses a different controller object; door reload rebuilds your expected one.

---

## 2) Weapon-1 flips `ctx+0x4C` (so your gate can accidentally block weapon-1)

You confirmed:
- Goemon/Ebisumaru wpn1 → `4C = 0x16`
- Sasuke/Yae wpn1 → `4C = 0x23`

So any gate expecting `4C==0x14` will block these, even if everything else is correct.

---

## 3) “Sticky effects” vs “per-frame effects”

You noticed:
- **pushback stays applied** but **moonjump/dmg disable**

That pattern often means:
- pushback wrote a value once into a field that persists (sticky)
- moonjump/dmg are applied per-frame (need the hook to keep running)
- when your gate fails, per-frame effects stop immediately, sticky writes remain

---

## 4) Fix strategy (minimal invasiveness)

If you want stability without losing safety:

- Keep pointer validity checks (`is_rdram_ptr_u32`)
- Treat `ctx==0x8016A090` as “nice to have”, not required
- Expand `4C` allowed values to the ones you’ve actually observed
- Or: identify the “real” stable discriminator (like a vtable pointer, struct tag, or ctl-family sanity check)

---

## 5) When in doubt: log on change

The fastest way to diagnose breakage:

- print `ctx`, `4C`, `ctl`, `weapon_slot`, `$a1`
- only when they change (so it’s readable)
- teleport / boss warp / door transition and watch what flips
