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

/*!
 * \file qpar_place.cc
 * \author Juexiao Su
 * \date 29 Oct 2017
 * \brief class to implemement data structure and algorithm for placement
 */


#include "qpar/qpar_place.hh"
#include "qpar/qpar_target.hh"
#include "qpar/qpar_netlist.hh" 
#include "qpar/qpar_place_cost.hh"


#include "utils/qlog.hh"

#include <fstream>
#include <algorithm>
#include <cmath>

#if 0
#define DBG_CODE(code) code
#else
#define DBG_CODE(code)
#endif


QPlace::~QPlace() {
    if (_used_matrix)
      delete _used_matrix;
    _used_matrix = NULL;

    if (_annealer)
      delete _annealer;
    _annealer = NULL;

    if (_placement_cost)
      delete _placement_cost;
    _placement_cost = NULL;
}

void QPlace::run() {
  initializePlacement();

  _placement_cost = new CongestionAwareCost;

  _current_total_cost = computeTotalCost(true);

  qlog.speak("Place", "Initial placement cost is %.6f", _current_total_cost);

  const int num_move = std::max((int)_movable_elements.size(), 100);
  const int move_limit = ((float)4*std::pow(_movable_elements.size(), (float)1.333));
  _annealer->setRLimit((float)(std::max(_hw_target->getXLimit(), _hw_target->getYLimit())));

  const float final_limit = 1.0;
  const float init_t = getStartingT();
  _annealer->setInitT(init_t);
  _annealer->setCurrentT(init_t);
  
  int tot_iter = 0;
  int outer_iter = 0;
  int move_since_recompute = 0;

  double cost_ave = 0;
  double sum_of_square = 0;
  int success_num = 0;

  qlog.speak("Place", "Start Placement");
  qlog.speak("Place", "|=======================================================");
  qlog.speak("Place", "|%5s|%11s|%10s |%9s  |   %7s | %10s|", "Iter", "T",  "Cost", "Succ_Swap",   "R_Limit",   "Tot Mov");

  while(!_annealer->shouldExit(_current_total_cost / (float)(_netlist->getWireNum()))) {
    cost_ave = 0;
    sum_of_square = 0.0;
    success_num = 0;
    ++outer_iter;

    for (unsigned inner_iter = 0; inner_iter < move_limit; ++inner_iter) {
      if (tryMove()) {
        ++success_num;
        cost_ave += _current_total_cost;
        sum_of_square = _current_total_cost * _current_total_cost;
      }
    }

    move_since_recompute += move_limit;
    if (move_since_recompute > 50000) {
      float new_cost = computeTotalCost(true);
      float delta = new_cost - _current_total_cost;
      qlog.speak("Place", "Recompute cost delta is %f", delta);
      _current_total_cost = new_cost;
      move_since_recompute = 0;
    }

    tot_iter += move_limit;
    float success_rat = (float)(success_num) / (float)move_limit;
    if (success_num > 0)
      cost_ave /= success_num;
    else 
      cost_ave = _current_total_cost;

    double std_dev = getStdDev(success_num, cost_ave, sum_of_square);
    float old_t = _annealer->getCurrentT();
    _annealer->updateT(success_rat);
    _annealer->updateMoveRadius(success_rat);

    qlog.speak("Place","|%5d|%11.3f|%10.2f |%7d  |   %.3f | %10d|", outer_iter, _annealer->getCurrentT(), _current_total_cost, success_num, _annealer->getRLimit(), tot_iter);

    if (outer_iter > 105)
      break;

  }

  sanityCheck();
  ELE_ITER ele_iter = _netlist->element_begin();
  for (; ele_iter != _netlist->element_end(); ++ele_iter) {
    ParElement* element = *ele_iter;
    element->updatePlacement();
  }
}

void QPlace::sanityCheck() {
  WIRE_ITER w_iter = _netlist->wire_begin();
  for(; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    QASSERT(wire->sanityCheck());
  }
}

double QPlace::computeTotalCost(bool set_wire_cost) {
  double cost = 0.0;
  WIRE_ITER w_iter = _netlist->wire_begin();
  for(; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    double cost_t = _placement_cost->computeCost(wire, *_used_matrix);
    cost += cost_t;
    if (set_wire_cost)
      wire->setCost(cost_t);
  }
  return cost;
}


