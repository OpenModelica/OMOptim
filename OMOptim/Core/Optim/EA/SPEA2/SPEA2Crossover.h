// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR 
 * THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE. 
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

     @file EA2Crossover.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 0.9 

  */
#ifndef EA2CROSSOVER_H
#define EA2CROSSOVER_H

#include <eoOp.h>

#include "LowTools.h"
#include "MOParameter.h"

template<class EOT>
class EA2Crossover: public eoQuadOp<EOT>
{
public:
  /// Ctor
  EA2Crossover(MOParameters* _parameters):eoQuadOp<EOT>() {parameters = _parameters;}
  virtual std::string className() const {return "EA2Crossover";}
virtual bool operator()(EOT& _eoA, EOT& _eoB ) {
    
    // for double, int and bool, both eos are modified, new values are taken between two old values)
    // !! need to be studied and adapted to specific problems

    bool oneHasChanged = false;

    int iVar1,iVar2;
    double newValue1, newValue2;
    
    //********************************
    // Crossover on double variables 
    //********************************
  if(rng.flip(parameters->value("doublePCross",0.25).toDouble()) && (_eoA.doubleVars.size()>0))
    {
  // getting new eoA iVar1 value
  iVar1 = LowTools::round(rng.random(_eoA.doubleVars.size()-1));
  //newValue = random value between eoA and eoB var values
  newValue1 = std::min<double>(_eoA.doubleVars.at(iVar1),_eoB.doubleVars.at(iVar1)) //min
      + rng.random(fabs(_eoA.doubleVars.at(iVar1)-_eoB.doubleVars.at(iVar1)));
  // getting new eoB iVar2 value
  iVar2 = LowTools::round(rng.random(_eoA.doubleVars.size()-1));
  //newValue = random value between eoA and eoB var values
  newValue2 = std::min<double>(_eoA.doubleVars.at(iVar2),_eoB.doubleVars.at(iVar2)) //min
      + rng.random(fabs(_eoA.doubleVars.at(iVar2)-_eoB.doubleVars.at(iVar2)));
  // modifying eos
  _eoA.doubleVars.at(iVar1) = newValue1;
  _eoB.doubleVars.at(iVar2) = newValue2;

  oneHasChanged = true;
    }
    
    //********************************
    // Crossover on int variables 
    //********************************
  if(rng.flip(parameters->value("intPCross",0.25).toDouble()) && (_eoA.intVars.size()>0))
    {
  // getting new eoA iVar1 value
  iVar1 = LowTools::round(rng.random(_eoA.intVars.size()-1));
  //newValue = random value between eoA and eoB var values
  newValue1 = std::min<double>(_eoA.intVars.at(iVar1),_eoB.intVars.at(iVar1)) //min
      + rng.random(abs((_eoA.intVars.at(iVar1))-(_eoB.intVars.at(iVar1))));
  // getting new eoB iVar2 value
  iVar2 = LowTools::round(rng.random(_eoA.intVars.size()-1));
  //newValue = random value between eoA and eoB var values
  newValue2 = std::min<double>(_eoA.intVars.at(iVar2),_eoB.intVars.at(iVar2)) //min
      + rng.random(abs((_eoA.intVars.at(iVar2)-_eoB.intVars.at(iVar2))));
  // modifying eos    
  _eoA.intVars.at(iVar1) = LowTools::round(newValue1);
  _eoB.intVars.at(iVar2) = LowTools::round(newValue2);

  oneHasChanged = true;
    }

    //********************************
    // Crossover on bool variables 
    //********************************
  if(rng.flip(parameters->value("boolPCross",0.25).toDouble()) && (_eoA.boolVars.size()>0))
    {

  iVar1 = LowTools::round(rng.random(_eoA.boolVars.size()-1));
  bool newBoolValue1 =_eoB.boolVars.at(iVar1);

  iVar2 = LowTools::round(rng.random(_eoA.boolVars.size()-1));
  bool newBoolValue2 =_eoA.boolVars.at(iVar2);

  _eoA.boolVars.at(iVar1)=newBoolValue1;
  _eoB.boolVars.at(iVar2)=newBoolValue2;

  oneHasChanged = true;
    }
    
    return oneHasChanged;
}
private :

  MOParameters *parameters;


};

#endif
