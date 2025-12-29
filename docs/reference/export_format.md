# Export / Dump Format

This repository uses two layers:

1. **Curated “UPDATED” notes** (human-readable verified systems)
2. **Machine-dump reference** (this folder), regenerated from evidence packs

## Evidence pack location
`docs/reference_dumps/2025-12-29_full_evidence_pack/`

## Files
- `index.json` — program metadata + exported functions list
- `memory_blocks.json` — memory map blocks
- `symbols.csv` / `symbols.jsonl` — symbol table snapshot
- `callgraph_edges.jsonl` — direct `jal` edges (+ marker that `jalr` exists)
- `functions/*.json` — per-function instruction dump + direct calls + refs-to-entry

## Re-generate
1) Run the exporter script in Ghidra
2) Commit the new dump under a dated folder
3) Re-run the doc generator (optional) to update the pages in `docs/reference/`

!!! tip
    Keep dumps dated so Git diffs reveal what changed after each mapping sprint.
