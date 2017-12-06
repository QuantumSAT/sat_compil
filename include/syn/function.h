/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

/* head file for functional representation and operations  */

#ifndef _SYN_H_
#define _SYN_H_

#include <assert.h>
//#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

namespace SYN {
    
  class SizeTemplate;
  class Cube;
  class CoverTable;
  class Function;
  
#if 0 //__WORDSIZE == 64
  typedef uint64_t* pset;
  typedef uint64_t word_t;
#else
  typedef uint32_t* pset;
  typedef uint32_t word_t;
#endif

  class SizeTemplate {
  public:
    SizeTemplate(size_t nin, size_t nout) {setSize(nin, nout);}
    ~SizeTemplate() {assert(_cnt == 0);}

    void setSize(size_t nin, size_t nout) {
      _n_in = nin;
      _n_out = nout;
      _n_bits = bpflg() + (nin<<1) + outIndex(nout);
      _n_words = (_n_bits >> logbpi()) + ((_n_bits & wordMask())? 1 : 0);
      //if (_n_words < 1) _n_words = 1;
      _cnt = 0;
    }
    size_t getNumIn() const {return _n_in;}
    size_t getNumOut() const {return _n_out;}
    size_t getNumWord() const {return _n_words;}
    size_t getSize() const {return _n_words * sizeof(word_t);}
    size_t getWordForOutput(size_t index) const {
      return (bpflg() + (_n_in<<1) + outIndex(index)) >> logbpi();
    }
    size_t getWordForInput(size_t index) const {
      return (bpflg() + (index<<1)) >> logbpi();
    }
    size_t getBitForOutput(size_t index) const {
      return (bpflg() + (_n_in<<1) + outIndex(index)) & wordMask();
    }
    size_t getBitForInput(size_t index) const {
      return (bpflg() + (index<<1)) & wordMask();
    }
    void reference() {++ _cnt;}
    void dereference() {
      if ((-- _cnt) == 0) delete this;
    }

    bool operator==(const SizeTemplate& rhs) const {
      return _n_in == rhs._n_in && _n_out == rhs._n_out;
    }
    bool operator!=(const SizeTemplate& rhs) const {
      return _n_in != rhs._n_in || _n_out != rhs._n_out;
    }
  private:
    size_t _n_in;
    size_t _n_out;
    size_t _n_bits;
    size_t _n_words;
    size_t _cnt;
#if 0 //__WORDSIZE == 64
    size_t bpi() const {return 64;}
    size_t logbpi() const {return 6;}
#else
    size_t bpi() const {return 32;}
    size_t logbpi() const {return 5;}
#endif
    size_t bpflg() const {return 8;}
    size_t outIndex(size_t idx) const {return _n_out>1? (size_t)idx : (size_t)0;}
    size_t wordMask() const {return bpi()-1;}
  };

  class Cube {
  public:
    friend class CoverTable;
    enum VAL {VOIDn=0, ONE=2, ZERO=1, DASH=3};

    Cube(CoverTable* cvt);
    Cube& operator=(const Cube&);

    ~Cube();

    void setFull();
    bool isFull() const;
    void setVar(size_t idx, VAL val);
    VAL getVar(size_t idx) const;
    bool isOne(size_t idx) const {return getVar(idx) == ONE;}
    bool isZero(size_t idx) const {return getVar(idx) == ZERO;}
    bool isDash(size_t idx) const {return getVar(idx) == DASH;}
    void addToOut(size_t idx);
    void removeFromOut(size_t idx);
    bool isImplForOut(size_t idx) const;

    size_t getNumIn() const {return _template->getNumIn();}
    size_t getNumOut() const {return _template->getNumOut();}
    size_t getNumLiteral() const;
    bool isEmpty() const {
      return _data == 0 || getNumLiteral() == 0;
    }

    int getCubeDist(const Cube& cube) const;
    bool cubeIntersect(const Cube&, const Cube&, bool keep_empty=false);
    bool cubeIntersect(const Cube& cube, bool keep_empty=false) {
      return cubeIntersect(*this, cube, keep_empty);
    }
    bool superCube(const Cube&, const Cube&);
    bool cubeCofactor(const Cube& cube, const Cube& factor);
    //bit-wise complement
    void cubeBitwiseCompl(const Cube&);
    bool cubeContainment(const Cube contained) const;
    void printCube(FILE* fp) const;

