# Tooling Gotchas (Ghidra Jython + workflows)

This page lists the “pain points” we hit repeatedly so you don’t burn time re-fixing the same issues.

---

## Ghidra 12 Jython gotchas (verified)

### Address handling
- `listing.getInstructionAt(...)` and `getInstructionAfter(...)` require an **Address**
  - Use `toAddr(0x80123456)` or `currentProgram.getAddressFactory()...`

### Instruction offset
- `InstructionDB` has no `getOffset()`
  - Use `inst.getAddress().getOffset()`

### MemoryBlockDB
- `MemoryBlockDB` has no `getBody()`
  - Use available APIs for ranges/iterators instead

### Jython bytearray
- Jython `bytearray` has no `.hex()`
  - use manual formatting

---

## Script reliability rules

- Avoid API methods with signature drift across Ghidra versions (ex: comment iterators).
- Prefer “iterator-free” exports:
  - iterate defined data
  - iterate instructions
  - query comment text per-address

---

## Reverse engineering heuristics

- `jr` dispatch will not show up as a nice callgraph edge.
- Treat segptr-like `0x08xxxxxx` as real targets, especially in scheduler/coroutines.
