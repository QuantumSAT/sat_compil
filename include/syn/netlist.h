/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

/* head file for netlist representation
 * we need to handle netlist with both mapped instances
 * and generic gates, flipflops and latches.
 */

#ifndef _SYN_NETLIST_H_
#define _SYN_NETLIST_H_

//#include "util/util.h"



#include <stdio.h>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
//#include <fstream>
//#include <algorithm>
//#include "data_model/data_model.h"

#include "syn/function.h"


namespace SYN {
  
  class Pin;
  class Net;
  class Element;
  class Model;
  class Inst;
  class Gate;

  template<class T> struct indexCmp {
    bool operator() (const T& x, const T& y) const {return x->index() < y->index();}
  };

  class Pin {
  public:
    friend class Model;
    enum DIR {INPUT, OUTPUT, BIDIR};
    enum TYPE {MPIN, GPIN};
    enum FUNC {DATA, CLK, CTRL};

    Pin(const char* name, Element* owner, DIR dir, FUNC func = DATA) 
      : _name(name), _dir(dir), _type(GPIN), _func(func), _net(0), 
        _element(owner), _index(-1), _level(-1), pin_id_(0) {}
    Pin(const char* name, Model* owner, DIR dir, FUNC func = DATA) 
      : _name(name), _dir(dir), _type(MPIN), _func(func), _net(0), 
        _model(owner), _index(-1), _level(-1), pin_id_(0) {}
    ~Pin();

    const std::string& getName() const {return _name;}
    const std::string& name() const {return _name;}
    DIR dir() const {return _dir;}
    bool isInpin() const {return _dir == INPUT;}
    bool isOutpin() const {return _dir == OUTPUT;}
    bool isBidir() const {return _dir == BIDIR;}
    void setDir(DIR dir) {_dir = dir;}

    bool isModelPin() const {return _type == MPIN;}
    bool isInstPin() const;
    bool isGatePin() const;
    Model* getModel() const {return isModelPin()? _model : 0;}
    Gate* getGate() const;
    Inst* getInst() const;
    Element* owner() const {return isModelPin()? 0 : _element;}

    int index() const {return _index;}


    Net* net() const {return _net;}
    void connect(Net*);
    void disconnect();

    int getLevel() const {return _level;}
    void setLevel(int l) {_level = l;}

    //tianheng pin_id for routing
    int32_t pin_id() const{ return pin_id_; }
    void set_pin_id(int32_t id) { pin_id_ = id; } 

    void createPinID(int col, int row, int pos) ;
    void clearPinID() {pin_id_ = 0 ; }


    

  private:
    std::string _name;
    DIR _dir:2;
    TYPE _type:1;
    FUNC _func:2;
    Net* _net;
    union {
      Model* _model;
      Element* _element;
    };
    int _index;
    int _level;
    void setIndex(int i) {_index = i;}

    void updateRequired(bool from_fanout);
    // tianheng id for routing
    int32_t pin_id_;
    // call the func createSpinID in route_graph.h
    // inpin 1: pos=3, inpin 2: pos=4, outpin: pos = 5

  };

  class Net {
  public:
    friend class Pin;
    Net(const char* name, bool dummy=false) : _name(name), is_routed_(false), is_dummy_(dummy) {
      _index = index_count; 
      ++index_count;
    }
    Net(const std::string& name, bool dummy=false) : _name(name), is_routed_(false), is_dummy_(dummy) {
      _index = index_count; 
      ++index_count;
    }
    ~Net() {}
    
    Pin* uniqSource() const {
      Pin* pin = 0;
      PIN_ITER_CONST pitr = begin();
      if (pitr != end()) {
        pin = *pitr;
        if (!pin->isOutpin()) pin = 0;
        else {
          ++pitr;
          if (pitr != end()) {
            Pin* pin1 = *pitr;
            if (pin1->isOutpin()) pin = 0;
          }
        }
      }
      return pin;
    }
    Pin* source() const {
      PIN_ITER_CONST pitr = begin();
      Pin* pin = 0;
      if (pitr != end()) {
        pin = *pitr;
        if (!pin->isOutpin()) pin = 0;
      }
      return pin;
    }
    unsigned int getIndex() const { return _index; }
    int numPin() const {return (int)_pin_list.size();}
    int numSink() const {return numPin() - numSrc();}
    int numSrc() const {
      int n = 0;
      for (PIN_ITER_CONST pitr = begin(); pitr != end() && (*pitr)->isOutpin();
           ++pitr, ++n) {
      }
      return n;
    }

