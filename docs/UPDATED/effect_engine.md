# Effect Engine / Particle System

This page documents the **reusable effect/particle mini-engine** patterns we’ve been mapping in `mnsg.us.0.z64`. The goal is to help modders quickly recognize “this is the effect system,” find the **real spawners**, and patch the **per-frame integrators** instead of getting stuck on helper builders.

---

## What to remember

- Many visual effects are not one-off code — they’re instances driven by a shared mini-engine:
  - **spawn/init** (alloc + set defaults)
  - **per-frame update** (physics truth / integration)
  - **draw** (render step)
  - **kill/free** (lifetime end)
- The single most important step is identifying the **per-frame truth** updater(s).
  - If you patch only an init/helper, the update loop may overwrite your values every frame.

---

## Recognizing the engine (common patterns)

Look for functions that match these shapes:

### A) Spawner / initializer (high signal)
- Called from many places.
- Allocates or grabs a slot.
- Writes a block of fields (position, velocity, lifetime, flags).
- Often takes:
  - an “effect type/id”
  - position vector
  - rotation/scale
  - parent actor pointer

### B) Per-frame update / integrator (highest signal)
- Runs every frame (or every N frames).
- Reads current state and writes updated state:
  - position = position + velocity
  - velocity += accel
  - lifetime--
  - flags change based on collisions/timers
- Often contains:
  - clamp/threshold checks
  - “kill when lifetime == 0”
  - branchy “state machine” per effect subtype

### C) Draw / render
- Builds display lists.
- Reads final positions/angles/scales.
- Often uses shared render helpers.

### D) Free / despawn
- Clears active flag or returns slot to pool.
- Sometimes chains into “spawn burst on death” effects.

---

## Proven workflow (don’t waste time)

### 1) Start from real spawners (not candidates)
When you have a list of “maybe spawners,” pick the ones that are:
- called from multiple gameplay contexts
- clearly allocate/initialize an instance

### 2) Find “physics truth” readers/writers
Use your reads/writes index (or CSV exporter) to identify:
- functions that read motion fields every frame
- functions that write the same fields every frame

That’s where behavior is actually implemented.

### 3) Patch where motion is integrated
When chasing recoil/spread/knockback-style behavior:
- target the velocity/accel setter or per-frame integrator
- do not patch a temporary offset builder that gets overwritten

**Hard rule:** `FUN_801EE530` is an offset builder, not the impulse setter.

---

## What we’ve verified that applies to effect-like systems

Even when we’re not in the effect engine directly, the same principles apply:

- **Per-frame integrators overwrite init helpers.**
- Many systems are hidden behind dispatch:
  - `jr` jump tables (no clean callgraph)
  - scheduler/coroutines (mid-function label targets)
- Some “tables” exist without xrefs (loaded indirectly).

---

## Practical mod hooks (generic)

If you want quick visibility without full decomp:
- Hook the likely **spawner** and print:
  - effect type/id
  - position
  - lifetime
  - pointer to instance
- Hook the likely **per-frame update** and print:
  - position/velocity deltas
  - lifetime transitions
  - state machine byte changes
- Hook the **despawn** path and log which effects die and why

---

## Tiny effect engine debug checklist

### 1) Are we spawning instances?
- Confirm spawner is being called.
- Confirm “active flag” flips on.
- Confirm instance pointer/slot index is stable.

### 2) Is per-frame update running?
- Count per-frame update calls.
- Log one instance’s position each frame.

### 3) Are we patching the right layer?
- If your change “works for 1 frame then resets”:
  - you patched init/helper
  - update loop overwrote it

### 4) Is there hidden dispatch?
- If you can’t find the handler in callgraph:
  - scan for `jr <reg>` dispatch
  - look for bounds checks (`sltiu`) near table loads

### 5) Is scheduler involved?
- If behavior happens after delays:
  - hook scheduler setter/runner and watch queued targets
