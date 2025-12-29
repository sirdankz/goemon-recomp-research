# Door Transition System

This page documents the **door/transition system** we’ve verified in `mnsg.us.0.z64`: the key **flags/bytes**, **who writes/reads them**, and the exact **scheduler chain** that drives door entry.

Verified end-to-end proof strategy:

`obj+0xDA consumer → field77 writer → door gate → door enter chain`

---

## What to remember

- Doors/transitions are driven by a **request bit** (set `0x80`) and a **state byte** (state machine).
- Overlay door triggers flow through an object field (`obj+0xDA`) that is **latched + cleared** by a consumer, then kicked through the scheduler.

---

## Key bytes (field map)

### A) Transition request byte (door/transition request)
- **Symbol/field:** `g_system.field77`
- **Offset:** `g_system + 0x3AE29`
- **Meaning (verified):** bit `0x80` = “transition requested”
- **Notes:**
  - In code, you’ll see it referenced via immediates like `+0x3AE29` and also a signed form
    (observed imm `-0x51D7` after a base add with `0x40000`).

### B) Transition state byte (state machine step)
- **Symbol/field:** `g_system.field444`
- **Offset:** `g_system + 0x3B01D`
- **Meaning (partially verified):** “transition state”
- **Notes:**
  - This field has many touches (observed **writers=14**, **readers=10** in our notes),
    but we only treat as **verified** the key writer below.

### C) Overlay object door trigger byte
- **Field:** `obj + 0xDA`
- **Meaning (verified):** “door trigger fired / request pending” for that overlay object instance

### D) Overlay object latch flag
- **Field:** `obj + 0xEC`
- **Meaning (partially verified):** consumer sets bit `|= 4` when processing a door trigger  
  *(we haven’t fully mapped all bits in this field yet)*

---

## Writers / gates (verified)

### 1) Transition request bit `g_system.field77 |= 0x80`
- **Verified writers (set `0x80`):**
  - `FUN_80006140`
  - `FUN_800061B0`
  - `FUN_80037284`
- **Verified gate/consumer:**
  - `FUN_801F7B0C`
    - Checks `field77 & 0x80`
    - When set, branches into the door-enter chain starting at `FUN_801F7C14`

### 2) Transition state byte `g_system.field444`
- **Key verified writer:**
  - `FUN_801F7F78` sets transition state to **`4`** *(verified)*
- **Readers/writers (unverified list):**
  - We know this byte is touched by many functions (writers=14/readers=10 observed),
    but only the above writer is proven in our “canon map” notes.

---

## Main chain & door-enter subchain (verified)

main scheduled chain:
801F71D0 → 728C → 750C → 75C0 → 7690 → 770C → 77F4 → 7AA4 → 7B0C → 8428

door-enter subchain:
7C14 → 7E1C → 7F78 → 8290 → 82FC → back to 7690


- `FUN_801F7B0C` is the **door-enter gate** (checks `field77 & 0x80`)
- `FUN_801F7F78` is where we confirmed: **transition state gets set to `4`**

---

## Overlay door trigger pipeline (verified)

- **Producers (write `obj+0xDA`):**
  - `FUN_801FB1C0` *(param1==0x10 gate)*
  - `FUN_8021D724`
  - `FUN_802239B0`

- **Consumer:** `FUN_803BC020`
  - if `obj+0xDA != 0`:
    - sets `obj+0xEC |= 4`
    - clears `obj+0xDA = 0`
  - schedules coroutines via `FUN_8003521C` with A0 targets:
    - `0x80001240` (twice)
    - `0x80001020` (once)

---

## Scheduler mechanics (why doors behave like coroutines)

### Scheduler setter
- `FUN_8003521C`
  - Writes a “next tick” function pointer into `*(DAT_8016DAB4 + 0x0C)`

### Scheduler runner
- `FUN_8003524C`
  - Swaps current/next pointers (`node+0x0C`, `node+0x14`)
  - Executes the scheduled function via `jalr` of the function pointer
  - Restores state afterwards

### Coroutine targets (verified mid-function labels)
These addresses are real “jump-in” labels used by the scheduler:
- `0x80001240` is inside `FUN_800011A0`
- `0x80001020` is inside `FUN_80000D00`

### Segptr rule (important for analysis)
We treat `0x08xxxxxx` pointers as “segptr-like” that resolve to VRAM `0x800xxxxx` (bytes match).  
This matters because door/scheduler code can jump to **mid-function labels**.

---

## Practical mod hooks (safe entry points)

### A) Observe transition requests
Hook any writer:
- `FUN_80006140`, `FUN_800061B0`, `FUN_80037284`  
Log when `field77 |= 0x80` happens.

### B) Force/deny door enter
Hook the gate:
- `FUN_801F7B0C`  
You can:
- deny transitions by clearing `field77 & 0x80`
- force transitions by setting it *(careful: system must be in a valid state)*

### C) Observe transition state machine
Hook:
- `FUN_801F7F78`  
Log when state is set to `4` and what led there.

### D) Overlay door trigger flow
Hook:
- `FUN_803BC020`  
This is where `obj+0xDA` is consumed/cleared and follow-up ticks are scheduled.

---

## TODO (not fully mapped yet)

- Full enum of `g_system.field444` transition states *(we only verified state=`4`)*
- Full meaning of bits in `obj+0xEC` *(we only verified `|=4` is used here)*
- Complete list of all writers/readers for `field444` with proof *(beyond the one key writer)*

---

!!! note "Why this matters"
    This is a clean “system boundary” where mods can hook a single point (consumer, gate, state writer)
    and reliably affect transitions without chasing dozens of stage-specific call sites.
