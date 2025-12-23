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
