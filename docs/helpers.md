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
