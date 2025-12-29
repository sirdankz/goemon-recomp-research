# Workflows & Rules

## 0x152 recoil / fan-spread workflow
1. Start from real spawners (`type_0x152_candidates`)
2. Use `offset_readers_writers.csv` to find per-frame “physics truth” readers
3. Implement changes where motion is integrated:
   - reads of `child+0x61` (index)
   - writes/reads of `actor+0x08/+0x0C/+0x10` (pos/vel style slots)

## Don’t redo rule
!!! warning "FUN_801EE530 is not the impulse setter"
    It’s a position offset builder.  
    Recoil belongs in velocity/acceleration setters or in the per-frame integrator.  
    Often write paired fields (`+0xA4` and `+0x6C`) to avoid overwrites.

## Door proof strategy (repeatable)
Link: `obj+0xDA consumer → field77 writer → FUN_801F7B0C gate → FUN_801F7C14 branch`
