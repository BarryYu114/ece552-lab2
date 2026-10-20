#include "twobitsat.h"

#include <array>

namespace
{
// Direct-mapped bimodal table: 4096 counters * 2 bits = 8192 bits.
// Four counters share each byte; no tags or history registers are needed.
constexpr uint64_t index_mask = 4096 - 1;
std::array<uint8_t, 1024> counters;
}

void twobitsat::initialize_branch_predictor()
{
  // Each two-bit field is 01 (weak not-taken).
  counters.fill(0x55);
}

bool twobitsat::predict(uint64_t ip)
{
  // Keep the low PC bits: x86 instructions are not fixed-width aligned.
  const auto index = ip & index_mask;
  const auto shift = 2 * (index & 3);
  const auto counter = (counters[index >> 2] >> shift) & 3;
  return counter >= 2;
}

void twobitsat::update(uint64_t ip, bool resolveDir)
{
  const auto index = ip & index_mask;
  const auto shift = 2 * (index & 3);
  auto& packed = counters[index >> 2];
  auto counter = (packed >> shift) & 3;

  if (resolveDir && counter < 3)
    ++counter;
  else if (!resolveDir && counter > 0)
    --counter;

  // Replace this counter without changing the other three in the byte.
  packed = static_cast<uint8_t>((packed & ~(3u << shift)) | (static_cast<unsigned>(counter) << shift));
}


