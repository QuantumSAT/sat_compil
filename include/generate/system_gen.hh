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

#ifndef SYSTEM_GEN_HH
#define SYSTEM_GEN_HH

/*!
 * \file system_gen.hh
 * \author Juexiao Su
 * \date 22 Dec 2017
 * \brief based on placement and routing solution to generation dwave configuration
 */


#include "hw_target/hw_loc.hh"
#include <boost/functional/hash.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>

class ParGrid;
class ParWire;
class ParNetlist;

namespace SYN {
  class Gate;
  class Pin;
}

struct ConfigElement {
  HW_Loc loc;
  double value;
};


typedef std::unordered_map<COORD,ConfigElement> QubitConfigs;
typedef std::unordered_map<std::pair<COORD, COORD>, ConfigElement,
        boost::hash<std::pair<COORD, COORD> > > InteractionConfigs;

typedef std::unordered_map<COORD, int> QubitState;


class DeviceGen;
class InteractionGen;

class CellGen {

public:
  /*! \brief generate the config for a cell
   */ 
  CellGen(ParGrid* grid);

  /*! \brief generate the config for a cell;
   */
  void generateConfig(DeviceGen* device);

  /*! \brief assign pin to location
   */
  void assignPin(SYN::Pin* pin, COORD);

  /*! \brief assign pin to first avaible location
   */
  void assignPin(SYN::Pin*);

  /*! \brief config spin value
   */
  void configSpin(COORD local, double val);

  /*! \brief config interaction 
   */
  void configInteraction(COORD local1, COORD local2, double val);

  /*! \brief get qubit configs
   */
  QubitConfigs& getQubitConfigs() { return _qubit_configs; }

  /*! \brief get interaciton configs
   */
  InteractionConfigs& getInteractionConfigs() { return _inter_configs; }

   /*! \brief get ground state energy for cell
   */
  double getGroundEnergy() const;

  void printConfig() const;

  void printGroundState() const;
private:

  ParGrid* _grid;  //!< placement and routing grid

  std::unordered_map<SYN::Pin*, COORD> _pin_to_loc; //!< netlist pin to its local location
  std::unordered_set<COORD> _used_qubit;
  std::vector<std::pair<COORD, COORD> > _incell_chains;

  QubitConfigs _qubit_configs; //!< qubit configs
  InteractionConfigs _inter_configs; //!< interaction configs

  QubitState _ground_state; //!< one of the ground state

};

class InteractionGen {

public:
  /*! \brief default constructor
   */
  InteractionGen(ParWire* wire) : _wire(wire) {}

  /*! \brief generate interaction configuration
   */
  void generateConfig(DeviceGen* device);


  /*! \brief get qubit configs
   */
  QubitConfigs& getQubitConfigs() { return _qubit_configs; }

  /*! \brief get interaciton configs
   */
  InteractionConfigs& getInteractionConfigs() { return _inter_configs; }


  /*! \brief get ground energy
   */
  double getGroundEnergy() const;


  void printConfig() const;

  void printGroundState() const;

private:
  ParWire* _wire; //!< route of wire

  QubitConfigs _qubit_configs; //!< qubit configs
  InteractionConfigs _inter_configs; //!< interaction configs

};

/*! \brief device configuration generator, it generates the real configuration based on placement and routing
 */
class DeviceGen {

typedef std::unordered_map<std::pair<COORD, COORD>, CellGen*,
        boost::hash<std::pair<COORD,COORD> > > LocToCellGen;

public:

  /*! \brief default constructor, the parnetlist will provice all necessary placement and 
   *         routing information
   */
  DeviceGen(ParNetlist* par_netlist) :
  _par_netlist(par_netlist) {}

  /*! \brief default destructor
   */
  ~DeviceGen();


  /*! \brief generate configuration
   */
  void doGenerate();


  /*! \brief dump generation
   */
  void dumpConfiguation();

  /*! \brief dump configuration based on d-wave format
   */
  void dumpDwaveConfiguration(std::string filename);

  /*! \brief add qubit config
   */
  void addQubitConfig(COORD x, double val);

  /*! \brief add interaction config
   */
  void addInteractionConfig(COORD qubit1, COORD qubit2, double val);

  /*! \brief get device ground energy
   */
  double getGroundEnergy() const;

  /*! \brief check if the device achieve the ground state
   */

private:
  ParNetlist* _par_netlist; //!< placement and routing netlist
  LocToCellGen _loc_to_cellgen; //!< location to cell

  std::vector<InteractionGen*> _v_interactions; //!< all interactions

  QubitConfigs _qubits; //!< all the used qubits
  InteractionConfigs _interactions; //!< all the used interactions

};




#endif


