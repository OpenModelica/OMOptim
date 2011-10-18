#ifndef EIMODELICAHE_H
#define EIMODELICAHE_H

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

        @file EIModelicaExtractor.h
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */


#include <QtCore/QString>

/** EIModelicaHE is a base class for heat exchangers informations. EIModelicaHE is an abstract class.
  * Inheriting classes should inform their classNames, fileNames, and ports.
  */


namespace EI
{

enum HEType
{
    HEType1
};

class EIModelicaHE
{
public:
    EIModelicaHE();
    virtual int getType() = 0;
    virtual QString className() = 0;
    virtual QString inletC()= 0;
    virtual QString inletH()= 0;
    virtual QString outletC()= 0;
    virtual QString outletH()= 0;
};

class HE1 : public EIModelicaHE
{
public:
    HE1(){};
    int getType(){return HEType1;}
    QString className(){return "CERESInfo.EnergyIntegration.EIHE1"; }
    QString inletC(){return "coldInlet";}
    QString inletH(){return "hotInlet";}
    QString outletC(){return "coldOutlet";}
    QString outletH(){return "hotOutlet";}
};

class EIModelicaHES
{
public :
   static  EIModelicaHE* getInstance(HEType type)
    {
        switch(type)
        {
        case HEType1 :
            return new HE1();
        }
    }
};



}
#endif // EIMODELICAHE_H
