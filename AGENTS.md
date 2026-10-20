# Repository Guidelines

## Lab Scope & Project Structure

This is the ECE552 Fall 2026 Dynamic Branch Prediction lab. Treat `Lab2.pdf` as the authority for implementation, experiments, and submission requirements.

- `ChampSim/branch/{twobitsat,twolvl,openbranch}/ece552_*.cc`: predictor implementations, currently skeletons.
- `ChampSim/ece552/`: supplied JSON configurations selecting each predictor.
- `ChampSim/src/` and `inc/`: simulator infrastructure and interfaces; consult these to understand behavior.
- `ChampSim/tracer/pin/`: microbenchmark tracing tool.
- `CACTI/`: executable and configurations for hardware estimates.

## Implementation Boundaries & Style

Only modify the three `ece552_*.cc` files for simulator implementation. The automarker ignores other implementation files. Preserve headers, function signatures, simulator statistics, and supplied build/configuration files. Separate required deliverables (`mb.c`, report, CACTI configurations) are allowed.

Use C++17 and match neighboring indentation and naming. Implement `initialize_branch_predictor`, `predict`, and `update`; initialize all state explicitly. Preserve low PC bits: x86 instructions have variable lengths. No formatter configuration is supplied.

The bimodal predictor has 8192 bits of counters. The two-level PAp predictor uses 512 six-bit local histories and sixteen PHTs of two-bit counters; follow the handout's PC-index diagram. Initialize counters to weak not-taken. The open-ended predictor has a 128-Kbit hardware-storage limit; document logical storage rather than software allocation size. Do not copy internet implementation code.

## Build & Run Commands

Use the course Linux environment; the Makefile references `/cad2/ece552f/champsim-deps/`. From `ChampSim/`:

```bash
./config.sh ece552/twolvl.json
make -j4
bin/twolvl --warmup-instructions 5000000 --simulation-instructions 20000000 /cad2/ece552f/Champsim_benchmarks/llama2.champsim.gz
```

Substitute `twobitsat` or `openbranch` for other predictors. From `CACTI/`, run `./cacti -infile pureRAM.cfg`; change only options authorized by the handout.

## Validation & Deliverables

Evaluate mcf, roms, charlie, merced, k-clique, sssp, llama2, and whisper. Record misprediction counts and MPKI with identical instruction counts. Open-ended geometric-mean MPKI must be below 3.35.

Create a 10–20-line `mb.c` validating two-level statistics; include explanatory comments, generated assembly snippets, and compilation flags. Compile with `gcc mb.c -o mb`, then trace through the supplied Pintool. The upstream test directory is absent.

Submit predictor sources, `mb.c`, a two-page report, and predictor CACTI configurations. Report area, access latency, leakage power, modified parameters, storage accounting, and partner contributions.

## Commits & Review

No Git history is available. Use focused imperative subjects. Describe affected predictors and validation results in reviews. Keep traces and generated binaries out of source changes; keep course code private. Only one partner should submit.
