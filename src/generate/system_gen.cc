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

#include "generate/system_gen.hh"

#include "syn/netlist.h"

#include "hw_target/hw_object.hh"

#include "qpar/qpar_netlist.hh"
#include "qpar/qpar_target.hh"
#include "qpar/qpar_routing_graph.hh"
#include "qpar/qpar_route.hh"

#include "utils/qlog.hh"

#include <fstream>

CellGen::CellGen(ParGrid* grid) : 
_grid(grid) {
}

DeviceGen::~DeviceGen() {
  LocToCellGen::iterator c_iter = _loc_to_cellgen.begin();
  for (; c_iter != _loc_to_cellgen.end(); ++c_iter) 
    delete c_iter->second;
  _loc_to_cellgen.clear();

  for (size_t i = 0; i < _v_interactions.size(); ++i)
    delete _v_interactions[i];

  _v_interactions.clear();

}

void CellGen::assignPin(SYN::Pin* pin, COORD loc) {
  if (_pin_to_loc.count(pin))
    QASSERT(_pin_to_loc.at(pin) == loc);
  else
    _pin_to_loc.insert(std::make_pair(pin, loc));
}

void CellGen::configSpin(COORD local, double val) {
  ParElement* element = _grid->getCurrentElement();
  COORD x_coord = element->getX();
  COORD y_coord = element->getY();

  HW_Loc loc_phy1(x_coord, y_coord, local);
  HW_Loc loc_phy2(x_coord, y_coord, local + 4);

  HW_Loc loc_chain(loc_phy1.getGlobalIndex(), loc_phy2.getGlobalIndex());

  ConfigElement q1;
  q1.loc = loc_phy1;
  q1.value = val / 2;

  ConfigElement q2;
  q2.loc = loc_phy2;
  q2.value = val / 2;

  _qubit_configs.insert(std::make_pair(loc_phy1.getGlobalIndex(), q1)); 
  _qubit_configs.insert(std::make_pair(loc_phy2.getGlobalIndex(), q2)); 

  ConfigElement chain;
  chain.loc = loc_chain;
  chain.value = -1.0;

  _inter_configs.insert(std::make_pair(std::make_pair(loc_phy1.getGlobalIndex(),
                                       loc_phy2.getGlobalIndex()),
                                       chain));

}

void CellGen::configInteraction(COORD local1, COORD local2, double val) {
  ParElement* element = _grid->getCurrentElement();
  COORD x_coord = element->getX();
  COORD y_coord = element->getY();
  HW_Loc loc_phy1_1(x_coord, y_coord, local1);
  HW_Loc loc_phy1_2(x_coord, y_coord, local1 + 4);

  HW_Loc loc_phy2_1(x_coord, y_coord, local2);
  HW_Loc loc_phy2_2(x_coord, y_coord, local2 + 4);

  HW_Loc inter_1(loc_phy1_1.getGlobalIndex(), loc_phy2_2.getGlobalIndex());
  HW_Loc inter_2(loc_phy1_2.getGlobalIndex(), loc_phy2_1.getGlobalIndex());

  ConfigElement inter1;
  inter1.loc = inter_1;
  inter1.value = val / 2;

  ConfigElement inter2;
  inter2.loc = inter_2;
  inter2.value = val / 2;

  _inter_configs.insert(std::make_pair(std::make_pair(loc_phy1_1.getGlobalIndex(), loc_phy2_2.getGlobalIndex()), inter1));
  _inter_configs.insert(std::make_pair(std::make_pair(loc_phy1_2.getGlobalIndex(), loc_phy2_1.getGlobalIndex()), inter2));

}

void CellGen::assignPin(SYN::Pin* pin) {

  std::set<COORD> locs;
  for (COORD i = 0; i < 4; ++i)
    locs.insert(i);


  std::unordered_map<SYN::Pin*, COORD>::iterator pin_iter = _pin_to_loc.begin();
  for (; pin_iter != _pin_to_loc.end(); ++pin_iter) {
    QASSERT(locs.count(pin_iter->second));
    locs.erase(pin_iter->second);
  }

  QASSERT(locs.size());
  _pin_to_loc.insert(std::make_pair(pin, *(locs.begin())));

}

