/****************************************************************************
 * Copyright (C) 2017 by Juexiao Su                                         *
 *                                                                          *
 * This file is part of QSat.                                               *
 *                                                                          *
 *   QSat is free software: you can redistribute it and/or modify it        *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   QSat is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with QSat.  If not, see <http://www.gnu.org/licenses/>.  *
 ****************************************************************************/


#include "qpar/qpar_netlist.hh"
#include "syn/netlist.h"
#include "utils/qlog.hh"

#include <vector>
#include <unordered_set>

ParNetlist* ParNetlist::_self = NULL;

ParNetlist::ParNetlist(SYN::Model* model) :
  _syn_netlist(model) {
    buildParNetlist();
}

ParNetlist::~ParNetlist() {
  ParElementSet::iterator e_iter = _elements.begin();
  for (; e_iter != _elements.end(); ++e_iter)
    delete *e_iter;
  _elements.clear();

  ParWireSet::iterator w_iter = _wires.begin();
  for (; w_iter != _wires.end(); ++w_iter)
    delete *w_iter;
  _wires.clear();
}


void ParNetlist::buildParNetlist() {
  QASSERT(model);

  std::vector<SYN::Gate*> gates;
  std::unordered_set<SYN::Gate*, ParElement*> gate_to_par_element;
  std::unordered_set<SYN::Net*, PairWire*> net_to_par_wire;
  gates = _syn_netlist->getModelGates();

  for (size_t i = 0; i < gates.size(); ++i) {
    SYN::Gate* syn_gate = gates[i];
    QASSERT(syn_gate);
    ParElement* element = new ParElement(syn_gate);
    gate_to_par_element.insert(std::make_pair(syn_gate, element));
    _elements.insert(element);
  }

  typedef std::vector<SYN::Pin*>::iterator PIN_ITER;
  for (size_t i = 0; i < gates.size(); ++i) {
    SYN::Gate* syn_gate = gates[i];
    QSSERT(gate_to_par_element.count(syn_gate));
    ParElement* element = gate_to_par_element.at(syn_gate);
    PIN_ITER pin_iter = gate->begin();
    for (; pin_iter != gate->end(); ++pin_iter) {
      SYN::Pin* pin = *pin_iter;
      QASSERT(!pin->isBidir());
      PairWire* parwire = NULL;

      SYN::Net* net = pin->net();
      if (net_to_pair_wire.count(net)) {
        parwire = net_to_par_wire.at(net);
      } else {
        if (net == NULL || net->isDummy()) {
          qlog.speak("Placement", "Gate %s Pin %s is dangling",
              pin->getGate()->getName.c_str(),
              net->name().c_str());
          continue;
        }
        parwire = new ParWire(net);
        _wires.insert(parwire);
        net_to_pair_wire.insert(std::make_pair(net, parwire));
      }
      element->addWire(parwire);
    }
    std::unordered_set<SYN::Net*, PairWire*>::iterator w_iter = net_to_par_wire.begin();
    for (; w_iter != net_to_par_wire.end(); ++w_iter) {
      ParWire* wire = w_iter->second;
      wire->buildWireTarget(gate_to_par_element);
    }

  }

}

unsigned int ParElement::_element_index_counter = 0;
ParElement::ParElement(SYN::Gate* gate) : 
_gate(gate) {
  _element_index = _element_index_counter;
  ++_element_index_counter;
}

unsigned int ParWire::_wire_index_counter = 0;
ParWire::ParWire(SYN::Net* wire) : 
_net(wire) {
  _wire_index = _wire_index_counter;
  ++_wire_index_couter;
}

std::vector<ParWireTarget*>& ParWire::buildWireTarget() {
  _target.clear();
  SYN::Pin* source = _net->uniqSource();
  QASSERT(source);
  std::vector<SYN::Pin*> sink;
  QASSERT(_net->getNumSink());
  for (unsigned i = 0; i <  _net->getNumSink(); ++i)
    sink.push_back(_net->getSink(i));
  
  bool src_is_model_pin = source->isModelPin();


  if (src_is_model_pin) {
    if (_net->getNumSink()) return _target;
  } else {

  }


   
}



