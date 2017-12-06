/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set expandtab tabstop=2 softtabstop=2 shiftwidth=2: */

#ifndef _SYN_DECOMP_H_
#define _SYN_DECOMP_H_

#include <unordered_set>
#include <unordered_map>
#include <string>
#include "optimize.h"
#include "netlist.h"
#include "cut.h"
#include "label.h"


namespace SYN {
  
  //typedef std::unordered_map<Pin*,Label*> PinLabelMap;

  class Decomp : public Opt {
  public:
    typedef std::unordered_map<Pin*,Label*> PinLabelMap;
    // pair<delay,pair<inpin,inpin_label>>
    // put items in a set in a delay-increasing order
    //std::multiset<std::pair<int,std::pair<Pin*,Label*>>,DelayPinLabelComp> delay_pin_label_set; 

    typedef struct {
      bool operator() (const std::pair<int,std::pair<Pin*,Label*>>& p1, const std::pair<int,std::pair<Pin*,Label*>>& p2) {
        if (p1.first < p2.first) return true;
        return false;
      }
    } DelayPinLabelComp;

    typedef std::multiset<std::pair<int,std::pair<Pin*,Label*>>,DelayPinLabelComp> DPLSET;

    //a vector of (bin and its inpins)
    typedef std::vector<std::pair<Label*,std::vector<Pin*>>> LABEL_INPIN;



    Decomp(Model* top) : Opt(top) {
      decomp_engine_ = this;
    }
    //need to delete the PinLabelMap
    ~Decomp() {
      for(PinLabelMap::iterator i=pl_map_.begin(); i!=pl_map_.end();++i){
        Label* label = i->second;
        if(label) { 
          delete label;
        }
      }
    }
    
    void techDecomp(int in_and, int in_or, int balance);
    void enuDecomp(int lutsize, int delaylimit);

    //void decompAndOr(Gate* gate, Model* top_model);
    
    static Decomp* getDecomp() {
      return decomp_engine_;
    }
    

    PinLabelMap* getPLMap() {return &pl_map_;}
    
  protected:

    
    std::vector<Gate*> dmigAndDecomp(Gate* gate, Model* top_model, int and_lim, bool connect_net);
    std::vector<Gate*> dmigOrDecomp(Gate* gate, Model* top_model, int or_lim, bool connect_net);
    std::vector<Gate*> balanceAndDecomp(Gate* gate, Model* top_model, int and_lim);
    std::vector<Gate*> balanceOrDecomp(Gate* gate, Model* top_model, int or_lim);
    void updateGateLevel(Gate* gate);


    // 4 funcs used for blance decomp 
    void getAndInpinPol(Gate* gate, size_t offset, size_t gate_size, 
			const Cube& gate_cube, std::vector<Pin*>& inpins, std::vector<Cube::VAL>& pals);
    void getAndInpinPol(const std::vector<Pin*> level_pins, const size_t offset, const size_t gate_size, 
			std::vector<Pin*>& inpins, std::vector<Cube::VAL>& pals );
    
    void getOrInpinPol(Gate* gate, size_t offset, size_t gate_size, std::vector<Pin*>& inpins, 
		       std::vector<Cube::VAL>& pals);
    void getOrInpinPol(const std::vector<Pin*> level_pins, const size_t offset, const size_t gate_size, 
		       std::vector<Pin*>& inpins, std::vector<Cube::VAL>& pals );
    
    // comparator for dmig struct
    typedef struct {
      bool operator() (const std::pair<Pin*,int>& p1, const std::pair<Pin*,int>& p2) {
        if (p1.first->getLevel() < p2.first->getLevel()) return true;
        return false;
      }
    } PairComp;

    // func for enu_decomp
    std::vector<Gate*> labelDecompGate(Gate* gate, Model* top_model, int delay_limit, int lut_size);
    void labelGate(Gate* gate,int delay_limit,int lut_size);
    void labelPI(Pin* inpin);

    void printPinPLMapInfo(Pin* pin);

    void cleanPICut(const std::vector<Pin*> inpins);
    void addInputsCuts(std::vector<Pin*> inpins);

    bool getNextCombination(const std::vector<int>& upper_bound, std::vector<int>& current_comb);
    void printEachCombination(const std::vector<int>& current_comb);
    std::vector<int> getDelayFromIndex(const std::vector<Label*> inpin_labels, const std::vector<int>& current_comb);

    bool genEnuCutLabel(const std::vector<Label*>& inpin_labels, int lut_size, int delay_limit, Label* label);

    Decomp::DPLSET enuDelayComb(const std::vector<Label*>& inpin_labels, const std::vector<Pin*>& inpins, int lut_size, int delay_limit);

    int getDPLBinPackDelay(DPLSET& dpl_set, int lut_size);
    void genBinPackLabels(const LABEL_INPIN& inpin_labels,const std::vector<Pin*> inputs_vec, int lut_size, int delay_limit, LABEL_INPIN& return_labels, std::vector<Pin*>& new_pin_records);

    bool checkLabelsWorstDelayIncOrder(const LABEL_INPIN& labels_ptr_vec);
    void addBinsSelfCuts(const LABEL_INPIN& bin_label_ptr_vec, std::vector<Pin*>& new_pin_records);
    void binPositionReorder(LABEL_INPIN& bins_ptr_vec,std::vector<Pin*>& bins_opins);
    void printDPLSetDelay(const DPLSET& dpl_set);

    void decompGateEnuCut(const DPLSET& opt_dpl,Gate* gate,Model* top_model,int lut_size,int delay_limit);
    
    std::vector<Cube::VAL> getGateInpinPhase(Gate* gate, const std::vector<Pin*> gate_inpin);