void CellGen::generateConfig(DeviceGen* device) {
  ParElement* element = _grid->getCurrentElement();

  QASSERT(element);
  SYN::Gate* gate = element->getSynGate();
  SYN::Pin* syn_pin = element->getPin();

  if (gate) {
    QASSERT(gate->getNumIn() <= 2);
    switch(gate->getDwaveType()) {
      case SYN::Gate::OR:
        {
          SYN::Pin* pin1 = gate->getInpin(0);
          SYN::Pin* pin2 = gate->getInpin(1);
          SYN::Pin* opin = gate->getOpin();
          SYN::Gate::PHASE in1_phase = gate->getPinPhase(pin1);
          SYN::Gate::PHASE in2_phase = gate->getPinPhase(pin2);
          QASSERT(gate->getNumOut() == 1);
          QASSERT(_pin_to_loc.count(pin1) && _pin_to_loc.count(pin2));
          QASSERT(_pin_to_loc.count(opin));
          COORD pos1 = _pin_to_loc.at(pin1);
          COORD pos2 = _pin_to_loc.at(pin2);
          COORD pos3 = _pin_to_loc.at(opin);

          if (in1_phase == SYN::Gate::POS_UNATE && in2_phase == SYN::Gate::POS_UNATE) {
            configSpin(pos1, 0.5);
            configSpin(pos2, 0.5);
            configSpin(pos3, -1.0);
            configInteraction(pos1, pos2, 0.5);
            configInteraction(pos1, pos3, -1.0);
            configInteraction(pos2, pos3, -1.0);
          } else if (in1_phase == SYN::Gate::POS_UNATE && in2_phase == SYN::Gate::NEG_UNATE) {
            configSpin(pos1, 0.5);
            configSpin(pos2, -0.5);
            configSpin(pos3, -1.0);
            configInteraction(pos1, pos2, -0.5);
            configInteraction(pos1, pos3, -1.0);
            configInteraction(pos2, pos3, 1.0);
          } else if (in1_phase == SYN::Gate::NEG_UNATE && in2_phase == SYN::Gate::POS_UNATE) {
            configSpin(pos1, -0.5);
            configSpin(pos2, 0.5);
            configSpin(pos3, -1.0);
            configInteraction(pos1, pos2, -0.5);
            configInteraction(pos1, pos3, 1.0);
            configInteraction(pos2, pos3, -1.0);
          } else if (in1_phase == SYN::Gate::NEG_UNATE && in2_phase == SYN::Gate::NEG_UNATE) {
            configSpin(pos1, -0.5);
            configSpin(pos2, -0.5);
            configSpin(pos3, -1.0);
            configInteraction(pos1, pos2, 0.5);
            configInteraction(pos1, pos3, 1.0);
            configInteraction(pos2, pos3, 1.0);
          } else {
            assert(0);
          }
          break;
        }
      case SYN::Gate::AND:
        {
          SYN::Pin* pin1 = gate->getInpin(0);
          SYN::Pin* pin2 = gate->getInpin(1);
          SYN::Pin* opin = gate->getOpin();
          SYN::Gate::PHASE in1_phase = gate->getPinPhase(pin1);
          SYN::Gate::PHASE in2_phase = gate->getPinPhase(pin2);
          QASSERT(gate->getNumOut() == 1);
          QASSERT(_pin_to_loc.count(pin1) && _pin_to_loc.count(pin2));
          QASSERT(_pin_to_loc.count(opin));
          COORD pos1 = _pin_to_loc.at(pin1);
          COORD pos2 = _pin_to_loc.at(pin2);
          COORD pos3 = _pin_to_loc.at(opin);
          if (in1_phase == SYN::Gate::POS_UNATE && in2_phase == SYN::Gate::POS_UNATE) {
            configSpin(pos1, -0.5);
            configSpin(pos2, -0.5);
            configSpin(pos3, 1.0);
            configInteraction(pos1, pos2, 0.5);
            configInteraction(pos1, pos3, -1.0);
            configInteraction(pos2, pos3, -1.0);
          } else if (in1_phase == SYN::Gate::POS_UNATE && in2_phase == SYN::Gate::NEG_UNATE) {
            configSpin(pos1, -0.5);
            configSpin(pos2, 0.5);
            configSpin(pos3, 1.0);
            configInteraction(pos1, pos2, -0.5);
            configInteraction(pos1, pos3, -1.0);
            configInteraction(pos2, pos3, 1.0);
          } else if (in1_phase == SYN::Gate::NEG_UNATE && in2_phase == SYN::Gate::POS_UNATE) {
            configSpin(pos1, 0.5);
            configSpin(pos2, -0.5);
            configSpin(pos3, 1.0);
            configInteraction(pos1, pos2, -0.5);
            configInteraction(pos1, pos3, 1.0);
            configInteraction(pos2, pos3, -1.0);
          } else if (in1_phase == SYN::Gate::NEG_UNATE && in2_phase == SYN::Gate::NEG_UNATE) {
            configSpin(pos1, 0.5);
            configSpin(pos2, 0.5);
            configSpin(pos3, 1.0);
            configInteraction(pos1, pos2, 0.5);
            configInteraction(pos1, pos3, 1.0);
            configInteraction(pos2, pos3, 1.0);
          } else {
            assert(0);
          }
          break;
        }
      case SYN::Gate::BUF:
        {
          QASSERT(0);
        }
      default:QASSERT(0);
    }
  } else if (syn_pin) {
    QASSERT(_pin_to_loc.count(syn_pin));
    COORD pos = _pin_to_loc.at(syn_pin);
    COORD x_coord = element->getX();
    COORD y_coord = element->getY();
    HW_Loc loc_phy1_1(x_coord, y_coord, pos);
    HW_Loc loc_phy1_2(x_coord, y_coord, pos + 4);

    HW_Loc inter_1(loc_phy1_1.getGlobalIndex(), loc_phy1_2.getGlobalIndex());
    ConfigElement inter;
    inter.loc = inter_1;
    inter.value = -1.0;
    _inter_configs.insert(std::make_pair(std::make_pair(loc_phy1_1.getGlobalIndex(), loc_phy1_2.getGlobalIndex()), inter));

  }

  QubitConfigs::iterator q_iter = _qubit_configs.begin();
  for (; q_iter != _qubit_configs.end(); ++q_iter) {
    device->addQubitConfig(q_iter->first, q_iter->second.value);
  }

  InteractionConfigs::iterator i_iter = _inter_configs.begin();
  for (; i_iter != _inter_configs.end(); ++i_iter) {
    device->addInteractionConfig(i_iter->first.first, i_iter->first.second, i_iter->second.value);
  }

}

