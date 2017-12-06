#ifndef _SYN_DWAVESYN_H_
#define _SYN_DWAVESYN_H_

//#include "optimize.h"
#include "decomp.h"


namespace SYN {
  class DwaveSyn : public Decomp {
  public:
    DwaveSyn(Model* top) : Decomp(top) {}

    void hello() { 
      msg(0,"hello!\n"); 
    }

    // convert the gate netlist into a netlist 
    // that only contain and gates and or gates
    //void convertAndOr();
    void convertAndOr(int mode);

    void testType();
  private:
    bool getPIs(const std::vector<Pin*>& opin_vec, std::vector<Pin*>& pis);
    bool insertBufInNet(Net* net);

  }; //end of class DwaveSyn

} //end of namespace SYN



#endif
