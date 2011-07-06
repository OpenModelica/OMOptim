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

 	@file MilpHEN1.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef MILPHEN1_H
#define MILPHEN1_H

#include <glpk.h>
#include <stdio.h>
#include <stdlib.h>
#include "EIItem.h"
#include "EIStream.h"
#include "EITools.h"
#include "GlpkTools.h"

#include "InfoSender.h"
#include "EIConnConstrs.h"
#include "MilpSet.h"
#include "MilpParam.h"
#include "MilpVariableResult.h"
#include "EIHEN1Parameters.h"
#include "EIHEN1Result.h"
#include "EIConns.h"


class MilpHEN1
{
public:
        MilpHEN1(EITree* eiTree,MOParameters *parameters,EIConnConstrs *_connConstrs,MOOptVector *variables,QDir folder,QString modFilePath, QString dataFilePath);
	~MilpHEN1(void);

        EIHEN1Result* launch();
	

private :
        glp_prob * launchGLPK();
        void DataToFile(QString dataFilePath,
	QList<EIStream*> &eiProcessStreams,
	QList<QList<MEQflow> > &Qpk, //.at(iStream).at(iDTk)
	QList<EIStream*> &eiUtilityStreams,
	QList<QList<MEQflow> > &Quk,//.at(iStream).at(iDTk)
	QMultiMap<EIGroupFact*,EIStream*> &factStreamMap, // multimap <unit multiplier, Streams concerned>,
	QMap<EIGroupFact*,EIGroupFact*> &factsRelation, // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
	QMap<EIGroupFact*,EIGroup*> &factGroupMap);

        EIHEN1Result* readResult(glp_prob *);
        EIConns* readEIConns(glp_prob *,QStringList colNames);

        static int hook(void *info, const char *s);


        EITree* _eiTree;
        MOOptVector *_variables;
        QDir _folder;
        QString _modFileName;
        QString _dataFileName;
        QString _resFileName;
        QString _logFileName;
        QString _sensFileName;
        EIConnConstrs *_connConstrs;
        MOParameters *_parameters;
        QList<METemperature> _Tk;

        int _QFlowUnit;
        int _TempUnit;

};


#endif
