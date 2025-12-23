# MNSG Recomp — Master Summarized Notes + Gotchas

---

## Status legend

- **Confirmed (code/doc):** explicitly implemented or stated in your `pasted.txt` mod file
- **Confirmed (your logs):** repeatedly observed by you in-game
- **Strong working theory:** fits evidence but not yet tied to a single engine field write/read

---

## 0) Quick “what the overlay means”

- **DD830** = counts up when attacking, jumping, changing characters *(your logs)*
- **E3B9C** = counts up when coming to a sudden stop OR using wpn1/2 atk *(your logs)*  
  - both route through `func_801E3B9C_59FAAC` *(your logs)*
- **wpn.** = current weapon out (1–3), includes flute and camera and are in order *(your logs)*
- **a1** = action / reason code inside `func_801DD830_599740` *(your logs)*
- **4C** = `rd8(ctx+0x4C)` state byte that flips by character/action *(your logs)*
- **BHold** = counts up when holding **B** only while Goemon’s wpn3 is out *(your logs)*
- **ready.** = 1 when holding **B** to charge fire coin (~3 seconds) *(your logs)*
- **fire.** = 1 after `ready.=1`, then throwing fire coin in the air *(your logs)*
- **recoil** = becomes nonzero after `fire.=1`, then counts down when `func_801E3B9C_59FAAC` gets called *(your logs)*

---

## 1) Key addresses (input / combat / status)

### Input / control

- `0x800C7D3A` (**u16**) = held input bitfield *(code/doc)*
  - **BTN_B** = `0x4000` *(code/doc)*
  - **BTN_A** = `0x8000` *(your logs)*
  - **BTN_L** = `0x0020` *(your logs)*

### Player combat / status

- `0x8015C604` (**u32**) = current weapon slot (**wpn.**) observed `1/2/3` *(code/doc + your logs)*
- `0x8015C5E7` (**u8**) = HP in half-hearts *(your logs; matches your GameShark note)*

---

## 2) Engine entrypoints / hook tap points (what you’re watching)

### Primary action classifier / state tap

- `func_801DD830_599740` (**ENTRY**) — “DD830”
  - High-level “player is doing something” path.
  - Captures **ctx** (`$a0`) and **action code** (`$a1`).
  - Proven: `$a1` is meaningful and action-coded.

### Stop/attack transition / push scalar tap

- `func_801E3B9C_59FAAC` (**ENTRY + RETURN**) — “E3B9C”
  - Sudden stop and many attack transitions route here *(your logs)*.
  - This is also where **recoil** is applied (writes a float into `ctl+0x04`) *(code/doc)*.

### Per-frame overlay + timers

- `func_801D98E0_5957F0` (**RETURN**)
  - Runs B-hold / ready / pending timers and draws overlay *(code/doc)*.

### Text draw helper (callee)

- `func_800042AC_4EAC(entry, str, x, y)` — overlay text draw *(code/doc)*.

---

## 3) On-screen fields → real globals (1:1 map)

### Counters / snapshots *(code/doc)*

- **DD830** → `g_dd830_calls`
- **E3B9C** → `g_e3b9c_calls`
- **wpn.** → `g_last_weapon_slot` (read from `0x8015C604`)
- **a1_prev** → `g_prev_a1`
- **a1** → `g_last_a1`
- **ctx** → `g_last_ctx_ptr`
- **4C** → `g_last_ctx_4c` (`rd8(ctx+0x4C)` when ctx looks valid)
- **buttons** → `g_last_buttons` (`rd16(0x800C7D3A)`)

### Fire coin / recoil state machine *(code/doc; behavior confirmed by your logs)*

- **BHold** → `g_b_hold_frames`
- **ready.** → `g_charge_ready`
- **pending** → `g_pending`
- **fire.** → `g_fire`
- **recoil** → `g_recoil_ticks`

### E3B9C-side debug fields *(code/doc)*

- **ctl** → `g_e3b9c_ctl`
- **pre4** → `g_e3b9c_pre4`
- **post4** → `g_e3b9c_post4`
- **ap** → `g_e3b9c_applied`

> Note: One overlay line may be blank if the backing slot isn’t filled (intentional in the file).

---

## 4) `$a1` action-code map (includes Yae + combo details)

### Baseline / shared meanings *(your logs)*

- `a1 = 3` : default when any character spawns behind smoke / character swap
- `a1 = 1` : jump (any character)
- `a1 = 4` : wpn1 or wpn2 attack for **Ebisumaru, Sasuke, Goemon**
- `a1 = 6` : **Sasuke & Goemon** wpn3 attack

### Character-specific notes *(your logs)*

- **Ebisumaru camera**: *no change* in `a1`
- **Yae wpn1 attack**: `a1 = 4`
- **Yae flute**: *no change* in `a1`

### Goemon combo detail *(your logs)*

- **Goemon 2nd attack in combo**: `a1 = 0`
- **Goemon 3rd attack in combo**: `a1 = 6`

#### Gotcha A — “no change” in a1
If camera/flute doesn’t produce a new `a1`, it might:
1) not traverse the same DD830/a1-coded path, OR  
2) DD830 still runs but a1 stays constant, OR  
3) that action doesn’t hit DD830 at all at that moment.  
**Don’t rely on `a1` alone** to detect those actions.

---

## 5) `ctx+0x4C` state byte (full map) + gate risks

### Your observed 4C values *(your logs)*

