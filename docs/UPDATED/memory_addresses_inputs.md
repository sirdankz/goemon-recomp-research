# Memory Addresses and Input Mapping

This page collects the **confirmed memory addresses and key runtime fields** we’ve been using for modding `mnsg.us.0.z64`, especially for input-driven mods (super jump, toggles, weapon behavior, debugging).

---

## What to remember

- Button “held” state is readable directly from RAM (u16 bitfield).
- Weapon slot selection is a stable RAM field (observed values 1/2/3).
- Several useful ctx/ctl offsets behave like “live camera/movement” indicators.

---

## Inputs (confirmed)

### Button held bitfield
- **Address:** `0x800C7D3A` (u16)
- **Meaning:** current buttons held (bitmask)

Known bits:
- `BTN_A = 0x8000`
- `BTN_B = 0x4000`

Example helper:

```c
static inline uint16_t read_buttons_held(void) {
    return *(volatile uint16_t*)0x800C7D3A;
}
