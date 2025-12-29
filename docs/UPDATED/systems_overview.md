# Systems Overview

This is the “systems map” entry point. Every section links to a deeper page.

## Door / Transition system
- Transition-request byte: `g_system.field77` @ `+0x3AE29` (written with `0x80`)
- Transition-state byte: `g_system.field444` @ `+0x3B01D` (key writer sets state=4)
- Door enter gate: `FUN_801F7B0C` checks `field77 & 0x80`, branches to `FUN_801F7C14`
- Overlay door trigger pipeline: producers write `obj+0xDA`, consumer latches and schedules coroutines

→ [Full door / transition details](door_transition_system.md)

## Action6 system (ctx+0x60)
- Index: `ctx+0x60` (u8)
- Wrappers: `0x801E132C`, `0x801E13D0`, `0x801E1414`
- Tables: `0x80204340` (a1), companions `0x80204370` / `0x80204350` / `0x80204390`
- Dispatch: `FUN_801E54E0` then tail-jump path via jr-table at `0x801E70D0`

→ [Full Action6 details](action6_system.md)

## Scheduler & coroutines
- Setter: `FUN_8003521C` writes next-tick ptr to `*(DAT_8016DAB4+0x0C)`
- Runner: `FUN_8003524C` does `jalr t9` to the tick function pointer
- Coroutine labels: can jump to mid-function addresses (e.g. `0x80001240`, `0x80001020`)

→ [Full scheduler details](scheduler_coroutines.md)
