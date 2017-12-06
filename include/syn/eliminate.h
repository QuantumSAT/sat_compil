/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _ELIMINATE_H_
#define _ELIMINATE_H_

#include "optimize.h"

namespace SYN {

  class Eliminate : public Opt {
  public:
    Eliminate(Model* top) : Opt(top) {}
    ~Eliminate() {}
    
    void eliminate(int thre, int limit);

  private:
    int nodeValue(Gate*, int limit, std::vector<Gate*>&);
  };

}
#endif //_ELIMINATE_H_