void QPlace::initializePlacement() {

  // initialize annealer
  float max_r = std::max(_hw_target->getXLimit(), _hw_target->getYLimit());
  _annealer = new Annealer(100.0, 1.0, max_r);

  ParGridContainer& grids = _hw_target->getGrids();
  grids.shuffle();
  unsigned grid_index = 0;

  ELE_ITER ele_iter = _netlist->element_begin();
  for (; ele_iter != _netlist->element_end(); ++ele_iter) {
    ParElement* element = *ele_iter;
    if (!element->isMovable()) {
      qlog.speakError("Placement does not support fixed element!");
    }
    _movable_elements.push_back(element);

    while (!grids[grid_index]->canBePlaced() && grid_index < grids.size())
      ++grid_index;

    if (grid_index == grids.size()) 
      qlog.speakError("Placement does not have enough resources to performance placement");

    element->setGrid(grids[grid_index]);
    element->save();
    grids[grid_index]->setParElement(element);
    grids[grid_index]->save();
    ++grid_index;
  }


  // initilize bounding box for each wire
  WIRE_ITER w_iter = _netlist->wire_begin();
  for(; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    wire->initializeBoundingBox();
  }

  COORD y_limit = _hw_target->getYLimit();
  COORD x_limit = _hw_target->getXLimit();

  QASSERT(y_limit);QASSERT(x_limit);
  _used_matrix = new qpr_matrix<unsigned>(x_limit, y_limit);

  // initilize grid utilization matrix
  if (_hw_target->getGrid(0,0)->getCurrentElement())
    _used_matrix->cell(0,0) = 1;
  else
    _used_matrix->cell(0,0) = 0;

  for (COORD y = 1; y < y_limit; ++y) {
    if (_hw_target->getGrid(0,y)->getCurrentElement())
      _used_matrix->cell(0,y) = _used_matrix->cell(0,y-1) + 1;
    else
      _used_matrix->cell(0,y) = _used_matrix->cell(0,y-1);
  }

  for (COORD x = 1; x < x_limit; ++x) {
    if (_hw_target->getGrid(x, 0)->getCurrentElement())
      _used_matrix->cell(x, 0) = _used_matrix->cell(x-1,0) + 1;
    else
      _used_matrix->cell(x, 0) = _used_matrix->cell(x-1,0);
  }

  for (COORD x = 1; x < x_limit; ++x) {
    for (COORD y = 1; y < y_limit; ++y) {
      unsigned used_element = 
        _used_matrix->cell(x-1, y) +
        _used_matrix->cell(x, y-1) -
        _used_matrix->cell(x-1, y-1);
      if (_hw_target->getGrid(x,y)->getCurrentElement())
        _used_matrix->cell(x,y) = used_element + 1;
      else
        _used_matrix->cell(x,y) = used_element;
    }
  }

  qlog.speak("QPlace", "Dump initial placement result");
  dumpCurrentPlacement("init.place"); 
  dumpUsedMatrix("init.matrix");
}

void QPlace::usedMatrixSanityCheck() {
  for (unsigned i = 0; i < _used_matrix->getSizeX(); ++i)
    for (unsigned j = 0; j < _used_matrix->getSizeY(); ++j)
      usedMatrixSanityCheck(i, j);
}

void QPlace::usedMatrixSanityCheck(unsigned x, unsigned y) {
  unsigned sum = 0;
  for (COORD i = 0; i <= x; ++i) {
    for (COORD j = 0; j <= y; ++j) {
      if (_hw_target->getGrid(i,j)->getCurrentElement())
        ++sum;
    }
  }
  if (_used_matrix->cell(x, y) != sum) {
    dumpCurrentPlacement("debug.place");
    dumpUsedMatrix("debug.usedmatrix");
  }
  QASSERT(_used_matrix->cell(x, y) == sum);
}

