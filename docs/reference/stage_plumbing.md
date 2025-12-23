# Stage / data plumbing (confirmed RE findings)

These are the “stage content loader” facts we’ve confirmed so far.

## 1) Stage pointer table
- `D_80231300_5EC7D0` is indexed with `(stage_id * 4)` to load a pointer entry.
- This behaves like a stage-level pointer table.

## 2) StageActorMetadata layout (partial)
Access patterns observed:
- `u16` at `+0x14`
- function pointer at `+0x18` (invoked via `jalr`)

Interpretation: a stage actor metadata struct carries both IDs and a per-actor function callback.

## 3) Loader loop
`func_80013AC4_146C4`:
- iterates a `u16` list of IDs terminated by `0`
- calls a loader for each entry

This is a classic “load resources / actors by ID list” pattern.

## 4) Global init (not stage-specific)
`func_801F71D0_5B30E0` appears global/init:
- touches globals
- calls `osViSetSpecialFeatures`
So it’s likely not the right target for stage-specific behaviors.
