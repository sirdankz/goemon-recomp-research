# Stage/data plumbing (confirmed)

This is your “stage actor metadata + stage pointer table” lane — separate from combat hooks, but extremely useful for:
- stage-specific mods
- actor swaps
- loading extra data assets
- understanding why certain room transitions reset your state

---

## Confirmed pointer table: `D_80231300_5EC7D0`

- `D_80231300_5EC7D0` is indexed with `(stage_id * 4)` to load a pointer entry.
- That entry points to stage-specific data.

**Practical use:**  
If you can identify the stage_id source, you can build “if stage_id == X then patch Y” logic without relying on fragile room names.

---

## Confirmed StageActorMetadata layout

When the engine resolves an “actor metadata” entry, you confirmed these offsets:

- `+0x14` — `u16` field (ID/type-like)
- `+0x18` — function pointer (called via `jalr`)

**Practical use:**  
This is a clean “actor init/behavior trampoline.” If you ever want to intercept actor spawns without hooking a giant global update loop, this is a strong target.

---

## Confirmed loader iterator: `func_80013AC4_146C4`

- Iterates a `u16` list terminated by `0`
- Calls a loader for each entry

**Practical use:**
- “Inject one more resource ID into the list” is a common mod trick (careful with memory/limits).

---

## Confirmed global/init-ish function: `func_801F71D0_5B30E0`

- Touches globals
- Calls `osViSetSpecialFeatures`
- Not stage-specific

**Practical use:**  
Good place for “once per boot” debug toggles, global settings, feature flags — not stage logic.

---

## Gotchas

- This plumbing is stage/actor oriented. If your issue is “movement lock during attacks”, you’re still in the DD830/E3B9C lane.
- BUT: it explains why door transitions and warps change behavior (objects rebuilt, pointers replaced, lists reloaded).
