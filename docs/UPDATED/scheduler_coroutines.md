# Scheduler & Coroutines

Verified: scheduler runs a function pointer, and can jump to mid-function labels (“coroutines”).

## Core mechanics
- **Setter:** `FUN_8003521C` writes next-tick ptr to `*(DAT_8016DAB4+0x0C)`
- **Runner:** `FUN_8003524C` swaps `*(node+0x0C)` → `*(node+0x14)`, then:
```
lw t9,0x0C(a0)
lw a1,0x18(a0)
jalr t9
```

## Coroutine labels (mid-function targets)
- `0x80001240` inside `FUN_800011A0`
- `0x80001020` inside `FUN_80000D00`

!!! important "Segptr handling"
    Treat `0x08xxxxxx` as a possible mid-function target that can resolve to VRAM `0x800xxxxx`
    with identical bytes in this project snapshot. This matters when jump tables or scheduler targets look “weird”.