    PinLabelMap pl_map_;
    static Decomp* decomp_engine_;
    

    void printUpperBound(std::vector<int> u_b){
      msg(2,"Upper bound is: ");
      for (int i = 0; i < u_b.size(); ++i) {
        msg(2,"%d ",u_b[i]);
      }
      msg(2,"\n");
    }

    void printIndex(std::vector<int> id){
      msg(2,"Current Index is: ");
      for (int i = 0; i < id.size(); ++i) {
        msg(2,"%d ",id[i]);
      }
      msg(2,"\n");
    }

    void printIndex(std::vector<size_t> id){
      msg(2,"Current Index is: ");
      for (int i = 0; i < id.size(); ++i) {
        msg(2,"%d ",id[i]);
      }
      msg(2,"\n");
    }

    void printCurrentDelay(std::vector<int> id){
      msg(2,"Current Delay is: ");
      for (int i = 0; i < id.size(); ++i) {
        msg(2,"%d ",id[i]);
      }
      msg(2,"\n");
    }

    // functions for v2 enumerate decomposition
    /* function 1: enuCutComb: enumerate cut combination of all the inpins to 
     * get the optimal cut comb that can form the optimal structure
     * Outputs: index of cut for each inpin (same order as inpins)
     * Inputs: inpin_labels: vec of labels of inpins (same order as inpins)
     *         inpins: vec of Pin* which are the inpins of a gate
     *         lut_size: lut_size of the FPGA
     *         delay_limit: max delay drift from opt delay
     */
    //std::vector<size_t> enuCutComb(const std::vector<Label*>& inpin_labels, const std::vector<Pin*>& inpins, int lut_size, int delay_limit);
    std::vector<std::pair<size_t,Cut*>> enuCutComb(const std::vector<Label*>& inpin_labels, const std::vector<Pin*>& inpins, int lut_size, int delay_limit);
    /* function 2: getCutBinPackDelay
     * Given a vector of cuts, there delays and the lut size, calculate the delay after bin packing
     * Outputs: The final delay of the gate when given this inpin cut combination
     * Inputs: std::vector<std::pair<size_t, Cut*>> delay_cut_vec: a vec of delay_cut pair of the gate
     *         std::vector<Pin*> cut_inputs; the source of every input, same order with the delay_cuts_vec
     *         int lut_size: the size of the lut
     */

    int getCutBinPackDelay(const std::vector<std::pair<size_t,Cut*>>& delay_cuts_vec, const std::vector<Pin*>& cut_inputs, int lut_size);

    /* function 3: decompGateEnuCutV2 (for cut combination enu)
     * Given a optimal cut combination, it decompose the gate accordingly
     * Outputs: a chain of gates which is the decomposition results of the current gate 
     * Inputs: std::vector<std::pair<size_t, Cut*>> opt_delay_cut_vec: a optimal vec of delay_cut pair of the gate
     *         std::vector<Pin*> cut_inputs; the source of every input, same order with the delay_cuts_vec
     *         Gate *gate: the current gate being decomposed
     *         Model *model: the model that the gate belongs to
     *         int lut_size: the size of the lut
     *         int delay_limit: the maximum delay for a pin's label can have
     */

    std::vector<Gate*> decompGateEnuCutV2(const std::vector<std::pair<size_t,Cut*>>& opt_delay_cuts_vec, const std::vector<Pin*>& cut_inputs, Gate* gate,Model* top_model,int lut_size,int delay_limit);

    /* function 4: get the next combination with command
     * we are try to enumerate all the combinations that less than minimal sum +k
     * strobe = 0: like the normal clock counting
     * strobe = 1: (if lowest bit != 0) lowest bit goes to 0, add 1 to the second lowest bit
     * strobe = 2: (if lowest bit == 0) define overflow bit: the largest and highest bit of all bits
     *              overflow bit = 0; add 1 to the higher bit just before the overflow bit
     * So we will determine the strobe based on the current_comb and trigger
     *
     * inputs: 1. upper_bound: the limit for n-ary 
     *         2. current_comb: current indexing/combination
     *         3. trigger: weather the current_comb triggers an unsatisfied situation
     *            (i.e. the current_comb delay sum > index[1...1] delay sum + k)
     * output: overflow signal
     */
    bool getNextCombinationV2(const std::vector<int>& upper_bound, std::vector<int>& current_comb, bool trigger);



    /* function 5: bin pack one level
     * we want to pack a vector of cuts into several bins
     * inputs: inpin_labels: for each gate inpin/bin, we have a Label_inpin struct, it is a labels along with the 
     *                        inpin/bin's inpins
     *         inputs_vec: the outpin of the inpin/bin of this level
     *         lut_size: the size of the lut/bin
     * outputs:return_labels: the newly generated bins(label+the inpins of the label)
     *         new_pin_records: the outpin of the newly generated bins
     */
    void genBinPackLabelsV2(const LABEL_INPIN& inpin_labels,const std::vector<Pin*> inputs_vec, int lut_size, int delay_limit, LABEL_INPIN& return_labels, std::vector<Pin*>& new_pin_records);

    /* function 6: add the bins self cut and delete the existing cut
     * inputs: bin_label_ptr_vec: the bins of a level
     * outputs: new_pin_records: the outpin of the bins
     */
    void addBinsSelfCutsV2(const LABEL_INPIN& bin_label_ptr_vec, std::vector<Pin*>& new_pin_records);

    void newLabelDecompGate(Gate* gate, Model* top_model, int delay_limit, int lut_size);

  }; //class Decomp


} //namespace SYN

#endif //_SYN_DECOMP_H_

