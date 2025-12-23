# Table of Contents (deep-dive)

## What this file is trying to do (one sentence)


### Updated facts & cross-links (always current)

If you only read one page: **Reference → Master summarized**  
- [Master summarized](../reference/master_summarized/)

Quick links to the tables that change most often:
- [$a1 action codes (character/action mapping)](../reference/action_codes/)
- [ctx+0x4C map + gotchas](../reference/ctx_4c_map/)
- [Stage plumbing (pointer tables + metadata offsets)](../reference/stage_plumbing/)

Key confirmed addresses (quick view):
- Inputs: `0x800C7D3A` (u16 held bitfield) → **A=0x8000, B=0x4000, L=0x0020**
- Weapon slot: `0x8015C604` (u32) → 1/2/3
- HP (half-hearts): `0x8015C5E7` (u8)

Hooks used by this mod:
- `func_801DD830_599740` (ENTRY) — DD830 classifier / trigger arming
- `func_801E3B9C_59FAAC` (ENTRY + RETURN) — ctl discovery + recoil apply at RETURN
- `func_801D98E0_5957F0` (RETURN) — per-frame overlay + timers


## Key addresses & engine entrypoints

## Global state variables (what each on-screen field really is)

## Helper utilities

### 4.1 rd8/rd16/rd32

### 4.2 is_rdram_ptr_u32 (pointer safety)

### 4.3 ctx_gate_ok (your ctx + 4C discriminator)

### 4.4 string builders (u32_to_hex, append_*)

## Hook #1: func_801DD830_599740 (ENTRY) — “DD830”

### 5.1 What it captures (a0/a1, weapon slot, buttons, ctx+4C)

### 5.2 Your “fire coin throw in air” detector (a1 transition logic)

### 5.3 What it arms (recoil ticks) vs what it just displays (fire flag)

## Hook #2: func_801E3B9C_59FAAC (ENTRY/RETURN) — “E3B9C”

### 6.1 How it finds ctl (ctx+0x5C)

### 6.2 Why writing ctl+0x04 here matters (frame-order correctness)

### 6.3 Why recoil counts down only here (matches your observation)

## Hook #3: func_801D98E0_5957F0 (RETURN) — per-frame overlay + B-hold timer

### 7.1 B-hold → ready + pending window

### 7.2 What each overlay line prints

## Call graph (callers / callees)

Your observed fields mapped to this code (1:1)

## Discoveries / connections from your notes

## Updated facts & cross-links (additive)

This page is a **deep dive into the recoil/overlay mod path**. For the complete, always-updated research pages (tables + gotchas + all confirmed mappings), jump here:

- **Master summarized:** [master_summarized.md](master_summarized.md)
- **Addresses:** [addresses.md](addresses.md)  |  **Globals:** [globals.md](globals.md)
- **$a1 action codes:** [reference/action_codes.md](reference/action_codes.md)
- **ctx + 0x4C map + gotchas:** [reference/ctx_4c_map.md](reference/ctx_4c_map.md)
- **Stage plumbing / pointer tables:** [reference/stage_plumbing.md](reference/stage_plumbing.md)

### Quick address table (confirmed)

| Item | Address | Type | Notes |
|---|---:|---|---|
| Buttons held bitfield | `0x800C7D3A` | u16 | Bits: A=`0x8000`, B=`0x4000`, L=`0x0020` |
| Weapon slot (wpn.) | `0x8015C604` | u32 | Observed values `1/2/3` (includes camera/flute slot ordering) |
| HP (half-hearts) | `0x8015C5E7` | u8 | Matches your GameShark note for “infinite health” behavior |
| Required ctx pointer (current gate) | `0x8016A090` | ptr | Used only if you enable the strict ctx gate |
| Required ctx+0x4C (current gate) | `0x14` | u8 | Default smoke-spawn state; see full map below |

### Gotchas (why detection randomly “dies”)

- **Debug teleports / boss-return teleports** can change the ctx pointer or the meaning/value of `ctx+0x4C`, so a strict gate (`ctx==0x8016A090 && 4C==0x14`) can fail silently. See: [reference/ctx_4c_map.md](reference/ctx_4c_map.md)
- **Weapon-1 attacks** can flip `ctx+0x4C` away from `0x14` for some characters (ex: `0x16`, `0x23`, `0x1D`), so the strict gate can exclude the exact windows you care about. See: [reference/ctx_4c_map.md](reference/ctx_4c_map.md)


## Full source code (verbatim)

### 1) What this file is trying to do

It builds a debug overlay that:

watches DD830 to detect the airborne fire coin throw moment (using a1 transitions + ctx discriminator), then

arms a short recoil window, and

applies the actual pushback by writing a negative float into ctl+0x04 inside E3B9C’s RETURN hook, counting down each time it successfully applies.

### 2) Key addresses & engine entrypoints
## Raw memory addresses

WEAPON_SLOT_ADDR = 0x8015C604 → your wpn. (1–3)

BUTTONS_HELD_ADDR = 0x800C7D3A → held buttons bitfield

BTN_B = 0x4000

“Context gate” discriminator (your known-good signature)

REQUIRED_CTX_PTR = 0x8016A090

REQUIRED_CTX_4C = 0x14

## Engine functions this mod hooks

func_801DD830_599740 ENTRY → your “DD830 counter / a1 classifier”

func_801E3B9C_59FAAC ENTRY + RETURN → your “E3B9C sudden stop / atk-related”

func_801D98E0_5957F0 RETURN → a per-frame place to draw overlay + run timers

func_800042AC_4EAC(entry, str, x, y) → draws on-screen text (callee used by overlay)

### 3) Global state variables (what each field really is)
## Counters / last-seen snapshots

g_dd830_calls → DD830

g_e3b9c_calls → E3B9C

g_last_weapon_slot → last read of WEAPON_SLOT_ADDR

g_last_ctx_ptr → last a0 seen in DD830 hook

g_last_ctx_4c → byte at (a0 + 0x4C) (if valid)

g_last_a1, g_prev_a1 → a1 tracking for transitions

g_last_buttons → held buttons snapshot

## Fire coin / recoil state machine

g_b_hold_frames → BHold (counts up while B held and wpn==3)

g_charge_ready → ready. (becomes 1 after FIRECOIN_HOLD_FRAMES)

g_pending → pending window after releasing B (lets you throw shortly after)

g_fire → fire. (latched to 1 only on the exact trigger moment)

g_recoil_ticks → recoil (counts down only when applied in E3B9C)

### 4) Helper utilities
### 4.1 rd8/rd16/rd32

Reads volatile memory at a fixed address (your GS-derived style reads).

### 4.2 is_rdram_ptr_u32(v)

Sanity check: must be in KSEG0 0x80000000..0x807FFFFF (8MB).
This prevents crashes when dereferencing a0 or ctl.

### 4.3 ctx_gate_ok(ctx_ptr, ctx4c)

When enabled (REQUIRE_CTX_GATE = 1), it only passes if:

ctx_ptr == 0x8016A090 AND ctx4c == 0x14

This is the code version of your “4C defaults to 14 on spawn smoke / certain actions” discriminator.

### 4.4 string helpers

append_str / append_hex / append_hex2 build each overlay line.

### 5) Hook #1: func_801DD830_599740 (ENTRY) — “DD830”
## Caller / Callee

Caller (engine): the game calls func_801DD830_599740(...)

Callee (your mod): hook_801DD830_entry() runs at function entry via RECOMP_HOOK

## What it captures

On every DD830 entry, it reads:

$a0 → treated as ctx pointer (g_last_ctx_ptr)

$a1 → your “action/state discriminator” (g_last_a1, g_prev_a1)

weapon slot → rd32(0x8015C604) (g_last_weapon_slot)

buttons held → rd16(0x800C7D3A) (g_last_buttons)

ctx+0x4C → g_last_ctx_4c (only if a0 looks valid)

## The “airborne fire coin throw” trigger

This hook sets fire and arms recoil_ticks ONLY when ALL are true:

weapon_slot == 3

transition gate: g_prev_a1 == 1 AND a1 == 6
(your note: jumping = 1, wpn3 atk = 6)

g_charge_ready == 1 (B was held long enough)

ctx_gate_ok(a0, ctx+0x4C) (your stable signature)

(g_pending != 0) OR (B still held)

If it triggers:

g_fire = 1

g_recoil_ticks = 12

clears g_charge_ready and g_pending

Otherwise:

g_fire = 0

Connection to your notes:
This is exactly your pipeline: BHold → ready → fire → recoil window.

### 6) Hook #2: func_801E3B9C_59FAAC (ENTRY/RETURN) — “E3B9C”
## Caller / Callee

Caller (engine): the game calls func_801E3B9C_59FAAC(ctx)

Callee (your mod):

hook_801E3B9C_entry() runs at entry

hook_801E3B9C_ret() runs at return

## How it finds ctl

At ENTRY:

reads a0 (ctx)

reads ctl = *(a0 + 0x5C)