void QPlace::checkIfReadyToMove() {
  WIRE_ITER w_iter = _netlist->wire_begin();
  for(; w_iter != _netlist->wire_end(); ++w_iter) {
    ParWire* wire = *w_iter;
    QASSERT(wire->isReadyToMove());
  }

  ELE_ITER ele_iter = _netlist->element_begin();
  for (; ele_iter != _netlist->element_end(); ++ele_iter) {
    ParElement* element = *ele_iter;
    QASSERT(element->isReadyToMove());
  }

  for (COORD x = 0; x < _hw_target->getXLimit(); ++x) {
    for (COORD y = 0; y < _hw_target->getYLimit(); ++y) {
      QASSERT(_hw_target->getGrid(x,y)->isReadyToMove());
    }
  }


}

void QPlace::generateMove(ParElement* &element, COORD& x, COORD& y) {
  unsigned ele_i = _random_gen.uRand(0, _movable_elements.size()-1);
  element = _movable_elements[ele_i];

  COORD ele_x = element->getX();
  COORD ele_y = element->getY();

  float r_limit = _annealer->getRLimit();

  COORD x_range_min = std::ceil(((float)ele_x >= r_limit) ? (float)ele_x - r_limit : 0);
  COORD x_range_max = (((float)ele_x + r_limit) >= (float)_hw_target->getXLimit()) ? 
    (_hw_target->getXLimit() - 1) : std::floor(((float)ele_x + r_limit));
  QASSERT((ele_x - x_range_min) < r_limit);
  QASSERT((x_range_max - ele_x) < r_limit);


  COORD y_range_min = std::ceil(((float)ele_y >= r_limit) ? (float)ele_y - r_limit : 0);
  COORD y_range_max = (((float)ele_y + r_limit) >= (float)_hw_target->getYLimit()) ? 
    (_hw_target->getYLimit() - 1) : std::floor(((float)ele_y + r_limit));
  QASSERT((ele_y - y_range_min) < r_limit);
  QASSERT((y_range_max - ele_y) < r_limit);

  do {
    x = _random_gen.iRand(x_range_min, x_range_max);
    y = _random_gen.iRand(y_range_min, y_range_max);
  } while( x == ele_x && y == ele_y );
}


bool QPlace::tryMove() {
  //1) check if they placer is ready to move, this is unnecessary, only for debug
  checkIfReadyToMove();
  usedMatrixSanityCheck();
  sanityCheck();

  COORD x = std::numeric_limits<COORD>::max();
  COORD y = std::numeric_limits<COORD>::max();

  ParElement* ele = NULL;
  generateMove(ele, x, y);

  COORD from_x = ele->getX();
  COORD from_y = ele->getY();

  ParGrid* tgt_grid = _hw_target->getGrid(x, y);
  bool is_swap = tgt_grid->getCurrentElement();


  findAffectedElementsAndWires(ele, x, y);

  double delta_cost = 0;
  WIRE_ITER w_iter = _affected_wires.begin();
  for (; w_iter != _affected_wires.end(); ++w_iter) {
    ParWire* par_wire = *w_iter;
    par_wire->saveCost();
    double new_cost = _placement_cost->computeCost(par_wire, *_used_matrix);
    delta_cost += (new_cost - par_wire->getSavedCost());
  }

  bool accept = _annealer->shouldAccept(delta_cost);
  if (accept) {
    commitMove();
    _current_total_cost += delta_cost;
  } else 
    restoreMove(from_x, from_y, x, y, is_swap);

  _affected_wires.clear();
  _affected_elements.clear();
  _affected_grids.clear();
  return accept;
}

void QPlace::commitMove() {
  for (size_t i = 0; i < _affected_elements.size(); ++i) {
    ParElement* element = _affected_elements[i];
    element->save();
  }

  for (size_t i = 0; i < _affected_grids.size(); ++i) {
    ParGrid* grid = _affected_grids[i];
    grid->save();
  }

  WIRE_ITER w_iter = _affected_wires.begin();
  for (; w_iter != _affected_wires.end(); ++w_iter) {
    (*w_iter)->saveBoundingBox();
    (*w_iter)->saveCost();
  }

}

