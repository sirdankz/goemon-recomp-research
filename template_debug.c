
#include <stdint.h>
#include "modding.h"

// On-screen text draw
void func_800042AC_4EAC(int16_t entry, const char* string, int16_t x, int16_t y);

#define WEAPON_SLOT_ADDR   0x8015C604u
#define BUTTONS_HELD_ADDR  0x800C7D3Au

// N64 controller bits
#define BTN_B 0x4000u

// Gate to the context you observed for weapon-3 throw handling (optional but recommended)
#define REQUIRE_CTX_GATE   1
#define REQUIRED_CTX_PTR   0x8016A090u
#define REQUIRED_CTX_4C    0x14u

static inline uint32_t rd32(uint32_t a){ return *(volatile uint32_t*)a; }
static inline uint16_t rd16(uint32_t a){ return *(volatile uint16_t*)a; }
static inline uint8_t  rd8 (uint32_t a){ return *(volatile uint8_t*)a;  }

// KSEG0 sanity: 0x80000000..0x807FFFFF (8MB RDRAM)
static inline int is_rdram_ptr_u32(uint32_t v) {
    if ((v & 0xFF000000u) != 0x80000000u) return 0;
    if ((v & 0x00FFFFFFu) >= 0x00800000u) return 0;
    return 1;
}

/* =========================
   tiny string helpers
   ========================= */
static void u32_to_hex(char* out, uint32_t v) {
    static const char hex[] = "0123456789ABCDEF";
    out[0]='0'; out[1]='x';
    for (int i=0;i<8;i++){ out[9-i]=hex[v & 0xF]; v >>= 4; }
    out[10]=0;
}
static char* append_str(char* p, const char* s){ while(*s) *p++=*s++; *p=0; return p; }
static char* append_hex(char* p, uint32_t v){ char h[11]; u32_to_hex(h,v); return append_str(p,h); }
static char* append_hex2(char* p, uint32_t v){
    static const char hex[]="0123456789ABCDEF";
    *p++=hex[(v>>4)&0xF]; *p++=hex[v&0xF]; *p=0; return p;
}

static inline int ctx_gate_ok(uint32_t ctx_ptr, uint8_t ctx4c) {
#if REQUIRE_CTX_GATE
    return (ctx_ptr == REQUIRED_CTX_PTR) && (ctx4c == REQUIRED_CTX_4C);
#else
    (void)ctx_ptr; (void)ctx4c;
    return 1;
#endif
}

/* =========================
   Fire coin detect (B-hold) — tuned for your observed ~15fps overlay tick
   ========================= */
static volatile uint32_t g_frame = 0;
static volatile uint16_t g_b_hold_frames = 0;
static volatile uint8_t  g_charge_ready = 0;
static volatile uint16_t g_pending = 0;

// tuned (works for you)
#define FIRECOIN_HOLD_FRAMES    45
#define FIRECOIN_WINDOW_FRAMES  120

// recoil window (counts down when applied in E3B9C)
#define RECOIL_TICKS            12

static volatile uint8_t g_fire = 0;          // latched for overlay (1 when we trigger)
static volatile uint8_t g_recoil_ticks = 0;  // counts down when we successfully apply in E3B9C

/* =========================
   DD830 capture (your discriminators)
   ========================= */
static volatile uint32_t g_dd830_calls = 0;
static volatile uint32_t g_last_weapon_slot = 0;
static volatile uint32_t g_last_ctx_ptr = 0;
static volatile uint8_t  g_last_ctx_4c = 0xFF;
static volatile uint32_t g_last_a1 = 0;
static volatile uint32_t g_prev_a1 = 0;
static volatile uint16_t g_last_buttons = 0;

RECOMP_HOOK("func_801DD830_599740")
void hook_801DD830_entry(void) {
    uint32_t a0, a1;
    __asm__ volatile ("move %0, $a0" : "=r"(a0));
    __asm__ volatile ("move %0, $a1" : "=r"(a1));

    g_dd830_calls++;

    g_last_weapon_slot = rd32(WEAPON_SLOT_ADDR);
    g_last_ctx_ptr = a0;
    g_last_buttons = rd16(BUTTONS_HELD_ADDR);

    g_last_ctx_4c = 0xFF;
    if (is_rdram_ptr_u32(a0)) g_last_ctx_4c = rd8(a0 + 0x4C);

    // a1 transition tracking
    g_prev_a1 = g_last_a1;
    g_last_a1 = a1;

    // Trigger recoil ONLY on:
    // - weapon slot 3
    // - a1 transition 1 -> 6 (your desired airborne throw gate)
    // - charge_ready (B held long enough)
    // - within pending window OR B still held
    // - ctx gate matches your observed ctx & 4C
    if (g_last_weapon_slot == 3 &&
        a1 == 6 &&
        g_prev_a1 == 1 &&
        g_charge_ready &&
        ctx_gate_ok(g_last_ctx_ptr, g_last_ctx_4c) &&
        (g_pending || (g_last_buttons & BTN_B)))
    {
        g_fire = 1;
        g_recoil_ticks = RECOIL_TICKS;
        g_charge_ready = 0;
        g_pending = 0;
    } else {
        g_fire = 0;
    }
}

/* =========================
   Where pushback SHOULD be applied:
   Use the SAME function that your pipe pushback mod proved controls the stop/push float.
   func_801E3B9C_59FAAC: a0 + 0x5C -> ctl, and ctl + 0x04 is the "F4 family" speed/push scalar.
   If we force ctl+0x04 negative HERE (in this function's return), it lands at the correct spot
   in the frame order (right where the engine normally stomps it).
   ========================= */

