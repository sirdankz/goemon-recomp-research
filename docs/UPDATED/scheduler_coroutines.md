# Scheduler and Coroutines

This page documents the scheduler system we’ve verified in `mnsg.us.0.z64`: how “next tick” callbacks are stored, executed, and why the game can jump into **mid-function labels** (coroutine-style).

Verified end-to-end proof strategy:

`scheduler setter → next-tick ptr stored → runner jalr executes → mid-function targets verified`

---

## What to remember

- The scheduler runs “next tick” functions via a stored function pointer.
- It can schedule **mid-function label addresses**, not just function starts.
- This is why you’ll see segptr-like values and “weird” targets that are actually valid.

---

## Core functions (verified)

### Scheduler setter
- `FUN_8003521C`
- Writes the next-tick pointer into: `*(DAT_8016DAB4 + 0x0C)`

### Scheduler runner
- `FUN_8003524C`
- Behavior (verified):
  - swaps `node+0x0C` ↔ `node+0x14` (current/next)
  - loads `t9 = *(node+0x0C)`
  - executes via `jalr t9`
  - restores state

---

## Coroutine targets (verified mid-function labels)

These are known scheduler targets that are *inside* functions:
- `0x80001240` inside `FUN_800011A0`
- `0x80001020` inside `FUN_80000D00`

This proves the scheduler is coroutine-capable (jump-in labels).

---

## Segptr rule (important for analysis)

We treat values shaped like `0x08xxxxxx` as “segptr-like” that resolve to VRAM `0x800xxxxx` (bytes match).

Why it matters:
- scheduled targets may appear as `0x08...` but really execute at `0x80...`
- jump targets may not align to a function start (they’re legitimate labels)

---

## Where this shows up (example: doors)

The overlay door consumer `FUN_803BC020` schedules follow-ups via `FUN_8003521C` with targets:
- `0x80001240` (twice)
- `0x80001020` (once)

That’s a concrete “scheduler in the wild” proof point.

---

## Practical mod hooks (safe entry points)

- Hook `FUN_8003521C` to log:
  - scheduled target address
  - callsite function
- Hook `FUN_8003524C` to confirm:
  - runner is executing scheduled target
  - frequency/ordering

---

## Tiny scheduler debug checklist

### 1) Are we scheduling anything?
- Count calls to `FUN_8003521C`.
- Log the target address.

### 2) Are scheduled targets executing?
- Count `FUN_8003524C` ticks.
- Log the executed `t9` target each time.

### 3) Do targets look “mid-function”?
- If targets don’t match function entrypoints, that’s OK.
- Confirm whether they match known labels or have valid code bytes.

### 4) Is your hook breaking timing?
- If scheduling happens but execution stops, your hook may be clobbering registers or stack.
