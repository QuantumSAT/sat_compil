/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _SYN_EXTRACT_H_
#define _SYN_EXTRACT_H_

#include "optimize.h"
#include "syn/utility/sparse.h"
#include "syn/multilevel/fast_extract_int.h"

namespace SYN_SIS {
  extern int ONE_PASS;
  extern int FX_DELETE;
  extern int LENGTH1;
  extern int LENGTH2;
  extern int OBJECT_SIZE;
  extern int DONT_USE_WEIGHT_ZERO;
}

namespace SYN {

  class Extract : public Opt {
  public:
    Extract(Model* top) : Opt(top), _preserve_level(false) {}
    ~Extract() {}
    void setPreserveLevel(bool flag) {_preserve_level = flag;}
    void resetIndex() {
      opin2index.clear();
      index2opin.clear();
    };
    
    void extract();
    void speedDecomp(size_t in_bound, int step);
    bool speedDecomp(Gate*, size_t in_bound);
    bool andOrDecomp(Gate*, size_t in_bound);
    bool simpleGateDecomp(Gate*, size_t in_bound);
    //void speedUp();
    //bool speedUp(Gate*, Delay);
    //bool formCone(Gate*, Delay, std::vector<Gate*>&, int, int);
    void deleteCone(Gate*);

    int getOpinIndex(Pin* pin) {
      int index;
      if (opin2index.find(pin) != opin2index.end()) index = opin2index[pin];
      else {
        index = opin2index.size();
        setOpinIndex(pin, index);
      }
      return index;
    }
    void setOpinIndex(Pin* pin, int i) {
      opin2index[pin] = i; 
      index2opin[i] = pin;
    }
    Pin* getOpinForIndex(int i) {return index2opin[i];}
    
    //gate conversion
    void fx_node_to_sm(Gate* node, SYN_SIS::sm_matrix* matrix) {
      node_to_sm(node, matrix, false);
    }
    void ex_node_to_sm(Gate* node, SYN_SIS::sm_matrix* matrix) {
      node_to_sm(node, matrix, true);
    }
    Gate* ex_sm_to_node(SYN_SIS::sm_matrix* mat) {return sm_to_node(mat);}
    Gate* fx_sm_to_node(SYN_SIS::sm_matrix* mat) {return sm_to_node(mat);}
    void ex_setup_globals_single(Gate*);
    void ex_free_globals() {resetIndex();}

    //gate generation based on extraction 
    void sd_reexpress(SYN_SIS::sm_matrix*, SYN_SIS::rect_t*, int, int, int*);
    SYN_SIS::array_t* dd_reexpress(SYN_SIS::sm_matrix*, SYN_SIS::ddivisor_t*, int*);
    int fx_divide_each_fanout(SYN_SIS::array_t*, Gate*, int);
  private:
    bool _preserve_level;

    bool onePass() const {return SYN_SIS::ONE_PASS;}
    void setOnePass(bool flag) {SYN_SIS::ONE_PASS = (int)flag;}
    int length1() const {return SYN_SIS::LENGTH1;}
    void setLength1(int l) {SYN_SIS::LENGTH1 = l;}
    int length2() const {return SYN_SIS::LENGTH2;}
    void setLength2(int l) {SYN_SIS::LENGTH2 = l;}
    int objectSize() const {return SYN_SIS::OBJECT_SIZE;}
    void setObjectSize(int s) {SYN_SIS::OBJECT_SIZE = s;}
    int dontUseWeightZero() const {return SYN_SIS::DONT_USE_WEIGHT_ZERO;}
    void setDontUseWeightZero(int w) {SYN_SIS::DONT_USE_WEIGHT_ZERO = w;}
    
    std::unordered_map<Pin*, int> opin2index;
    std::unordered_map<int, Pin*> index2opin;

    void node_to_sm(Gate*, SYN_SIS::sm_matrix*, bool with_value_cell);
    Gate* sm_to_node(SYN_SIS::sm_matrix*);
   };

  extern Extract* myExt;

}

#endif //_SYN_EXTRACT_H_