- **Default**: `4C = 0x14` when any character spawns behind smoke
- `4C = 0x16` : **Ebisumaru & Goemon** wpn1 attacks
- `4C = 0x14` : **Sasuke/Goemon** wpn2 or wpn3 attacks
- `4C = 0x23` : **Sasuke & Yae** wpn1 attack
- `4C = 0x1D` : **Ebisumaru** wpn2 attack
- **Ebisumaru camera / Yae flute**: *no change* in 4C

### Context gate (stability filter) *(code/doc)*

- `REQUIRED_CTX_PTR = 0x8016A090`
- `REQUIRED_CTX_4C  = 0x14`
- Gate passes only if:
  - `(ctx == 0x8016A090) && (rd8(ctx+0x4C) == 0x14)`

#### Gotcha B — Gate can block the exact windows you care about
If weapon-1 flips 4C to `0x16/0x23/0x1D` **and** your gate demands `0x14`,
then those attack windows can be excluded even though you’re “in the right place”.

#### Gotcha C — Teleports / boss-return can break ctx identity
Debug teleports or scripted teleports can produce “equivalent gameplay” but a different
ctx pointer or 4C signature, making the gate fail and the detector “die quietly”.

**Practical gate strategies** *(strong working theory)*:
- Allow-list multiple ctx pointers observed after teleports.
- Allow 4C in `{0x14,0x16,0x23,0x1D}` for relevant action windows.
- Gate on **structure sanity** (valid `ctl`, valid ranges) rather than a single ctx magic.

---

## 6) Fire coin / recoil pipeline (merged with your wording)

### Your behavior summary *(your logs; backed by mod logic)*

- **BHold** starts counting up when holding **B** only while Goemon’s wpn3 is out.
- **B = 1** whenever B button is pressed or held.
- **ready. = 1** after charging fire coin (~3 seconds).
- **fire. = 1** after `ready.=1`, when throwing fire coin in the air.
- **recoil** becomes nonzero after `fire.=1`, then counts down when `func_801E3B9C_59FAAC` gets called.

### Under the hood (code/doc phrasing that matches your observed behavior)

- Per-frame (in `func_801D98E0` return):
  - if `wpn==3` and B held → `BHold++`
  - when BHold passes threshold → `ready=1`
  - when B released after ready → `pending` window starts
- Trigger moment (in DD830 entry hook):
  - `fire=1` and `recoil_ticks` armed only on a specific `a1` transition + gate logic
- Apply moment (in E3B9C return hook):
  - while `recoil_ticks>0`: write `ctl+0x04 = -0.8f` and decrement `recoil_ticks`

#### Gotcha D — fire. is supposed to “flash”
`fire=1` is intentionally a one-moment latch. A brief blink is expected.

#### Gotcha E — recoil only counts down when E3B9C runs
If recoil is armed but E3B9C isn’t being called in that situation,
recoil won’t apply or tick down.

---

## 7) CTL block probes you verified (camera / movement fields)

### ctl discovery *(code/doc)*

- `ctl = *(ctx + 0x5C)` is read when E3B9C is hit.
- recoil write target: `*(ctl + 0x04)` (push/scalar float slot)

### Verified camera/movement offsets *(your logs)*

- `ctl + 0x398` : camera left/right/zoom; freezes when still
- `ctl + 0x3A0` : camera pitch; look-only zoom mode freezes at `0x41A00000`
- `ctl + 0x39C`, `ctl + 0x3A4` : movement-related; freeze when no movement/camera

Conclusion: these are camera/movement controller floats, not attack-lock flags.

---

## 8) Stage / data plumbing (from our earlier confirmed RE work)

- `D_80231300_5EC7D0` = pointer table indexed by `(stage_id * 4)`
- `StageActorMetadata`:
  - `u16` at `+0x14`
  - function pointer at `+0x18` (called via `jalr`)
- `func_80013AC4_146C4` iterates a `u16` list terminated by `0`; calls a loader per ID
- `func_801F71D0_5B30E0` looks global/init (touches globals, `osViSetSpecialFeatures`), not stage-specific

---

## 9) Call graph (who calls what)

Engine calls:
- `func_801DD830_599740`
  → your DD830 ENTRY hook (captures ctx/a1/wpns/buttons + can arm fire/recoil)

- `func_801E3B9C_59FAAC`
  → your E3B9C ENTRY hook (find ctl + read pre4)
  → your E3B9C RETURN hook (apply recoil write + decrement)

- `func_801D98E0_5957F0`
  → your RETURN hook (per-frame timers + overlay draw)

Overlay draw uses:
- `func_800042AC_4EAC(entry, str, x, y)`

---

## 10) Gotcha checklist (why things “stop working”)

If detection/arming breaks after debug teleport / boss return teleport:
- [ ] ctx pointer changed (not `0x8016A090`) → gate fails silently
- [ ] `4C != 0x14` at that moment (`0x16/0x23/0x1D` during attacks) → gate fails
- [ ] wpn not 3 → BHold/ready/pending reset behavior kills charging path
- [ ] fire only flashes by design → don’t expect it to stay 1
- [ ] recoil armed but E3B9C not running → recoil won’t apply/count down

---

## 11) What’s still not pinned (don’t mark as fact yet)

- Overlay labels from other experiments (k, mv, t, atk, A/B offset hunting, etc.)
  unless each is tied to a concrete variable/offset read/write site.
