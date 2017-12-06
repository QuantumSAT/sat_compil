#ifndef _SYN_CUT_H_
#define _SYN_CUT_H_

#include "optimize.h"
#include "netlist.h"

namespace SYN {
  class Decomp;
  class Label;
  class Cut{
    public:
      typedef std::set<Pin*> PINSET;
      typedef std::set<Pin*>::iterator PINSET_ITER;

      /*
      Cut(const Cut* cut) {
        pin_set_ = *(cut->getPinSet());
      }
      */

      ~Cut() {}

      void addPin(Pin* pin) {pin_set_.insert(pin);}
      PINSET* getPinSet() {return &pin_set_;}
      void mergeCut(Cut* cut);
      size_t getCutSize() {return pin_set_.size();}
    private:
      PINSET pin_set_;
  }; //class Cut
}
#endif
