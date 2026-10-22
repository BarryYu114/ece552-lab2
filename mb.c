/* Build: gcc -O0 -fno-pie -no-pie mb.c -o mb
 * Inspect: objdump -d -Mintel mb | sed -n '/<main>:/,/^$/p'
 * See the validation notes below for branch outcomes and predictor checks.
 */
int main(void)
{
  volatile unsigned checksum = 0;
  unsigned i;
  for (i = 0; i < 100000; ++i) {
    if ((i & 3u) == 0)
      checksum += 3;
    else
      checksum += 1;
  }
  return checksum != 150000;
}

/*
Validation notes (C program above is 12 lines, excluding its header comment).

Compile with the exact flags above. -O0 retains the conditional branch, and
volatile keeps the checksum updates observable. The checksum is
25000*3 + 75000*1 = 150000; a successful native run returns exit status 0.

Actual WSL GCC disassembly from this build (recheck on the course machine):
  401121: and    eax,0x3
  401124: test   eax,eax
  401126: jne    401133       # B1: skip the +3 path when i%4 != 0
  ...
  40113c: add    DWORD PTR [rbp-0x4],0x1
  401140: cmp    DWORD PTR [rbp-0x4],0x1869f
  401147: jbe    40111e       # B2: continue while i <= 99999
  ...
  401151: setne  al           # Return-value check is not a branch

B1 executes 100000 times with outcomes N,T,T,T repeated 25000 times.
B2 executes 100001 times: 100000 taken outcomes, then one not-taken exit.
For the observed PCs, B1 uses BHT[0x112] and PHT[6]; B2 uses BHT[0x114]
and PHT[7]. They do not alias each other. Recompute these indices from the
actual course binary: BHT=(PC>>4)&511, PHT=PC&15. Check the unconditional
jumps too: update() is invoked for all branch types by the supplied wrapper.

For B1 alone, starting with history=0 and weak-not-taken counters, the
direction misses occur at zero-based iterations 1,2,3,5,6,7,9 (seven misses).
Its steady-state six-bit histories are 011101,111011,110111,101110.
Each identifies the next outcome uniquely; after training, B1 should stop
missing. Train the counter selected by OLD history, then append the actual
outcome with history=((history<<1)|taken)&63.

For B2 alone, seven initial taken outcomes train histories 0,1,3,7,15,31,63;
the final not-taken exit adds one miss (eight direction misses total).
These cold-start counts assume no other branches interfere with the tables.
They are NOT an assertion that the entire program reports 15 mispredictions:
startup, library code, unconditional branches, target prediction, and aliasing
can affect ChampSim totals. Whole-program MPKI uses retired instructions,
not the number of branches or C loop iterations.

Course-machine workflow, starting in the lab root:
  gcc -O0 -fno-pie -no-pie mb.c -o mb
  objdump -d -Mintel mb | sed -n '/<main>:/,/^$/p'
  ./mb; echo $?                 # Expect 0
  cd ChampSim/tracer/pin
  make
  /cad2/ece552f/pin3.22 -t obj-intel64/champsim_tracer.so \
    -o mb.champsim -s 0 -t 5000000 -- ../../../mb
  cd ../..
  ./config.sh ece552/twolvl.json
  make
  bin/twolvl --warmup-instructions 0 tracer/pin/mb.champsim

The trace cap exceeds this loop's expected instruction count. Verify the
tracer reaches program completion. Zero warmup preserves cold-start behavior.
Use disassembly and, when available, a debugger to inspect the B1/B2 counters
and histories without changing submitted simulator files. Aggregate MPKI alone
is a smoke check, not proof of every indexing or update rule.
*/