reads a “speed/push scalar family” value: pre4 = *(ctl + 0x04)

## Why applying recoil at RETURN is the “right spot”

At RETURN, if g_recoil_ticks > 0:

writes *(ctl + 0x04) = 0xBF4CCCCD (float ≈ -0.8)

sets g_e3b9c_applied = 1

decrements g_recoil_ticks--

This matches your observation perfectly:

“recoil becomes nonzero after fire.=1 then counts down when E3B9C gets called”

That’s literally how the code is written: recoil only decrements inside E3B9C return hook when it successfully writes.

### 7) Hook #3: func_801D98E0_5957F0 (RETURN) — overlay + B-hold timer
## Caller / Callee

Caller (engine): game calls func_801D98E0_5957F0(...)

Callee (your mod): overlay_firecoin_recoil_final() at return

B-hold logic (creates ready + pending)

Every tick:

reads wpn, reads held buttons, b_down = (held & BTN_B)!=0

if wpn == 3:

while b_down: increments g_b_hold_frames

if g_b_hold_frames >= 45: sets g_charge_ready = 1

when B is released:

if g_charge_ready was set: starts g_pending = 120

resets g_b_hold_frames = 0

else (not wpn3): resets hold/ready/pending

So you get:

BHold = g_b_hold_frames

ready = g_charge_ready

“throw window after release” = g_pending

## What the overlay prints

Line by line (exact variables):

DD830=<g_dd830_calls> E3B9C=<g_e3b9c_calls>

wpn=<wpn> Bhold=<g_b_hold_frames> ready=<g_charge_ready>

pending=<g_pending> FIRE=<g_fire> recoil=<g_recoil_ticks>

a1_prev=<g_prev_a1> a1=<g_last_a1>

ctx=<g_last_ctx_ptr> 4C=<g_last_ctx_4c>

ctl=<g_e3b9c_ctl> pre4=<g_e3b9c_pre4> B=<b_down>

post4=<g_e3b9c_post4> ap=<g_e3b9c_applied>

(Note) g_l8 is drawn but never filled in this file (so it’ll be blank).

### 8) Call graph (callers / callees)
Engine → your hooks

Engine calls func_801DD830_599740
↳ ENTRY hook: hook_801DD830_entry

Engine calls func_801E3B9C_59FAAC
↳ ENTRY hook: hook_801E3B9C_entry
↳ RETURN hook: hook_801E3B9C_ret

Engine calls func_801D98E0_5957F0
↳ RETURN hook: overlay_firecoin_recoil_final

## Your code internal callees

overlay_firecoin_recoil_final → calls func_800042AC_4EAC (text draw)

Both hooks use:

rd16/rd32/rd8

is_rdram_ptr_u32

ctx_gate_ok

string helpers (append_*)

### 9) Your observed fields mapped 1:1 to this code

DD830 = g_dd830_calls (increments every time DD830 entry hook runs)

E3B9C = g_e3b9c_calls

wpn. = rd32(0x8015C604)

a1 = $a1 captured in DD830 entry hook → g_last_a1 / g_prev_a1

BHold = g_b_hold_frames (only while wpn==3 and B held)

B = b_down computed from held bit BTN_B

4C = rd8(a0 + 0x4C) captured from DD830’s a0 ctx

ready. = g_charge_ready

fire. = g_fire (only 1 on the exact trigger moment)

recoil = g_recoil_ticks (armed in DD830, decremented in E3B9C when applied)

### 10) Discoveries / connections from your notes

Your “a1 meaning” is being used as an actual state-transition key.
This file doesn’t treat a1 as arbitrary—it specifically watches the transition 1 → 6. That strongly supports your model that:

a1=1 correlates to a jump/airborne-related state

a1=6 correlates to weapon-3 attack/throw state for Goemon/Sasuke (as you observed)

ctx+0x4C = “mode discriminator” is consistent with your “defaults to 0x14 on spawn smoke.”
The code assumes your stable throw context is (ctx == 0x8016A090 && ctx+0x4C == 0x14).
That matches your observation: 4C defaults to 14 when a character spawns behind smoke, and weapon2/3 tends to bring it back to 0x14.

Why recoil counts down exactly when you saw it count down:
Because it only decrements inside E3B9C return hook, and only if it successfully wrote the recoil value.

Potential gotcha if teleports / character swaps ever change the ctx signature:
This file hard-gates to one ctx pointer + one 4C value. If debug teleports ever put you into a different-but-equivalent context pointer, the detector will silently stop arming recoil.