void DeviceGen::dumpDwaveConfiguration(std::string filename) {
  qlog.speak("Generate", "Dump D-Wave configuration to %s", filename.c_str());
  std::ofstream outfile;
  outfile.open(filename.c_str());
  size_t line_num = _qubits.size() + _interactions.size();
  outfile << 2048 << " " << line_num << std::endl;
  
  QubitConfigs::iterator q_iter = _qubits.begin();
  for (; q_iter != _qubits.end(); ++q_iter) {
    outfile << q_iter->first << " " << q_iter->first << " " << q_iter->second.value << std::endl;
  }

  InteractionConfigs::iterator i_iter = _interactions.begin();
  for (; i_iter != _interactions.end(); ++i_iter) {
    outfile << i_iter->first.first << " " << i_iter->first.second << " " << i_iter->second.value << std::endl;
  }
}

void DeviceGen::addQubitConfig(COORD x, double val) {
  if (_qubits.count(x)) {
    QASSERT(_qubits.at(x).value == val);
  } else {
    ConfigElement con;
    COORD x_index = HW_Loc::globalIndexToX(x);
    COORD y_index = HW_Loc::globalIndexToY(x);
    COORD local = HW_Loc::globalIndexToLocalIndex(x);
    HW_Loc loc(x_index, y_index, local);
    con.loc = loc;
    con.value = val;
    _qubits.insert(std::make_pair(x, con));
  }

}

void DeviceGen::addInteractionConfig(COORD qubit1, COORD qubit2, double val) {
  if (_interactions.count(std::make_pair(qubit1, qubit2))) {
    QASSERT(_interactions.at(std::make_pair(qubit1, qubit2)).value == val);
  } else if (_interactions.count(std::make_pair(qubit2, qubit1))) {
    QASSERT(_interactions.at(std::make_pair(qubit2, qubit1)).value == val);
  } else {
    ConfigElement con;
    HW_Loc loc(qubit1, qubit2);
    con.loc = loc;
    con.value = val;
    _interactions.insert(std::make_pair(std::make_pair(qubit1, qubit2), con));
  }

}


