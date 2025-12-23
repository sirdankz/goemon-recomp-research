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
