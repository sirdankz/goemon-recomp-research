# Evidence Pack Format

This is the structure we export from Ghidra so analysis can happen “offline” and then be applied back in one shot.

```
full_evidence_pack/
  index.json
  memory_blocks.json
  symbols.csv
  symbols.jsonl
  callgraph_edges.jsonl
  functions/
    0xXXXXXXXX_FUN_xxxxxxxx.json
```

## Key idea
The pack is designed for: **auto-mapping, table inference, dispatch detection, and renaming** without hand-clicking Ghidra for each question.

## Status
- Latest confirmed run produced a valid `full_evidence_pack.zip` and enabled automatic JR table extraction.
- Exporter should be Jython-safe (avoid multi-line conditional expressions inside dict literals).