void DeviceGen::doGenerate() {

  qlog.speak("Generate", "Generate configuration");
  ELE_ITER e_iter = _par_netlist->element_begin();
  for (; e_iter != _par_netlist->element_end(); ++e_iter) {
    ParElement* element = *e_iter;
    ParGrid* grid = element->getCurrentGrid();
    QASSERT(grid);
    COORD x_index = grid->getLoc().getLocX();
    COORD y_index = grid->getLoc().getLocY();
    CellGen* cellgen= new CellGen(grid);
    _loc_to_cellgen.insert(std::make_pair(std::make_pair(x_index, y_index),
          cellgen));

  }

  qlog.speak("Generate", "Collect routing information to reoslve qubit assignment");
  std::vector<ParWireTarget*>& targets = _par_netlist->getTargets();
  for (size_t i = 0; i < targets.size(); ++i) {
    ParWireTarget* target = targets[i];
    
    if (target->getDontRoute()) continue;

    RoutePath* route = target->getRoutePath();


    QASSERT(route->size() >= 2);
    RoutingNode* src = route->at(0);
    SYN::Pin* src_pin = src->getPin();

    RoutingNode* tgt = route->at(route->size()-1);
    SYN::Pin* tgt_pin = tgt->getPin();

    std::vector<RoutingEdge*>& edges = route->getEdges();
    QASSERT(route->size() >= 2);
    RoutingEdge* src_edge = edges[0];
    RoutingEdge* tgt_edge = edges[edges.size()-1];

    RoutingNode* src_qubit = src_edge->getOtherNode(src);
    RoutingNode* tgt_qubit = tgt_edge->getOtherNode(tgt);

    COORD x_index_src = src_qubit->getQubit()->getLoc().getLocX();
    COORD y_index_src = src_qubit->getQubit()->getLoc().getLocY();

    COORD x_index_tgt = tgt_qubit->getQubit()->getLoc().getLocX();
    COORD y_index_tgt = tgt_qubit->getQubit()->getLoc().getLocY();

    CellGen* src_cellgen = _loc_to_cellgen.at(std::make_pair(x_index_src, y_index_src));
    CellGen* tgt_cellgen = _loc_to_cellgen.at(std::make_pair(x_index_tgt, y_index_tgt));

    src_cellgen->assignPin(src_pin, src_qubit->getQubit()->getLoc().getLocalIndex());
    tgt_cellgen->assignPin(tgt_pin, tgt_qubit->getQubit()->getLoc().getLocalIndex());
  }

  qlog.speak("Generate", "Generate chains");
  WIRE_ITER w_iter = _par_netlist->wire_begin();
  for (; w_iter != _par_netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    InteractionGen* interac = new InteractionGen(wire);
    _v_interactions.push_back(interac);
    interac->generateConfig(this);
  }

  w_iter = _par_netlist->model_wire_begin();
  for (; w_iter != _par_netlist->model_wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    if (wire->getElementNumber() == 0) continue;
    SYN::Pin* u_pin = wire->getUniqElementPin();
    QASSERT(u_pin);
    ParElement* element = wire->getUniqElement();
    ParGrid* grid = element->getCurrentGrid();
    QASSERT(grid);
    COORD x_index = grid->getLoc().getLocX();
    COORD y_index = grid->getLoc().getLocY();
    CellGen*  cellgen = _loc_to_cellgen.at(std::make_pair(x_index, y_index));
    cellgen->assignPin(u_pin);
  }
 
  qlog.speak("Generate", "Generate cell configuration");
  LocToCellGen::iterator cell_iter = _loc_to_cellgen.begin();
  for (; cell_iter != _loc_to_cellgen.end(); ++cell_iter) {
    CellGen* cellgen = cell_iter->second;
    cellgen->generateConfig(this);
  }

  qlog.speak("Generate", "Done. Total Qubit %lu, Interactions %lu",
      _qubits.size(),
      _interactions.size());

}

void InteractionGen::generateConfig(DeviceGen* device) {

  std::unordered_set<RoutingNode*> nodes = _wire->getUsedRoutingNodes();
  std::unordered_set<RoutingNode*>::iterator n_iter = nodes.begin();
  for (; n_iter != nodes.end(); ++n_iter) {
    RoutingNode* node = *n_iter;
    if (node->isPin()) continue;
    if (node->isLogic()) continue;

    if (node->isInteraction()) {
      HW_Interaction* hw_inter = node->getInteraction();

      ConfigElement chain;
      chain.loc = hw_inter->getLoc();
      chain.value = -1.0;

      _inter_configs.insert(std::make_pair(std::make_pair(hw_inter->getLoc().getInteractionIndex().first, hw_inter->getLoc().getInteractionIndex().second),
            chain));
      device->addInteractionConfig(hw_inter->getLoc().getInteractionIndex().first,
                                   hw_inter->getLoc().getInteractionIndex().second,
                                   -1.0);
      
    } else if (node->isQubit()) {
      HW_Qubit* qubit = node->getQubit();

      ConfigElement qu;
      qu.loc = qubit->getLoc();
      qu.value = 0.0;

      _qubit_configs.insert(std::make_pair(qubit->getLoc().getGlobalIndex(), qu));
      device->addQubitConfig(qubit->getLoc().getGlobalIndex(), 0.0);

    } else {
      QASSERT(0);
    }

  }


}

