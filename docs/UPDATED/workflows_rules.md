# Workflows and Rules

This page documents the **proven workflows** and “don’t redo” rules we’ve agreed on while reverse-engineering and modding `mnsg.us.0.z64`.

Goal: avoid wasting time on dead ends and focus on the exact code that actually controls behavior.

---

## What to remember

- Don’t patch helper builders if a later per-frame integrator overwrites the values.
- Always identify the “physics truth” reader/writer that runs every frame.
- `jr` dispatch systems will not appear as nice JAL callgraphs—treat them as systems.

---

## Proven workflow: recoil / spread / impulse (“0x152 workflow”)

When chasing recoil, fan spread, or knockback:
1) Start from **real spawners** (not every candidate)
2) Use reads/writes index (or CSV) to find **per-frame integrators**
3) Patch behavior where motion is integrated, not where offsets are temporarily built

What to look for in “physics truth” functions:
- reads of `child+0x61` (index/variant)
- writes/reads of:
  - `actor+0x08`
  - `actor+0x0C`
  - `actor+0x10`
(these behave like pos/vel slots depending on the actor type)

---

## Don’t redo rule (hard rule)

**`FUN_801EE530` is a position offset builder, NOT the impulse/velocity setter.**

If you patch recoil there, it will often get overwritten later.

Where recoil belongs:
- velocity/acceleration setters, OR
- the per-frame integrator that updates position from velocity

Extra: often you must write paired fields (example pattern: `+0xA4` and `+0x6C`) to avoid one overwriting the other.

---

## Door proof strategy (repeatable)

When proving door/transition behavior:
1) link `obj+0xDA` consumer (`FUN_803BC020`)
2) link to `field77` writer (request bit)
3) confirm `FUN_801F7B0C` gate
4) confirm `FUN_801F7C14` door-enter chain
5) confirm state write (`FUN_801F7F78` sets state=4)

This keeps you from chasing stage-specific noise.

---

## Dispatch detection rule (Action6 + others)

When looking for “handler execution”:
- scan for `jr <any reg>` (not just `jr t9`)
- scan for nearby:
  - bounds check (`sltiu`)
  - table load (`lw reg, imm(base)`)

Treat wrappers + tables + jr-dispatch as a single system.

---

## Tiny workflow checklist (when you feel stuck)

- Am I patching something that gets overwritten later?
- Do I know the per-frame integrator that commits the behavior?
- Is there a `jr` dispatch that hides the real handler?
- Can I prove the chain end-to-end instead of guessing?