    Pin* getSink(unsigned index) const {
      assert(uniqSource());
      PIN_ITER_CONST pitr = begin();
      ++pitr;
      Pin* pin = nullptr;
      assert((int)index < numSink());
      for (unsigned i = 0; i < index; ++i) {
        ++pitr;
      }
      pin = *pitr;
      if (!pin->isInpin()) pin = NULL;
      return pin;
    }

    //iterator for pins
    typedef std::list<Pin*>::iterator PIN_ITER;
    typedef std::list<Pin*>::const_iterator PIN_ITER_CONST;
    PIN_ITER begin() {return _pin_list.begin();}
    PIN_ITER end() {return _pin_list.end();}
    PIN_ITER_CONST begin() const {return _pin_list.begin();}
    PIN_ITER_CONST end() const {return _pin_list.end();}

    //added by Juexiao
    std::string name() const { return _name ; }
    //bruin::Box bbox() const { return bbox_ ; }
    //void setBox(bruin::Box box) { bbox_ = box ; }
    bool isDummy() const { return is_dummy_; }



    // Added by Tianheng routing info
    void set_routing_results(const std::vector<std::pair<int32_t, int32_t> >& results) { 
      is_routed_ = true; 
      routing_results_ = results;
    }

    void set_routing_orig(const std::vector<std::pair<int32_t, int32_t> >& results) { 
      routing_results_orig_ = results;
    }

    void clear_routing_results() { 
      is_routed_ = false; 
      routing_results_.clear(); 
    }
    const std::vector<std::pair<int32_t, int32_t>>& routing_results() const { return routing_results_; }
    const std::vector<std::pair<int32_t, int32_t>>& routing_results_orig() const { return routing_results_orig_; }

  private:
    static unsigned int index_count;

    unsigned int _index;
    std::string _name;
    //put drivers at the beginning of the list
    std::list<Pin*> _pin_list;
    //Model* _owner;
    void connect(Pin* pin) {
      if (pin->isOutpin()) _pin_list.push_front(pin);
      else _pin_list.push_back(pin);
    }
    //disconnect may consider to free itself if it becomes dangling
    void disconnect(Pin* pin) {
      _pin_list.remove(pin);
      if (numPin() == 0) delete this;
    }

    //added by Juexiao

    //bruin::Box bbox_ ;
    bool is_dummy_;

    // Added by Tianheng, routing info
    std::vector<std::pair<int32_t, int32_t>> routing_results_;
    std::vector<std::pair<int32_t, int32_t>> routing_results_orig_;
    bool is_routed_;
  };

  /* Element is the base type of logic element in a model
   * Inst is instance of library cell
   * Gate is premitive logic gate or FF/latch
   */
  class Element {
  public:
	friend class Model;
    Element(const char* name, Model* owner);
    Element(const std::string& name, Model* owner);
    ~Element();

    Model* owner() const {return _owner;}

    typedef std::vector<Pin*>::iterator PIN_ITER;
    PIN_ITER begin() {return _pin_list.begin();}
    PIN_ITER end() {return _pin_list.end();}
    size_t getNumPin() const {return _pin_list.size();}
    Pin* getPin(size_t i) const {return _pin_list[i];}

    const std::string& name() const {return _name;}
    const std::string& getName() const {return _name;}
    void setName(const std::string& nn) {_name = nn;}
    void setName(const char* nn) {_name = nn;}
    bool preserve() const {return _preserve != 0;}
    bool noTouch() const {return _no_touch != 0;}
    virtual bool isInst() const { return false;}
    virtual bool isGate() const { return false;}
    virtual bool hasArc(int in_id, int out_id) const {return false;}
    virtual bool hasArc(Pin* ipin, Pin* opin) const {return false;}

    int index() const {return _index;}
    void setIndex(int i) {_index = i;}
  protected:
    unsigned int _preserve:1;
    unsigned int _no_touch:1;
    std::string _name;
    Model* _owner;
    std::vector<Pin*> _pin_list;
    int _index;
  private:
    static int index_count;
 };
  