void QPlace::restoreMove(COORD from_x, COORD from_y, COORD to_x, COORD to_y, bool is_swap) {
  for (size_t i = 0; i < _affected_elements.size(); ++i) {
    ParElement* element = _affected_elements[i];
    element->restore();
  }

  for (size_t i = 0; i < _affected_grids.size(); ++i) {
    ParGrid* grid = _affected_grids[i];
    grid->restore();
  }

  WIRE_ITER w_iter = _affected_wires.begin();
  for (; w_iter != _affected_wires.end(); ++w_iter) {
    (*w_iter)->restore();
  }

  if (!is_swap)
    updateUseMatrix(to_x, to_y, from_x, from_y);

}


void QPlace::findAffectedElementsAndWires(ParElement* element, COORD dest_x, COORD dest_y) {
  QASSERT(_affected_elements.size() == 0);
  QASSERT(_affected_wires.size() == 0);
  QASSERT(_affected_grids.size() == 0);
  element->save();

  ParGrid* src_grid = element->getCurrentGrid();
  src_grid->save();

  ParGrid* tgt_grid = _hw_target->getGrid(dest_x, dest_y);
  tgt_grid->save();

  _affected_grids.push_back(src_grid);
  _affected_grids.push_back(tgt_grid);


  ParElement* tgt_element = tgt_grid->getCurrentElement();
  bool is_swap = tgt_element;

  COORD from_x = src_grid->getLoc().getLocX();
  COORD from_y = src_grid->getLoc().getLocY();

  COORD to_x = tgt_grid->getLoc().getLocX();
  COORD to_y = tgt_grid->getLoc().getLocY();
  

  if (is_swap) {
    tgt_element->save();
    _affected_elements.push_back(element);
    _affected_elements.push_back(tgt_element);
    tgt_grid->setParElement(element);
    src_grid->setParElement(tgt_element);

    element->setGrid(tgt_grid);
    tgt_element->setGrid(src_grid);
  } else {
    _affected_elements.push_back(element);
    tgt_grid->setParElement(element);
    src_grid->setParElement(NULL);

    element->setGrid(tgt_grid);

    ////only swap to an empty grid need to update the use matrix
    //updateUseMatrix(src_grid->getLoc().getLocX(), src_grid->getLoc().getLocY(),
    //              dest_x, dest_y);

    ////sanity check
    updateUseMatrix(from_x, from_y, to_x, to_y);
    usedMatrixSanityCheck();
  }


  std::vector<ParElement*>::iterator ele_iter = _affected_elements.begin();
  for (; ele_iter != _affected_elements.end(); ++ele_iter) {
    ParElement* curr_ele = *ele_iter;
    WIRE_ITER_V w_iter = curr_ele->begin();
    for (; w_iter != curr_ele->end(); ++w_iter) {
      _affected_wires.insert(*w_iter);
      if (curr_ele == element)
        (*w_iter)->updateBoundingBox(from_x, from_y, to_x, to_y);
      else if (curr_ele == tgt_element)
        (*w_iter)->updateBoundingBox(to_x, to_y, from_x, from_y);
      else
        QASSERT(0);
    }
  }


  if (!is_swap) {
    WIRE_ITER w_iter = _netlist->wire_begin(); 
    for (; w_iter != _netlist->wire_end(); ++w_iter) {
      if (_affected_wires.count(*w_iter)) continue;

      ParWire* wire = *w_iter;
      if (wire->getCurrentBoundingBox().getBoundBox().isInBox((unsigned)from_x, (unsigned)from_y) ||
          wire->getCurrentBoundingBox().getBoundBox().isInBox((unsigned)to_x, (unsigned)to_y))
        _affected_wires.insert(*w_iter);
    }
  }
}

