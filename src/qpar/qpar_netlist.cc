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
#include "qpar/qpar_target.hh"
#include "qpar/qpar_utils.hh"

#include "syn/netlist.h"
#include "utils/qlog.hh"

#include <vector>
#include <algorithm>
#include <unordered_set>

bool ParWireCmp::operator()(const ParWire* wire1, const ParWire* wire2) const {
  return wire1->getUniqId() < wire2->getUniqId();
}

bool ParWireTargetCmp::operator()(const ParWireTarget* tgt1, const ParWireTarget* tgt2) const {
  return tgt1->getUniqId() < tgt2->getUniqId();
}

bool ParElementCmp::operator()(const ParElement* ele1, const ParElement* ele2) const {
  return ele1->getUniqId() < ele2->getUniqId();
}

void ParElement::setGrid(ParGrid* grid) {
  _grid.setStatus(grid);
}

void ParElement::save() { _grid.saveStatus(); }

void ParElement::restore() { 
  _grid.restoreStatus(); 
}

COORD ParElement::getX() const {
  QASSERT(_grid.getStatus());
  return _grid.getStatus()->getLoc().getLocX();
}


COORD ParElement::getY() const {
  QASSERT(_grid.getStatus());
  return _grid.getStatus()->getLoc().getLocY();
}

ParGrid* ParElement::getCurrentGrid() const {
  return _grid.getStatus();
}

//std::pair<COORD,COORD> ParElement::getCurrentPlacement() const {
//  return std::make_pair(getX(),getY());
//}

ParNetlist* ParNetlist::_top_netlist = NULL;

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
          qlog.speak("Design", "Gate %s Pin %s is dangling",
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
      std::vector<ParWireTarget*> targets = wire->buildWireTarget(gate_to_par_element);
      _all_targets.insert(_all_targets.end(), targets.begin(), targets.end());
    }

  }

  qlog.speak("Design", "%u wires and %u elements has been constructed",
      (unsigned)net_to_par_wire.size(),
      (unsigned)gate_to_par_element.size()
      );

}

unsigned int ParElement::_element_index_counter = 0;
ParElement::ParElement(SYN::Gate* gate) : 
_gate(gate),
_sink(NULL),
_movable(true) {
  _element_index = _element_index_counter;
  ++_element_index_counter;
}

void ParElement::addWire(ParWire* wire) {
  _wires.push_back(wire);
  wire->addElement(this);
}

void ParElement::updatePlacement() {
  QASSERT(_grid.getStatus());
  _x = (_grid.getStatus())->getLoc().getLocX();
  _y = (_grid.getStatus())->getLoc().getLocY();

}

