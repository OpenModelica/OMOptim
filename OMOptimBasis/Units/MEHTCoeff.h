/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)
 * @file MEHTCoeff.h
 * @brief Comments for file documentation.
 * @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 * Company : CEP - ARMINES (France)
 * http://www-cep.ensmp.fr/english/
 * @version
 */


// $Id: MEHTCoeff.h 9551 2011-07-28 16:56:59Z hubert.thieriot $

#if !defined(_MEHTCOEFF_H)
#define _MEHTCOEFF_H

#include "MEDimValue.h"
#include <cmath>

class MEHTCoeff : public MEDimValue
{
public:
        MEHTCoeff();
        MEHTCoeff(double value,int unit);
        MEHTCoeff(const MEHTCoeff&);
        ~MEHTCoeff();

    enum Units
    {
            W_M2_K,
            KW_M2_K,
            MW_M2_K
    };

    QString unit(int iUnit)  const;
        QString unit() const;  //should'nt be (should be inherited) but error in compiler otherwise
    unsigned nbUnits() const;
    double convert(double value,int orgUnit,int dstUnit) const;

        MEHTCoeff& operator+=(const MEHTCoeff&);
        MEHTCoeff& operator-=(const MEHTCoeff&);
        MEHTCoeff operator-(const MEHTCoeff&) const;
        MEHTCoeff operator+(const MEHTCoeff&) const;

};




#endif
