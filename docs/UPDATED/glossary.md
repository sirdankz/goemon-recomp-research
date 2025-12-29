# Glossary
- **ctx**: per-actor context pointer used by action/state systems
- **Action6**: handler system selected by `ctx+0x60` and dispatched via tables
- **jr-dispatch**: jump-table dispatch using `jr <reg>` to tail-jump into handler code
- **coroutine label**: mid-function entry point used as a scheduled tick target
- **segptr**: segmented pointer style; in this project `0x08xxxxxx` can map to `0x800xxxxx`
