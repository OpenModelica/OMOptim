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

     @file MyEAEvalContinue.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 

  */

#ifndef _MyEAEvalContinue_h
#define _MyEAEvalContinue_h

#include <eoContinue.h>
#include <eoEvalFuncCounter.h>
//#include "continuator/moContinuator.h"

/** 
 * Continues until a number of evaluations has been made
*/
template< class EOT>
class MyEAEvalContinue: public eoContinue<EOT>
{
public:
  /// Ctor 
  MyEAEvalContinue( eoEvalFuncCounter<EOT> & _eval, unsigned long _totalEval,bool* _stop)
      : eval(_eval), repTotalEvaluations( _totalEval ) {
    std::cout << "Ctor de eoEvalFuncCounter avec total = " << repTotalEvaluations << std::endl;
    stop = _stop;
};
  
  /** Returns false when a certain number of evaluations has been done
   */
  virtual bool operator() ( const eoPop<EOT>& _vEO ) {
      if (eval.value() >= repTotalEvaluations) 
      {
    std::cout << "STOP in MyEAEvalContinue: Reached maximum number of evaluations [" << repTotalEvaluations << "]\n";
    return false;
      }
      if ((*stop))
      {
    std::cout << "STOP asked by user. Reached" << repTotalEvaluations << "evaluations \n";
  return false;
      }
      return true;
  }
  
  /** Returns the number of generations to reach*/
  virtual unsigned long totalEvaluations( ) 
  {  
    return repTotalEvaluations; 
  };
  
  virtual std::string className(void) const { return "MyEAEvalContinue"; }
private:
  eoEvalFuncCounter<EOT> & eval;
  unsigned long repTotalEvaluations;
  bool* stop;
};

///**
// * Continues until a number of evaluations has been made
//*/
//template< class EOT>
//class MyEAEvalContinueMo: public moContinuator<EOT>
//{
//public:
//  /// Ctor
//  MyEAEvalContinueMo( eoEvalFuncCounter<EOT> & _eval, unsigned long _totalEval,bool* _stop)
//    : eval(_eval), repTotalEvaluations( _totalEval ) {
//    std::cout << "Ctor de eoEvalFuncCounter avec total = " << repTotalEvaluations << std::endl;
//  stop = _stop;
//};

//  /** Returns false when a certain number of evaluations has been done
//   */
//  virtual bool operator() ( const eoPop<EOT>& _vEO ) {
//    if (eval.value() >= repTotalEvaluations)
//    {
//            std::cout << "STOP in MyEAEvalContinue: Reached maximum number of evaluations [" << repTotalEvaluations << "]\n";
//            return false;
//    }
//    if ((*stop))
//    {
//            std::cout << "STOP asked by user. Reached" << repTotalEvaluations << "evaluations \n";
//          return false;
//    }
//    return true;
//  }

//  /** Returns the number of generations to reach*/
//  virtual unsigned long totalEvaluations( )
//  {
//    return repTotalEvaluations;
//  };

//  virtual std::string className(void) const { return "MyEAEvalContinue"; }
//private:
//  eoEvalFuncCounter<EOT> & eval;
//  unsigned long repTotalEvaluations;
//  bool* stop;
//};


#endif
