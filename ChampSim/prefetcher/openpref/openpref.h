#ifndef PREFETCHER_OPEN_H
#define PREFETCHER_OPEN_H

#include <cstdint>

#include "champsim.h"
#include "modules.h"
#include <map>
#include <unordered_set>

class openpref : public champsim::modules::prefetcher
{
private:
	void prefetch(uint64_t addr){ prefetch_line(addr, true, 0); }
	void access(uint64_t ip, uint64_t addr, bool hit);
	//don't make students deal with MSHR capacity retries
	std::map<uint64_t, std::unordered_set<uint64_t>> in_flight;
public:
  using prefetcher::prefetcher;

  void prefetcher_initialize();
  // void prefetcher_branch_operate(champsim::address ip, uint8_t branch_type, champsim::address branch_target) {}
  uint32_t prefetcher_cache_operate(champsim::address addr, champsim::address ip, uint8_t cache_hit, bool useful_prefetch, access_type type,
                                    uint32_t metadata_in)
  {
	uint64_t a = addr.to<uint64_t>();
	uint64_t pc = ip.to<uint64_t>();
	if (in_flight[a].count(pc)) return metadata_in;
	in_flight[a].insert(pc);
	access(pc, a, cache_hit);
	return metadata_in;
  }
  uint32_t prefetcher_cache_fill(champsim::address addr, long set, long way, uint8_t prefetch, champsim::address evicted_addr, uint32_t metadata_in)
  {
	  uint64_t block = (addr.to<uint64_t>() >> LOG2_BLOCK_SIZE) << LOG2_BLOCK_SIZE;
	  uint64_t next = block + BLOCK_SIZE;
	  for (auto it = in_flight.lower_bound(block);  
			  it != in_flight.end() && it->first < next; 
			  it = in_flight.erase(it));
	  return metadata_in;
  }
  // void prefetcher_cycle_operate() {}
  // void prefetcher_final_stats() {}
};

#endif
