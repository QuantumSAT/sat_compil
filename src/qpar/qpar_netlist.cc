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

ParNetlist* ParNetlist::getOrCreate(SYN::Model* model) {
  if (_self) return _self;

  _self = new ParNetlist(model);
  return _self;
}

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
  QASSERT(_syn_netlist);

  std::vector<SYN::Gate*> gates;
  std::unordered_map<SYN::Gate*, ParElement*> gate_to_par_element;
  std::unordered_map<SYN::Net*, ParWire*> net_to_par_wire;
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
    QASSERT(gate_to_par_element.count(syn_gate));
    ParElement* element = gate_to_par_element.at(syn_gate);
    PIN_ITER pin_iter = syn_gate->begin();
    for (; pin_iter != syn_gate->end(); ++pin_iter) {
      SYN::Pin* pin = *pin_iter;
      QASSERT(!pin->isBidir());
      ParWire* parwire = NULL;

      SYN::Net* net = pin->net();
      if (net_to_par_wire.count(net)) {
        parwire = net_to_par_wire.at(net);
      } else {
        if (net == NULL || net->isDummy()) {
          qlog.speak("Placement", "Gate %s Pin %s is dangling",
              pin->getGate()->getName().c_str(),
              net->name().c_str());
          continue;
        }
        parwire = new ParWire(net);
        _wires.insert(parwire);
        net_to_par_wire.insert(std::make_pair(net, parwire));
      }
      element->addWire(parwire);
    }
    std::unordered_map<SYN::Net*, ParWire*>::iterator w_iter = net_to_par_wire.begin();
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

void ParElement::addWire(ParWire* wire) {
  _wires.push_back(wire);
  wire->addElement(this);
}

unsigned int ParWire::_wire_index_counter = 0;
ParWire::ParWire(SYN::Net* wire) : 
_net(wire) {
  _wire_index = _wire_index_counter;
  ++_wire_index_counter;
}

ParWire::~ParWire() {
  for (unsigned i = 0; i < _targets.size(); ++i) {
    delete _targets[i];
  }
}

std::vector<ParWireTarget*>& ParWire::buildWireTarget(
    const std::unordered_map<SYN::Gate*, ParElement*>& gate_to_par_element) {
  _targets.clear();
  SYN::Pin* source = _net->uniqSource();
  QASSERT(source);
  std::vector<SYN::Pin*> sink;
  QASSERT(_net->numSink());
  for (unsigned i = 0; i <  _net->numSink(); ++i)
    sink.push_back(_net->getSink(i));
  
  bool src_is_model_pin = source->isModelPin();


  if (src_is_model_pin) {
    SYN::Pin* gate1 = NULL;
    for (unsigned i = 0; i < sink.size(); ++i) {
      if (sink[i]->isGatePin()) {
        gate1 = sink[i];
        break;
      }
    }

    if (gate1) {

      for (unsigned i = 0; i < sink.size(); ++i) {
        SYN::Pin* sk = sink[i];
        if (sk == gate1) {
          ParElement* src_ele = gate_to_par_element.at(gate1->getGate());
          ParWireTarget* target = new ParWireTarget(src_ele, src_ele, source, sk);
          target->setDontRoute(true);
          _targets.push_back(target);
          continue;
        }

        if (sk->isModelPin()) {
          ParElement* src_ele = gate_to_par_element.at(gate1->getGate());
          ParWireTarget* target = new ParWireTarget(NULL, NULL, source, sk);
          target->setDontRoute(true);
          _targets.push_back(target);
        } else {
          ParElement* src_ele = gate_to_par_element.at(gate1->getGate());
          ParElement* tgt_ele = gate_to_par_element.at(sk->getGate());
          ParWireTarget* target = new ParWireTarget(src_ele, tgt_ele, source, sk);
          _targets.push_back(target);
        }
      }
    } else {
      for (unsigned i = 0; i < sink.size(); ++i) {
        SYN::Pin* sk = sink[i];
        ParWireTarget* target = new ParWireTarget(NULL, NULL, source, sk);
        target->setDontRoute(true);
        _targets.push_back(target);
      }
    }
  } else {
    for (unsigned i = 0; i < sink.size(); ++i) {
      SYN::Pin* sk = sink[i];
      ParElement* src_ele = gate_to_par_element.at(source->getGate());
      if (sk->isModelPin()) {
        ParWireTarget* target = new ParWireTarget(src_ele, NULL, source, sk);
        target->setDontRoute(true);
        _targets.push_back(target);
      } else {
        ParElement* tgt_ele = gate_to_par_element.at(sk->getGate());
        ParWireTarget* target = new ParWireTarget(src_ele, tgt_ele, source, sk);
        _targets.push_back(target);
      }
    }

  }

  return _targets;
}