    //tag
    bool isActive() const {return hasFlag(active());}
    bool isPrime() const {return hasFlag(prime());}
    bool isNonessen() const {return hasFlag(nonessen());}
    bool isRedundant() const {return hasFlag(redund());}
    bool isCovered() const {return hasFlag(covered());}
    bool isRelessen() const {return hasFlag(relessen());}
    bool isIndep() const {return hasFlag(indep());}
    bool isValid() const {
      for (size_t i = 0; i < getNumIn(); ++i) {
        if (getVar(i) == VOIDn) return false;
      }
      return true;
    }

    void setActive() {setFlag(active());}
    void setPrime() {setFlag(prime());}
    void setNonessen() {setFlag(nonessen());}
    void setRedundant() {setFlag(redund());}
    void setCovered() {setFlag(covered());}
    void setRelessen() {setFlag(relessen());}
    void setIndep() {setFlag(indep());}

    void resetActive() {resetFlag(active());}
    void resetPrime() {resetFlag(prime());}
    void resetNonessen() {resetFlag(nonessen());}
    void resetRedundant() {resetFlag(redund());}
    void resetCovered() {resetFlag(covered());}
    void resetRelessen() {resetFlag(relessen());}
    void resetIndep() {resetFlag(indep());}

    void print(FILE* fp) {
      for (size_t i = 0; i < getNumIn(); ++i) {
        VAL val = getVar(i);
        fprintf(fp, "%c", val==ONE? '1' : (val==ZERO? '0' : '-'));
      }
      fprintf(fp, "\n");
    }
   
    // TTH revise, change getData into public
    pset getData() const {return _data;}

  private:
    pset _data;
    SizeTemplate* _template;
    //empty constructor for covertable use
    Cube() : _data(0), _template(0) {}

    void setData(pset set) {
      if (!isEmpty() && isIndep()) (void)free((char*)_data);
      _data = set;
    }
    SizeTemplate* getTemplate() const {return _template;}
    void setTemplate(SizeTemplate* tt);

    void setFlag(int flag) {_data[0] |= flag;}
    void resetFlag(int flag) {_data[0] &= ~flag;}
    bool hasFlag(int flag) const {return _data && (_data[0] & flag);} 
    int active() const {return 0x8;}
    int prime() const {return 0x20;}
    int nonessen() const {return 0x10;}
    int redund() const {return 0x4;}
    int covered() const {return 0x2;}
    int relessen() const {return 0x1;}
    int indep() const {return 0x40;}
  };

  class CoverTable {
  public:
    friend class Cube;
    CoverTable(size_t n_in, size_t n_out, size_t cap=10)
      : _data(0), _template(0), _n_cube(0), _capacity(cap), _n_lit(-1) {
      reset(n_in, n_out, cap);
    }
    CoverTable(const CoverTable& rhs);
    CoverTable& operator=(const CoverTable& rhs);

    CoverTable(std::string& lut_init);
    CoverTable(bool val) : _data(0), _template(0), _n_cube((int)val) {}

    ~CoverTable() {clear();}

    enum TYPE {ZERO=0, ONE, INV, BUF, AND, OR, XOR, XNOR, MUX2, COMPLEX};
    enum PHASE {POS_UNATE, NEG_UNATE, BINATE, UNKNOWN};

    size_t getNumIn() const {return isConst()? 0 : _template->getNumIn();}
    size_t getNumOut() const {return isConst()? 1 : _template->getNumOut();}
    size_t getNumCube() const {return _data? _n_cube : 0;}
    bool empty() const {return _data && _n_cube==0;}
    size_t getNumLiteral();
 
    TYPE type();
    PHASE phase(size_t idx);
    bool isConstOne() const {
      return _data == 0 && _template == 0 && _n_cube;
    }
    bool isConstZero() const {
      return _data == 0 && _template == 0 && _n_cube == 0;
    }
    bool isConst() const {return isConstOne() || isConstZero();}
    void setToConst(bool val) {
      if (_data) {
        (void)free((char*)_data);
        _data = 0;
        _template->dereference();
        _template = 0;
      }
      _n_cube = (int)val;
      _n_lit = 0;
    }

