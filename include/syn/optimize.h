/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

/* head file for logic optimization */

#ifndef _SYN_OPT_H_
#define _SYN_OPT_H_

#include <stdarg.h>
#include "syn/netlist.h"
#include "syn/twolevel.h"

namespace SYN {

  class DwaveSyn;

  class Decomp;

  class Opt {
  public:
    Opt(Model* top) : _top(top) {}
    ~Opt() {}
    bool sweep(bool with_seq);
    bool cleanup(bool with_seq);
    void eliminate(int thre, int limit);
    void collapse(int limit);
    void simplify();
    void resub(bool, int);
    void techDecomp(int in_and, int in_or, int balance);
    void wideGateMerge(bool tree_mode);
    void setVerbose(int v) {_verbose = v;}
    void gateCollapse(); // collapse the narrow gate into wide gate after technology decomposition
    void countWideGate(); // count wide gate after AndOr decomp
    static void msg(int lvl, const char* fmt, ...) {
      if (lvl <= _verbose) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
      }
    }

  protected:
    Model* _top;
    //std::vector<Gate*> _region;
    static int _verbose;

    bool resub(Gate*, bool);
    int nodeValue(Gate*, int, std::vector<Gate*>&);
    std::vector<Gate*> decompAndOr(Gate* gate, Model* top_model);
    std::vector<Gate*> getMFFC(Gate *gate);

  };
  
  extern Model* myTop;
}

#endif //_SYN_OPT_H_