std::string ParElement::getName() const {
  return _gate->getName();
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

void ParWire::recomputeBoundingBox() {

  int xl = std::numeric_limits<int>::max();
  int xr = -1;
  int yt = std::numeric_limits<int>::max();
  int yb = -1;

  int xle = 0;
  int xre = 0;
  int yte = 0;
  int ybe = 0;

  ELE_ITER ele_iter = _elements.begin();
  for (; ele_iter != _elements.end(); ++ele_iter) {
    ParElement* ele = *ele_iter;
    COORD coordX = ele->getX();
    COORD coordY = ele->getY();

    if (coordX < xl) {
      xl = coordX;
      xle = 1;
    } else if (coordX == xl) {
      ++xle;
    }

    if (coordX > xr) {
      xr = coordX;
      xre = 1;
    } else if (coordX = xr) {
      ++xre;
    }

    if (coordY > yb) {
      yb = coordY;
      ybe = 1;
    } else if (coordY == yb) {
      ++ybe;
    }

    if (coordY < yt) {
      yt = coordY;
      yte = 1;
    } else if (coordY == yt) {
      ++yte;
    }

  }

  Box box1(xl, xr, yt, yb);
  Box box2(xle,xre,yte,ybe);
  _bounding_box.getStatus().setBoundingBox(box1);
  _bounding_box.getStatus().setEdgeBox(box2);

}

void ParWire::initializeBoundingBox() {
  recomputeBoundingBox();
  _bounding_box.saveStatus();
}

void ParWire::saveCost() {
  _cost.saveStatus();
}

double ParWire::getCurrentCost() const {
  return _cost.getStatus();
}

double ParWire::getSavedCost() const {
  return _cost.getSavedStatus();
}

void ParWire::setCost(double val) {
  _cost.setStatus(val);
}

void ParWire::sanityCheck() {
  int xl = std::numeric_limits<int>::max();
  int xr = -1;
  int yt = std::numeric_limits<int>::max();
  int yb = -1;

  int xle = 0;
  int xre = 0;
  int yte = 0;
  int ybe = 0;

  ELE_ITER ele_iter = _elements.begin();
  for (; ele_iter != _elements.end(); ++ele_iter) {
    ParElement* ele = *ele_iter;
    COORD coordX = ele->getX();
    COORD coordY = ele->getY();

    if (coordX < xl) {
      xl = coordX;
      xle = 1;
    } else if (coordX == xl) {
      ++xle;
    }

    if (coordX > xr) {
      xr = coordX;
      xre = 1;
    } else if (coordX = xr) {
      ++xre;
    }

    if (coordY > yb) {
      yb = coordY;
      ybe = 1;
    } else if (coordY == yb) {
      ++ybe;
    }

    if (coordY < yt) {
      yt = coordY;
      yte = 1;
    } else if (coordY == yt) {
      ++yte;
    }

  }

  Box box = getCurrentBoundingBox().getBoundBox();
  if (box.xr() != xr)
    qlog.speak("Place", "Sanity Checking Net: BoundingBox xr %u is not equal to incr %u", box.xr(), xr);
  if (box.xl() != xl)
    qlog.speak("Place", "Sanity Checking Net: BoundingBox xl %u is not equal to incr %u", box.xl(), xl);
  if (box.yb() != yb)
    qlog.speak("Place", "Sanity Checking Net: BoundingBox yb %u is not equal to incr %u", box.yb(), yb);
  if (box.yt() != yt)
    qlog.speak("Place", "Sanity Checking Net: BoundingBox yt %u is not equal to incr %u", box.yt(), yt);


}

void ParWire::restore() {
  _bounding_box.restoreStatus();
  _cost.restoreStatus();
}

BoundingBox ParWire::getCurrentBoundingBox() const {
  return _bounding_box.getStatus();
}

void ParWire::saveBoundingBox() {
  _bounding_box.saveStatus();
}

void ParWire::updateBoundingBox(COORD from_x, COORD from_y, COORD to_x, COORD to_y) {
  Box& bbox = _bounding_box.getStatus().getBoundBox();
  Box& ebox = _bounding_box.getStatus().getEdgeBox();
  bool recal = false ;

  if (to_x < bbox.xl()) {
    bbox.set_xl(to_x) ;
    ebox.set_xl(1) ;
  } else if (to_x == bbox.xl()) {
    ebox.incr_xl() ;
    if (from_x == bbox.xl())
      ebox.decr_xl() ;
  } else if (from_x == bbox.xl()) {
    if (ebox.xl() > 1)
      ebox.decr_xl() ;
    else
      recal = recal || true ;
  }

  if (to_x > bbox.xr()) {
    bbox.set_xr(to_x) ;
    ebox.set_xr(1) ;
  } else if (to_x == bbox.xr()) {
    ebox.incr_xr() ;
    if (from_x == bbox.xr())
      ebox.decr_xr() ;
  } else if (from_x == bbox.xr()) {
    if (ebox.xr() > 1)
      ebox.decr_xr() ;
    else
      recal = recal || true ;
  }

  if (to_y < bbox.yt()) {
    bbox.set_yt(to_y) ;
    ebox.set_yt(1) ;
  } else if (to_y == bbox.yt()) {
    ebox.incr_yt() ;
    if (from_y == bbox.yt())
      ebox.decr_yt() ;
  } else if (from_y == bbox.yt()) {
    if (ebox.yt() > 1)
      ebox.decr_yt() ;
    else
      recal = recal || true ;
  }


  if (to_y > bbox.yb()) {
    bbox.set_yb(to_y) ;
    ebox.set_yb(1) ;
  } else if (to_y == bbox.yb()) {
    ebox.incr_yb() ;
    if (from_y == bbox.yb())
      ebox.decr_yb() ;
  } else if (from_y == bbox.yb()) {
    if (ebox.yb() > 1)
      ebox.decr_yb() ;
    else
      recal = recal || true ;
  }

  if (recal)
    recomputeBoundingBox();

}

unsigned ParWireTarget::_wire_target_counter = 0;

ParWireTarget::ParWireTarget(ParElement* source, ParElement* dest,
    SYN::Pin* src_pin, SYN::Pin* tgt_pin) :
  _source(source),
  _target(dest),
  _src_pin(src_pin),
  _tgt_pin(tgt_pin),
  _dontRoute(false) {
    _target_index = _wire_target_counter;
    ++_wire_target_counter;
  }