  class Inst : public Element {
  public:
    Inst(const char* name, /*nimbus::DataModel::*/Model* mod, Model* owner);
    ~Inst() {}

    bool isInst() const {return true;}
    bool isGate() const {return false;}
#if 0
    /*nimbus::DataModel::*/Model* downModel() const {return _down_model;}
    bool isFF() const {return strncmp(_down_model->name(), "FD", 2) == 0 ||
        strncmp(_down_model->name(), "LD", 2) == 0; }
    bool isRAMB() const {return strncmp(_down_model->name(), "RAM", 3) == 0;}
    bool isDSP() const {return strncmp(_down_model->name(), "DSP", 3) == 0;}
    bool isLUT() const {return strncmp(_down_model->name(), "LUT", 3) == 0;}
    bool hasArc(Pin* inpin, Pin* opin) const {return false;}
    bool hasArc(int in_id, int o_id) const {return false;}
#endif
  private:
    /*nimbus::DataModel::*/Model* _down_model;
  };

  class Gate : public Element {
  public:
    Gate(const char* name = "", Model* owner = 0) 
      : Element(name, owner), _func(0), _type(COMB) {
      Pin* pin = new Pin("O", this, Pin::OUTPUT);
      _pin_list.push_back(pin);
      _nout = 1;
      pos_x_ = -1 ;
      pos_y_ = -1 ;
    }
    Gate(std::string& name, Model* owner) 
      : Element(name, owner), _func(0), _type(COMB), _nout(0) {
      Pin* pin = new Pin("O", this, Pin::OUTPUT);
      _pin_list.push_back(pin);
      _nout = 1;
      pos_x_ = -1 ;
      pos_y_ = -1 ;
    }
    Gate(const Gate& rhs);
    ~Gate() {delete _func;}
    Gate& operator=(const Gate& rhs);
    
    enum TYPE {COMB, FF, LATCH};
    enum PHASE {POS_UNATE, NEG_UNATE, BINATE, UNKNOWN};

    enum DWAVETYPE {INVALID = -1, 
                    ZERO = 0, 
                    ONE = 1, 
                    OR = 2, 
                    AND = 3, 
                    BUF = 4};

    bool isInst() const {return false;}
    bool isGate() const {return true;}
    bool isComb() const {return _type == COMB;}
    bool isSeq() const {return _type != COMB;}
    bool isFF() const {return _type == FF;}
    bool isLatch() const {return _type == LATCH;}

    bool hasArc(Pin* ipin, Pin* opin) const {return true;}
    bool hasArc(int iidx, int oidx) const {return true;}
    size_t getNumIn() const {return getNumPin() - _nout;}
    size_t getNumOut() const {return _nout;}
    Pin* getOpin(size_t id=0) const {return _pin_list[id];}
    Pin* getInpin(size_t id) const {return _pin_list[id + _nout];}
#if 0
    int getFaninIndex(Pin* inpin) const {
      for (size_t i = 0; i < getNumIn(); ++i) {
        if (getInpin(i) == inpin) return i;
      }
      return -1;
    }
#endif
    int getFaninIndex(Gate* input) const;
    int getFaninIndex(Pin* input) const;
    PHASE getFaninPhase(size_t idx);
    //get all the fanins except the given input
    //int getFaninIndex(Gate* input, std::vector<Pin*>& in_set) const;
    void makeCommonBase(Gate* input,
                        std::vector<int>& index_vec,
                        std::vector<int>& input_index_vec,
                        std::vector<Pin*>& new_input_vec);
    bool baseContain(Gate*);

    void setFunc(Function* func, std::vector<Pin*>& inputs, bool dummy=false);    
    void replaceFunc(Function*);
    void setFunc(bool);   
    Function* getFunc() const {return _func;}
    CoverTable* getOnCvt() const {return _func->getOnSet();}