static volatile uint32_t g_e3b9c_calls = 0;
static volatile uint32_t g_e3b9c_ctx = 0;
static volatile uint32_t g_e3b9c_ctl = 0;
static volatile uint32_t g_e3b9c_pre4 = 0;
static volatile uint32_t g_e3b9c_post4 = 0;
static volatile uint8_t  g_e3b9c_applied = 0;

RECOMP_HOOK("func_801E3B9C_59FAAC")
void hook_801E3B9C_entry(void) {
    uint32_t a0;
    __asm__ volatile ("move %0, $a0" : "=r"(a0));
    g_e3b9c_calls++;
    g_e3b9c_ctx = a0;
    g_e3b9c_ctl = 0;
    g_e3b9c_pre4 = 0;
    g_e3b9c_post4 = 0;
    g_e3b9c_applied = 0;

    if (!is_rdram_ptr_u32(a0)) return;
    uint32_t ctl = *(volatile uint32_t*)(a0 + 0x5C);
    g_e3b9c_ctl = ctl;
    if (!is_rdram_ptr_u32(ctl)) return;

    g_e3b9c_pre4 = *(volatile uint32_t*)(ctl + 0x04);
}

RECOMP_HOOK_RETURN("func_801E3B9C_59FAAC")
void hook_801E3B9C_ret(void) {
    uint32_t ctl = g_e3b9c_ctl;
    if (is_rdram_ptr_u32(ctl)) {
        g_e3b9c_post4 = *(volatile uint32_t*)(ctl + 0x04);

        // If recoil is armed, force a strong negative scalar here.
        // Start conservative; you can tune later.
        if (g_recoil_ticks) {
            const uint32_t RECOIL_F4_BITS = 0xBF4CCCCDu; // -0.8f (safer than -1.0)
            *(volatile uint32_t*)(ctl + 0x04) = RECOIL_F4_BITS;

            g_e3b9c_post4 = *(volatile uint32_t*)(ctl + 0x04);
            g_e3b9c_applied = 1;

            // Count down ONLY when we actually applied inside E3B9C
            g_recoil_ticks--;
        }
    }
}

/* =========================
   Per-frame overlay + B-hold charge timing
   ========================= */
#define OVERLAY_ENTRY_BASE 1
static char g_l1[150], g_l2[150], g_l3[150], g_l4[150], g_l5[150], g_l6[150], g_l7[150], g_l8[150];

RECOMP_HOOK_RETURN("func_801D98E0_5957F0")
void overlay_firecoin_recoil_final(void) {
    g_frame++;

    const uint32_t wpn = rd32(WEAPON_SLOT_ADDR);
    const uint16_t held = rd16(BUTTONS_HELD_ADDR);
    const uint8_t b_down = (held & BTN_B) ? 1 : 0;

    if (g_pending) g_pending--;

    // Only track while weapon 3 equipped
    if (wpn == 3) {
        if (b_down) {
            if (g_b_hold_frames < 60000) g_b_hold_frames++;
            if (g_b_hold_frames >= FIRECOIN_HOLD_FRAMES) g_charge_ready = 1;
        } else {
            if (g_charge_ready) g_pending = FIRECOIN_WINDOW_FRAMES;
            g_b_hold_frames = 0;
        }
    } else {
        g_b_hold_frames = 0;
        g_charge_ready = 0;
        g_pending = 0;
    }

    char *p;

    p = g_l1; p = append_str(p, "DD830="); p = append_hex(p, g_dd830_calls);
    p = append_str(p, "  E3B9C="); p = append_hex(p, g_e3b9c_calls);

    p = g_l2; p = append_str(p, "wpn="); p = append_hex(p, wpn);
    p = append_str(p, " Bhold="); p = append_hex(p, g_b_hold_frames);
    p = append_str(p, " ready="); p = append_hex2(p, g_charge_ready);

    p = g_l3; p = append_str(p, "pending="); p = append_hex(p, g_pending);
    p = append_str(p, " FIRE="); p = append_hex2(p, g_fire);
    p = append_str(p, " recoil="); p = append_hex2(p, g_recoil_ticks);

    p = g_l4; p = append_str(p, "a1_prev="); p = append_hex(p, g_prev_a1);
    p = append_str(p, " a1="); p = append_hex(p, g_last_a1);


    p = g_l5; p = append_str(p, "ctx="); p = append_hex(p, g_last_ctx_ptr);
    p = append_str(p, " 4C="); p = append_hex2(p, g_last_ctx_4c);

    p = g_l6; p = append_str(p, "ctl="); p = append_hex(p, g_e3b9c_ctl);
    p = append_str(p, " pre4="); p = append_hex(p, g_e3b9c_pre4);
    p = append_str(p, " B="); p = append_hex2(p, b_down);


    p = g_l7; p = append_str(p, " post4="); p = append_hex(p, g_e3b9c_post4);
    p = append_str(p, " ap="); p = append_hex2(p, g_e3b9c_applied);
    


    func_800042AC_4EAC(OVERLAY_ENTRY_BASE + 0, g_l1, 1,  16);
    func_800042AC_4EAC(OVERLAY_ENTRY_BASE + 1, g_l2, 1,  28);
    func_800042AC_4EAC(OVERLAY_ENTRY_BASE + 2, g_l3, 1,  40);
    func_800042AC_4EAC(OVERLAY_ENTRY_BASE + 3, g_l4, 1,  52);
    func_800042AC_4EAC(OVERLAY_ENTRY_BASE + 4, g_l5, 1,  64);
    func_800042AC_4EAC(OVERLAY_ENTRY_BASE + 5, g_l6, 1,  76);
    func_800042AC_4EAC(OVERLAY_ENTRY_BASE + 6, g_l7, 1,  86);
    func_800042AC_4EAC(OVERLAY_ENTRY_BASE + 7, g_l8, 1,  98);
}
