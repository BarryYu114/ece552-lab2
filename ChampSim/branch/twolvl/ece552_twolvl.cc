#include "twolvl.h"

#include <array>

namespace
{
// Logical hardware: 512 * 6 history bits + 16 * 64 * 2 counter bits
// = 5120 bits (640 bytes). Byte-sized software entries simplify access.
std::array<uint8_t, 512> local_history;
std::array<std::array<uint8_t, 64>, 16> pattern_tables;

constexpr uint64_t pht_mask = 0x0f;
constexpr uint64_t bht_mask = 0x1ff;
constexpr unsigned history_mask = 0x3f;
}

void twolvl::initialize_branch_predictor()
{
  local_history.fill(0);
  for (auto& table : pattern_tables)
    table.fill(1); // 01: weak not-taken.
}

bool twolvl::predict(uint64_t ip)
{
  // Handout layout: [unused PC bits | 9-bit BHT index | 4-bit PHT index].
  const auto bht_index = (ip >> 4) & bht_mask;
  const auto pht_index = ip & pht_mask;
  const auto history = local_history[bht_index];

  return pattern_tables[pht_index][history] >= 2;
}

void twolvl::update(uint64_t ip, bool resolveDir)
{
  const auto bht_index = (ip >> 4) & bht_mask;
  const auto pht_index = ip & pht_mask;
  auto& history = local_history[bht_index];

  // Train using the OLD history that selected the prediction counter.
  auto& counter = pattern_tables[pht_index][history];
  if (resolveDir && counter < 3)
    ++counter;
  else if (!resolveDir && counter > 0)
    --counter;

  // Append the actual outcome, retaining only the latest six outcomes.
  history = static_cast<uint8_t>(((static_cast<unsigned>(history) << 1) | static_cast<unsigned>(resolveDir)) & history_mask);
}