    Function::TYPE getFuncType() const {return _func->type();}
    bool isConstOne() const {return getFuncType() == Function::ONE;}
    bool isConstZero() const {return getFuncType() == Function::ZERO;}
    bool isConst() const {return isConstOne() || isConstZero();}
    bool isAnd() const {return getFuncType() == Function::AND;}
    bool isOr() const {return getFuncType() == Function::OR;}
    bool isBuf() const {return getFuncType() == Function::BUF;}
    bool isInv() const {return getFuncType() == Function::INV;}
    bool isXor() const {return getFuncType() == Function::XOR;}
    bool isXor2() const {return isXor()&&(getNumIn() == 2);}
    bool isXnor() const {return getFuncType() == Function::XNOR;}
    bool isXnor2() const {return isXnor()&&(getNumIn() == 2);}
    bool isMux2() const {return getFuncType() == Function::MUX2;}
    bool isComplex() const {
      if(isConst()||isOr()||isAnd()||isBuf()||isInv()) return false;
      return true;
    }
    void setAndGate(std::vector<Pin*>& inpins, std::vector<Cube::VAL>& polarities);
    void setOrGate(std::vector<Pin*>& inpins, std::vector<Cube::VAL>& polarities);

    //reset will use function or covertable directly, do not free them
    void reset(Function*, std::vector<Pin*>&);
    void reset(CoverTable*, std::vector<Pin*>&);
    
    void collapse(Gate*);//collapse an input gate
    void collapse(Gate*, Cube::VAL);//collapse constant input
    bool substitute(Gate*, bool use_complement);
    Gate* divide(Gate* divisor, bool phase, Gate** remainder);
    void divide(Gate* divisor, bool phase, CoverTable*& quotient, CoverTable*& remainder);
    Gate* divideOnePhase(Gate*, bool);

    Gate* gateAnd(Gate*);
    Gate* gateOr(Gate*);
    Gate* gateInv();
    Gate* gateXor(Gate*);
    Gate* gateXnor(Gate*);
    Gate* literal(bool phase);
    Gate* largestCubeDivisor();
    //bool contains(Gate*);
    //bool equal(Gate*);
  
    void cofactor(Gate* input, CoverTable*& p, CoverTable*& q, CoverTable*& r);

    bool removeDupFanin();
    bool removeConstFanin();
    bool removeRedFanin();
    bool scc();
    bool simplifyCvt();

    void simplify() {
      removeConstFanin();
      removeDupFanin();
      simplifyCvt();
      removeRedFanin();
      scc();//simplifyCvt();
    }

    bool isValid() {
      bool flag = true;
      CoverTable* cvt = getOnCvt();
      if (getNumIn() != cvt->getNumIn() && !(cvt->isConstOne() || cvt->isConstZero())) {
        //printf("gate %s has %d inputs, but cvt has %d inputs\n",
        //       getName().c_str(), getNumIn(), cvt->getNumIn());
        flag = false;
      }
      if (!cvt->isValid()) {
        //printf("gate %s cvt is not valid\n", getName().c_str());
        flag = false;
      }
      return flag;
    }
    void print(FILE* fp) {
      fprintf(fp, "gate %s: ", getName().c_str());
      for (size_t i = 0; i < getNumIn(); ++i) {
        Pin* ipin = getInpin(i);
        Pin* drv = ipin->net()->source();
        if (drv->isModelPin()) fprintf(fp, " %s", drv->getName().c_str());
        else fprintf(fp, " %s", drv->owner()->getName().c_str());
      }
      fprintf(fp, "\n");
      getOnCvt()->print(fp);
    }

    void build2InputInv(Pin* input);
    void build2InputBuf(Pin* input);
    void set2InputInv(Pin* input);
    void set2InputBuf(Pin* input);


    //added by Juexiao for placement
    int posX() const { return pos_x_ ; }
    int posY() const { return pos_y_ ; }
    void setPosX(int x) { pos_x_ = x ; }
    void setPosY(int y) { pos_y_ = y ; }

    void updatePinID() {
      assert(pos_x_ != -1) ;
      assert(pos_y_ != -1) ;
      assert(_pin_list.size() == 3) ;
      int inpin_index = 0 ;
      for (PIN_ITER it = begin(); it != end(); it++) {
        if ((*it)->isInpin()) {
          if (inpin_index)
            (*it)->createPinID(posX(), posY(), 4) ;
          else 
            (*it)->createPinID(posX(), posY(), 3) ;
          inpin_index++ ;
          
        } else if ((*it)->isOutpin()) {
          (*it)->createPinID(posX(), posY(), 5) ;
        } else {
          assert(0) ;
        }

      }
    }


