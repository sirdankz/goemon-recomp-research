# Callgraph

Generated from `2025-12-29` evidence pack (`callgraph_edges.jsonl`).

- Direct jal edges: **6281**
- Functions with any jalr present (marker edges): **336**

## Top fan-in (most called)

| Function entry | Count | Name (if known) |
| --- | --- | --- |
| `0x80013AC4` | 376 | `HUB_fanin_376__80013AC4` |
| `0x801DACDC` | 242 | `HUB_fanin_242__801DACDC` |
| `0x8003521C` | 133 | `sched_setter_8003521C` |
| `0x80038BC8` | 88 | `HUB_fanin_88__80038BC8` |
| `0x80003E10` | 72 | `HUB_fanin_72__80003E10` |
| `0x80003EA0` | 71 | `FUN_80003ea0` |
| `0x80035EEC` | 55 | `FUN_80035eec` |
| `0x80033898` | 54 | `FUN_80033898` |
| `0x80003F30` | 51 | `FUN_80003f30` |
| `0x80034E08` | 49 | `FUN_80034e08` |
| `0x800040A8` | 48 | `FUN_800040a8` |
| `0x80030688` | 48 | `FUN_80030688` |
| `0x80014CC4` | 45 | `FUN_80014cc4` |
| `0x801DC554` | 44 | `FUN_801dc554` |
| `0x800141C4` | 42 | `FUN_800141c4` |
| `0x800417E0` | 42 | `_nsqrtf` |
| `0x8001D394` | 42 | `FUN_8001d394` |
| `0x80040110` | 41 | `osRecvMesg` |
| `0x802192B4` | 41 | `FUN_802192b4` |
| `0x802171A8` | 35 | `FUN_802171a8` |
| `0x80003728` | 33 | `FUN_80003728` |
| `0x80034ED4` | 33 | `FUN_80034ed4` |
| `0x801E7DE8` | 33 | `FUN_801e7de8` |
| `0x80047710` | 32 | `osVirtualToPhysical` |
| `0x801E8C44` | 32 | `FUN_801e8c44` |
| `0x80040620` | 31 | `osWritebackDCache` |
| `0x801E8964` | 30 | `FUN_801e8964` |
| `0x80003A94` | 29 | `FUN_80003a94` |
| `0x80049540` | 29 | `__osRestoreInt` |
| `0x8001DB04` | 26 | `FUN_8001db04` |
| `0x80049520` | 26 | `__osDisableInt` |
| `0x8004CF80` | 26 | `__osContRamRead` |
| `0x801DD720` | 26 | `FUN_801dd720` |
| `0x801EE4AC` | 26 | `FUN_801ee4ac` |
| `0x8021A26C` | 26 | `FUN_8021a26c` |
| `0x8021664C` | 26 | `FUN_8021664c` |
| `0x8002AA00` | 25 | `FUN_8002aa00` |
| `0x80014B74` | 24 | `FUN_80014b74` |
| `0x800322A8` | 24 | `FUN_800322a8` |
| `0x80219E08` | 24 | `FUN_80219e08` |
| `0x80013B14` | 23 | `FUN_80013b14` |
| `0x800148F0` | 23 | `FUN_800148f0` |
| `0x800476B0` | 23 | `alHeapDBAlloc` |
| `0x8004CF0C` | 23 | `__osPfsSelectBank` |
| `0x801E3324` | 23 | `FUN_801e3324` |
| `0x801DAC7C` | 23 | `FUN_801dac7c` |
| `0x8001D898` | 22 | `FUN_8001d898` |
| `0x80034EF8` | 21 | `FUN_80034ef8` |
| `0x800042AC` | 21 | `FUN_800042ac` |
| `0x80029E08` | 21 | `FUN_80029e08` |

## Top fan-out (calls many)

| Function entry | Count | Name (if known) |
| --- | --- | --- |
| `0x802268A8` | 85 | `HUB_fanout_85__802268A8` |
| `0x801E4624` | 75 | `HUB_fanout_75__801E4624` |
| `0x801D9E9C` | 72 | `HUB_fanout_72__801D9E9C` |
| `0x8002EB10` | 48 | `HUB_fanout_48__8002EB10` |
| `0x80030730` | 48 | `HUB_fanout_48__80030730` |
| `0x802176C4` | 46 | `FUN_802176c4` |
| `0x801E8964` | 42 | `FUN_801e8964` |
| `0x801CD310` | 33 | `FUN_801cd310` |
| `0x801E3400` | 33 | `FUN_801e3400` |
| `0x8001FA80` | 32 | `FUN_8001fa80` |
| `0x8021F6DC` | 31 | `FUN_8021f6dc` |
| `0x80000580` | 25 | `FUN_80000580` |
| `0x801F728C` | 24 | `FUN_801f728c` |
| `0x8003D68C` | 22 | `FUN_8003d68c` |
| `0x801E75D4` | 22 | `FUN_801e75d4` |
| `0x80000AD8` | 21 | `FUN_80000ad8` |
| `0x8003DF98` | 21 | `FUN_8003df98` |
| `0x80047C30` | 21 | `alSynNew` |
| `0x80049B90` | 21 | `__osDevMgrMain` |
| `0x801F77F4` | 21 | `FUN_801f77f4` |
| `0x80005394` | 20 | `FUN_80005394` |
| `0x80006568` | 20 | `JRDISP_container_80006568` |
| `0x801EFBDC` | 19 | `FUN_801efbdc` |
| `0x800154B0` | 18 | `FUN_800154b0` |
| `0x80016C44` | 18 | `FUN_80016c44` |
| `0x80002138` | 17 | `JRDISP_container_80002138` |
| `0x800176F0` | 17 | `FUN_800176f0` |
| `0x80215CA4` | 17 | `FUN_80215ca4` |
| `0x80050960` | 16 | `__osLeoInterrupt` |
| `0x801D72D4` | 16 | `FUN_801d72d4` |
| `0x801E708C` | 16 | `A6_dispatch_selector_FUN_801E708C` |
| `0x801EEDF8` | 16 | `FUN_801eedf8` |
| `0x801E1928` | 15 | `FUN_801e1928` |
| `0x801F7C14` | 15 | `DoorEnter_chain__FUN_801F7C14` |
| `0x801F7F78` | 15 | `TransitionState_set4__FUN_801F7F78` |
| `0x80218548` | 15 | `FUN_80218548` |
| `0x8021EE50` | 15 | `FUN_8021ee50` |
| `0x80019ED8` | 14 | `FUN_80019ed8` |
| `0x80020440` | 14 | `FUN_80020440` |
| `0x8003F720` | 14 | `osInitialize` |
| `0x801E8F50` | 14 | `FUN_801e8f50` |
| `0x801E9AE0` | 14 | `FUN_801e9ae0` |
| `0x801EBF48` | 14 | `FUN_801ebf48` |
| `0x801F4754` | 14 | `FUN_801f4754` |
| `0x80218E7C` | 14 | `FUN_80218e7c` |
| `0x80000D00` | 13 | `FUN_80000d00` |
| `0x80024160` | 13 | `FUN_80024160` |
| `0x80037DC0` | 13 | `FUN_80037dc0` |
| `0x8004D2E0` | 13 | `osPfsChecker` |
| `0x801CB9C8` | 13 | `FUN_801cb9c8` |

