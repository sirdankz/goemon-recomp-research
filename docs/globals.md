# Globals

This page maps your **on-screen overlay labels** to what they actually are.

> If a field is listed as “observed”, it means it came from your runtime notes/logs (not a guessed decomp name).

---

## Counters / snapshots

- `DD830` — call counter for `func_801DD830_599740` (counts up during attacking/jumping/character swap)
- `E3B9C` — call counter for `func_801E3B9C_59FAAC` (counts up on sudden-stop + wpn1/2 attack paths)
- `wpn.` — last read of `0x8015C604` (weapon slot 1–3; includes camera/flute slots depending on character)
- `a1` — last `$a1` observed inside `func_801DD830_599740`
- `a1_prev` — previous `$a1` (for transition detection)
- `ctx` — last `$a0` observed inside `func_801DD830_599740` (your “context pointer”)
- `4C` — last byte read from `ctx + 0x4C` (used as a “mode discriminator”)
- `buttons` — last read of `0x800C7D3A` (held bitfield)

---

## Fire coin / recoil state machine (overlay fields)

These are the fields you used to track “B-hold → ready → fire → recoil”.

- `BHold` — counts **up** while **B is held** and weapon slot is `3`  
  - Observed: “only while Goemon’s wpn3 is out”, but the code path is generally `wpn==3`.
- `B` — 1 whenever **B is pressed or held** (derived from the held-bitfield snapshot)
- `ready.` — becomes 1 after holding B long enough to charge (you observed ~3 seconds)
- `fire.` — 1 on the exact trigger moment after `ready.` when the throw condition happens (ex: airborne throw moment)
- `recoil` — becomes nonzero after `fire.=1`, then counts down when `func_801E3B9C_59FAAC` gets called (RETURN hook applies pushback)

---

## Important “gate” discriminator (ctx + 0x4C)

Your original “stability filter” was:

- `REQUIRED_CTX_PTR = 0x8016A090`
- `REQUIRED_CTX_4C  = 0x14`
- “Gate passes only if `(ctx == REQUIRED_CTX_PTR && rd8(ctx+0x4C) == 0x14)`”

This is **useful** for preventing bad pointer reads, but it has real gotchas (see: **Reference → ctx+0x4C map** and **Reference → Gotchas**).
