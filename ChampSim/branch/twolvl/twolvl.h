#ifndef BRANCH_TWOLVL_H
#define BRANCH_TWOLVL_H

#include "modules.h"

class twolvl : champsim::modules::branch_predictor
{

  bool predict(uint64_t ip);
  void update(uint64_t ip, bool resolveDir);
public:
  using branch_predictor::branch_predictor;

  void initialize_branch_predictor();
  bool predict_branch(champsim::address ip) { return predict(ip.to<uint64_t>()); }
  void last_branch_result(champsim::address ip, champsim::address branch_target, bool taken, uint8_t branch_type) { update(ip.to<uint64_t>(), taken); }
};

#endif
