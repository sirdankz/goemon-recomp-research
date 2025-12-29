# Tooling Gotchas

## Ghidra 12 Jython quirks
- Use `toAddr()` for listing.getInstructionAt/After, not raw ints
- `InstructionDB` has no `getOffset()`; use `inst.getAddress().getOffset()`
- `MemoryBlockDB` has no `getBody()`
- Jython `bytearray` has no `.hex()`

## Dispatch detection
- Scan for `jr <any reg>` (not just `jr t9`) — tail-call jump tables are common.
- Resolve gp-relative / indirect tables; don’t trust xrefs only.

## Segptr handling
- Treat `0x08xxxxxx` as possible mid-function targets; they can be scheduler/coroutine labels.