    Cube& getCube(size_t n) {
      //the best way is to put _cube outside this cvt
      //so this function can be set const
      assert(_data && _template && n < _n_cube);
      _cube._data = _data + n*_template->getNumWord();
      _cube.resetIndep();
      return _cube;
    }
    void addCube(Cube& cube) {
      if (isConstOne()) return;
      if (cube.isFull()) setToConst(true);
      else {
        assert(_data && _template);
        if (++_n_cube > _capacity) resize(_n_cube + 10);
        pset addr = _data + (_n_cube-1) * _template->getNumWord();
        for (size_t i = 0; i < _template->getNumWord(); ++i) {
          addr[i] = cube._data[i];
        }
        _n_lit = -1;
      }
    }
    void cofactor(int idx, CoverTable*& p, CoverTable*& q, CoverTable*& r);
    CoverTable* cvtTransform(int, std::vector<int>&);
    void transform(int, std::vector<int>&);
    CoverTable* divide(CoverTable* divisor, CoverTable** remainder);
    void setFunc(TYPE, size_t n_in);

    bool isValid() {
      for (size_t i = 0; i < getNumCube(); ++i) {
        Cube& cube = getCube(i);
        if (cube.isValid() == false) return false;
      }
      return true;
    }

    void print(FILE* fp) {
      fprintf(fp, "--- cvt of %u input ---\n", getNumIn());
      for (size_t i = 0; i < getNumCube(); ++i) {
        Cube& cube = getCube(i);
        cube.print(fp);
      }
    }
  private:
    pset _data;
    SizeTemplate* _template;
    size_t _n_cube;
    size_t _capacity;
    int _n_lit;
    std::vector<PHASE> _phase;
    Cube _cube;
    void resize(size_t n) {
      if (n > _capacity) {
        _data = (pset)realloc((char*)_data, n * _template->getSize());
        _capacity = n;
      }
    }
    void reset(size_t nin, size_t nout, size_t capacity);
    void clear() {
      _cube._data = 0;
      _cube._template = 0;
      if (_template) {
        _template->dereference();
        (void)free((char*)_data);
      }
    }
  };

  //function is the main class used in netlist and optimization
  class Function {
  public:
    Function(CoverTable* on, CoverTable* off=0, CoverTable* dc=0)
      : _on_set(on), _off_set(off), _dc_set(dc), _type(UNKNOWN) {}
    Function(bool val) : _off_set(0), _dc_set(0), _type(UNKNOWN) {
      _on_set = new CoverTable(val);
    }
    Function(const Function& rhs) 
      : _on_set(0), _off_set(0), _dc_set(0), _type(rhs._type) {
      if (rhs._on_set) _on_set = new CoverTable(*rhs._on_set);
      if (rhs._off_set) _off_set = new CoverTable(*rhs._off_set);
      if (rhs._dc_set) _dc_set = new CoverTable(*rhs._dc_set);
    }
    ~Function() {
      if (_on_set) delete _on_set;
      if (_off_set) delete _off_set;
      if (_dc_set) delete _dc_set;
    }
    Function* not1();
    //make sure TYPE matches with covertable
    enum TYPE {ZERO=0, ONE, INV, BUF, AND, OR, XOR, XNOR, MUX2, COMPLEX, UNKNOWN};

    CoverTable* getOnSet() const {return _on_set;}
    CoverTable* getOffSet() const {return _off_set;}
    CoverTable* getDcSet() const {return _dc_set;}
    size_t getNumIn() const {return _on_set->getNumIn();}
    size_t getNumOut() const {return _on_set->getNumOut();}

    bool isConstOne() const {return _on_set->isConstOne();}
    bool isConstZero() const {return _on_set->isConstZero();}
    TYPE type() {
      if (_type == UNKNOWN) _type = (TYPE)_on_set->type();
      return _type;
    }
  private:
    CoverTable* _on_set;
    CoverTable* _off_set;
    CoverTable* _dc_set;
    TYPE _type;
  };
}

#endif //_SYN_H_
