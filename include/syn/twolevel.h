/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _TWOLEVEL_H_
#define _TWOLEVEL_H_

#include "syn/function.h"
#include "syn/twolevel/espresso.h"

namespace SYN {

  typedef SYN_SIS::pset_family pcover;
  typedef SYN_SIS::pset pcube;
  typedef SYN_SIS::pPLA pPLA;
  typedef CoverTable* ptable;
  
  class TwoLevelOpt {
  public:
    TwoLevelOpt();
    ~TwoLevelOpt();
    
    enum OBJECT {CUBE=SYN_SIS::OBJ_CUBE, LITERAL=SYN_SIS::OBJ_LITERAL, SUPPORT=SYN_SIS::OBJ_SUPPORT};
    enum ALGORITHM {ESPRESSO=SYN_SIS::ESPRESSO, NOCOMP=SYN_SIS::NOCOMP, SNOCOMP=SYN_SIS::SNOCOMP, DCSIMPLIFY=SYN_SIS::DCSIMPLIFY};
    void setFunction(Function*);
    Function* getFunction(bool with_off = false, bool with_dc = false);
    int getNumCube() const {return _pla->F->count;}

    void heuristic(ALGORITHM, OBJECT);
    void exact(OBJECT);
    void separate(OBJECT, bool);

    void scc() {
      //sf_contain will free old pcover
      _pla->F = SYN_SIS::sf_contain(_pla->F);
    }
    void simplify() {
      //simplify will free old cubelist
      pcover res = SYN_SIS::simplify(cube1list(_pla->F));
      freeCover(_pla->F);
      _pla->F = res;
    }
    void irredundant() {
      if (_pla->D == 0) _pla->D = newCover(0);
      //irredundant will modify F
      _pla->F = SYN_SIS::irredundant(_pla->F, _pla->D);
    }
    bool tautology() {
      int res = SYN_SIS::tautology(cube1list(_pla->F));
      return res == 1;
    }
    void setStrongOpt(bool flag) {SYN_SIS::use_super_gasp = (SYN_SIS::esp_bool)flag;}
    
    //CoverTable optimization
    void scc(ptable&);
    void irredundant(ptable&);
    void simplify(ptable&);
    bool tautology(const ptable&);

    ptable not1(const ptable);
    ptable and2(const ptable, const ptable);
    ptable or2(const ptable, const ptable);
    ptable xor2(const ptable, const ptable);
    ptable xnor2(const ptable, const ptable);

    pcover cvtToSet(ptable);
    ptable setToCvt(pcover);

  private:
    pPLA _pla;
    int _nin;
    int _nout;
    static int _reference;

    ptable getOffSet();
    ptable getDcSet();
    ptable getOnSet();
    
    void resize(int nin, int nout);
    void prepare(ptable);
    bool prepare(ptable t1, ptable t2) {
      if (!match(t1, t2)) return false;
      prepare(t1);
      return true;
    }
    bool match(ptable t1, ptable t2) {
      return t1->getNumIn() == t2->getNumIn() && t1->getNumOut() == t2->getNumOut();
    }
    
    void initPLA(pPLA pla) {
      pla->F = pla->D = pla->R = 0;
      pla->filename = 0;
      pla->pla_type = 3;
      pla->phase = 0;
      pla->pair = 0;
      pla->label = 0;
      pla->symbolic = 0;
      pla->symbolic_output = 0;
    }

    void cleanPLA(pPLA pla) {
      if (pla->F) {
        freeCover(pla->F);
        pla->F = 0;
      }
      if (pla->R) {
        freeCover(pla->R);
        pla->R = 0;
      }
      if (pla->D) {
        freeCover(pla->D);
        pla->D = 0;
      }
      if (pla->phase) {
        freeCube(pla->phase);
        pla->phase = 0;
      }
    }

    void simplify(pcover& F) {
      pcover res = SYN_SIS::simplify(SYN_SIS::cube1list(F));
      SYN_SIS::free_cover(F);
      F = res;
    }

    pcover newCover(int n) {
      pcover cv = SYN_SIS::sf_new(n, SYN_SIS::cube.size);
      return cv;
    }
    void freeCover(pcover cc) {SYN_SIS::sf_free(cc);}
    void freeCube(pcube cc) {SYN_SIS::set_free(cc);}

    bool is_one(pcover p) const {return p->count == -1;}
    bool is_zero(pcover p) const {return p->count == -2 || p->count == 0;}
  };

}

#endif //_TWOLEVEL_H_