void QPlace::updateUseMatrix(COORD from_x, COORD from_y, COORD to_x, COORD to_y) {
  QASSERT(from_x != to_x || from_y != to_y);
  bool x_inc = to_x >= from_x;
  bool y_inc = to_y >= from_y;

  if (x_inc == y_inc) {
    for (COORD i = (x_inc)?from_x:to_x; i < _used_matrix->getSizeX(); ++i) {
      for (COORD j = (x_inc)?from_y:to_y; j <= (((x_inc)?to_y:from_y) - 1); ++j) {
        if (x_inc)
          --_used_matrix->cell((unsigned)i, (unsigned)j);
        else
          ++_used_matrix->cell((unsigned)i, (unsigned)j);
      }
    }

    for (COORD i = (x_inc)?from_x:to_x; i <= (((x_inc)?to_x:from_x) - 1); ++i) {
      for (COORD j = (x_inc)?to_y:from_y; j < _used_matrix->getSizeY(); ++j) {
        if (x_inc)
          --_used_matrix->cell((unsigned)i, (unsigned)j);
        else
          ++_used_matrix->cell((unsigned)i, (unsigned)j);
      }
    }

  } else {
    for (COORD i = (x_inc)?from_x:to_x; i <= (((x_inc)?to_x:from_x) - 1); ++i) {
      for (COORD j = (x_inc)?from_y:to_y; j < _used_matrix->getSizeY(); ++j) {
        if (x_inc)
          --_used_matrix->cell((unsigned)i, (unsigned)j);
        else
          ++_used_matrix->cell((unsigned)i, (unsigned)j);
      }
    }

    for (COORD i = (x_inc)?to_x:from_x; i < _used_matrix->getSizeX(); ++i) {
      for (COORD j = (x_inc)?to_y:from_y; j <= (((x_inc)?from_y:to_y) - 1); ++j) {
        if (x_inc)
          ++_used_matrix->cell((unsigned)i, (unsigned)j);
        else
          --_used_matrix->cell((unsigned)i, (unsigned)j);
      }
    }
  }


}

void QPlace::dumpUsedMatrix(std::string filename) const {

  std::ofstream outfile;
  outfile.open(filename.c_str());

  qlog.speak("QPlace", "Dump utilization matrix to %s", filename.c_str());

  outfile << "#used matrix"<< std::endl;
 
  COORD y_limit = _hw_target->getYLimit();
  COORD x_limit = _hw_target->getYLimit();
  for (COORD y = 0; y < y_limit; ++y) {
    for (COORD x = 0; x < x_limit; ++x) {
      outfile << "x: "  << x << " y: " << y << " " << _used_matrix->cell((unsigned)x, (unsigned)y) << std::endl;
    }
  }

  outfile.close();
}

void QPlace::dumpCurrentPlacement(std::string filename) const {
  std::ofstream outfile;
  outfile.open(filename.c_str());
  

  qlog.speak("QPlace", "Dump placement to %s", filename.c_str());
  ELE_ITER ele_iter = _netlist->element_begin();
  for (; ele_iter != _netlist->element_end(); ++ele_iter) {
    ParElement* element = *ele_iter;
    COORD x = element->getX();
    COORD y = element->getY();
    std::string name = element->getName();
    outfile << "Gate: " << name <<
      " " << x <<" " << y << std::endl;
  }

  outfile.close();

}


float QPlace::getStartingT() {
  qlog.speak("Place", "Getting starting temperature...");
  const int num_move = std::max((int)_movable_elements.size(), 100);
  unsigned num_accepted = 0;
  double ave = 0.0;
  double sum_of_square = 0.0;
  for (int i = 0; i < num_move; ++i) {
    qlog.speak("JSU_DEBUG", "Try move %d", i);
    dumpCurrentPlacement("debug.place");
    dumpUsedMatrix("debug.usedmatrix");
    if (tryMove()) {
      ++num_accepted;
      ave += _current_total_cost;
      sum_of_square += (_current_total_cost * _current_total_cost);
    }

  }

  if (num_accepted > 0)
    ave /= num_accepted;

  double std_dev = getStdDev(num_accepted, ave, sum_of_square) ;
  qlog.speak("Place", "std_dev: %g, avg_cost: %g, starting_T: %g\n", std_dev, ave, 20 * std_dev) ;


  return static_cast<float>(20 * std_dev);
}

double QPlace::getStdDev(unsigned num, double ave, double s_square) const {
  
  double std_dev = 0.0;
  if (num > 1) {
    std_dev = (s_square - (num*ave*ave))/(num - 1);
  } else {
    std_dev = s_square;
  }

  if (std_dev > 0)
    std_dev = std::sqrt(std_dev);

  return std_dev;
}





