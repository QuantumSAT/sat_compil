#ifndef _SYN_LABEL_H_
#define _SYN_LABEL_H_

#include "optimize.h"
#include "netlist.h"

namespace SYN {

  class Cut;
  class Decomp;

  class Label{
    public:
      // LABEL_DATA is a set of <delay,cut*>
      // the set is in the decreasing order of delays
      typedef struct {
        bool operator() (const std::pair<size_t,Cut*>& p1, const std::pair<size_t,Cut*>& p2) {
          return (p1.first < p2.first);
        
        }
      
      } LabelDataComp;

      typedef std::multiset<std::pair<size_t,Cut*>,LabelDataComp> LABEL_DATA;
      Label():add_itself_flag_(false){}

      ~Label(){
        for (LABEL_DATA::iterator it = label_.begin(); 
             it != label_.end(); ++it) {
          Cut* cut = it->second;
          if (cut) {
            delete cut;
          }
        }
      }
      
      void addLabelItem(size_t delay, Cut* cut); 
      void delItemsByDelay(size_t delay);
      LABEL_DATA* getLabel() {return &label_;}
      //std::pair<size_t,Cut*>* getLabelItem(size_t ind) {return &label_[ind];}
      std::pair<size_t,Cut*> const *  getLabelItem(size_t ind) {
        LABEL_DATA::iterator it = label_.begin();
        for (int i = 0; i < ind; ++i) {
          ++it;
        }

        std::pair<size_t,Cut*> const *return_ptr = &(*it);
        return return_ptr;
      }


      size_t getLabelSize() {return label_.size();}

      std::set<size_t> getDelays() {return delays_;}
      size_t getDelaySize() {return delays_.size();}
      size_t getOptDelay() {return *delays_.begin();}
      size_t getWorstDelay() {return *delays_.crbegin();}
      
      size_t getIndexDelay(int index) {
        assert(index<delays_.size());
        
        std::set<size_t>::iterator it = delays_.begin();
        for (int i=0; i<index; ++i) {
          ++it;        
        }
        return *(it);
      }

      std::vector<Cut*> getDelayCuts(size_t delay);

      void setAddItselfCutFlag(){add_itself_flag_=true;}
      void clearAddItselfCutFlag(){add_itself_flag_=false;}
      bool getAddItselfCutFlag() {return add_itself_flag_;}

      bool isCutInLabel(Cut* cut);
      void getFixDelaySubLabel(const int& delay, Label* new_label);


    private:
      LABEL_DATA label_;
      std::set<size_t> delays_;
      bool add_itself_flag_;

  }; //class Label

}
#endif
