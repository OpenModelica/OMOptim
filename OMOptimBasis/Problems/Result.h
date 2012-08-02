// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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

    @file Result.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
/*!
 * \file Result.h
 * \brief File containing Results class description.
 * \author Hubert Thieriot (CEP-Armines)
 * \version 0.1
 */

#if !defined(_RESULT_H)
#define _RESULT_H

#include "Problem.h"
#include "MOVector.h"
#include "QtCore/QTime"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include "Info.h"
#include <QtXml/QDomDocument>
#include "OMCase.h"

class Problem;

class Result: public OMCase
{
    Q_OBJECT

    /*! \class Result
   *
   *  All problems should get a personnalized result class derived from this one.
   */

public :
    QTime _duration; /// duration of the computation who led to this result
    QDateTime _date; /// launching date of the computation who led to this result

protected :
    bool _success; /// is the result a success
    Problem* _problem; /// cloned version of original problem


public:
    Result();
    Result(ProjectBase*,const Problem &);
    Result(const Result &r);
    virtual ~Result(void);

    virtual QDomElement toXmlData(QDomDocument &)=0; ///@sa OMCase::toXmlData(QDomDocument &doc)

    void setDefaultSaveFileName();


    virtual QString getFieldName(int iField,int iRole);
    virtual unsigned getNbFields();

    Problem* problem();
    bool isSuccess();

    virtual void setProblem(Problem &);
    void setSuccess(bool);

};



#endif
