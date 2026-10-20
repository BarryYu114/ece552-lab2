#ifndef BRANCH_NT_H
#define BRANCH_NT_H

#include "modules.h"

class nt : champsim::modules::branch_predictor
{

public:
  using branch_predictor::branch_predictor;

  bool predict_branch(champsim::address ip);
};

#endif