    // tianheng pin_gate_type for verification
    DWAVETYPE getDwaveType() const;
    Gate::PHASE getPinPhase(const Pin* pin) const; // return KNOWN means error

  private:
    Function* _func;
    TYPE _type;
    size_t _nout;
    //bool _dup_free;
    //bool _scc_minimal;
    //remove those inpins with mark
    void removeInpin(std::vector<bool>& ipin_tag);

    //added by Juexiao for placement
    int pos_x_ ;
    int pos_y_ ;
  };

  class Model {
  public:
    Model(const char* name);
    ~Model();

    Model& operator=(const /*nimbus::DataModel::*/Model& nmod);
    Gate* vcc() {return _vcc;}
    Gate* gnd() {return _gnd;}

    const std::string& name() const {return _name;}

    typedef std::unordered_set<Net*>::iterator NET_ITER;
    typedef std::set<Element*>::iterator ELEM_ITER;
    typedef std::set<Pin*>::iterator PORT_ITER;

    void erase(ELEM_ITER itr) {_elem_list.erase(itr);}
    void erase(Element* elem) {_elem_list.erase(elem);}
    void add(Element* elem) {
      if (elem->getName() == "") {
        char buf[20];
        sprintf(buf, "elem%d", ++_gate_max_id);
        elem->setName(buf);
      } else {
        const char* name = elem->getName().c_str();
        if (strncmp(name, "elem", 4) == 0) {
          int id = atoi(name+4);
          if (id >= 0 && id >= _gate_max_id) _gate_max_id = id;
        }
      }
      elem->_owner = this;
      _elem_list.insert(elem);
    }
    ELEM_ITER elem_begin() {return _elem_list.begin();}
    ELEM_ITER elem_end() {return _elem_list.end();}
    unsigned getNumElem() const {return (unsigned)_elem_list.size();}
#if 0
    void erase(NET_ITER itr) {_net_list.erase(itr);}
    void erase(Net* net) {_net_list.erase(net);}
    void add(Net* net) {_net_list.insert(net);}
    NET_ITER net_begin() {return _net_list.begin();}
    NET_ITER net_end() {return _net_list.end();}
    int getNumNet() const {return _net_list.size();}
#endif
    void erase(PORT_ITER itr) {_io_list.erase(itr);}
    void erase(Pin* pin) {_io_list.erase(pin);}
    void add(Pin* pin);
    PORT_ITER port_begin() {return _io_list.begin();}
    PORT_ITER port_end() {return _io_list.end();}
    unsigned getNumPort() const {return (unsigned)_io_list.size();}

    void mapToGeneric();

    void topoOrder(std::vector<Pin*>&);
    std::set<Element*, indexCmp<Element*> > elemList(){return _elem_list;}
    void updateTiming(bool full);
    int getMaxLevel() const {return _max_level;}

    void writeBlif(char*);
    // new added for dwave route
    std::unordered_set<Net*> netList() {return _net_list; }
    void getNetListFromElem();
    NET_ITER net_begin() {return _net_list.begin();}
    NET_ITER net_end() {return _net_list.end();}
    unsigned getNumNets() const {return (unsigned)_net_list.size();}

    std::vector<Gate*> getModelGates();
    std::vector<Pin*> getModelIOs();

    //added by Juexiao for placement
    void setPlace(bool finish) { _is_place = finish ; }
    bool isPlaced() const { return _is_place ; }
    void resetPlace() ;

  private:
    std::string _name;
    /*nimbus::DataModel::*/Model* _nimbus_model;
    std::unordered_set<Net*> _net_list;
    std::set<Pin*, indexCmp<Pin*>> _io_list;
    std::set<Element*, indexCmp<Element*> > _elem_list;
    Gate* _vcc, *_gnd;
    int _io_max_id, _gate_max_id;
    int _max_level;

    void topoOrderRec(Pin*, std::vector<Pin*>&, std::unordered_set<Pin*>&);

    //added by Juexiao for placement
    bool _is_place ;
  };
  
  //added by juexiao to solve IO block
  class ModelWithIO : public Model {

    public:

      ModelWithIO(const char* name) : Model(name) {}
      void elaborateNetlist() ;

  } ;

}

#endif //_SYN_NETLIST_H_
