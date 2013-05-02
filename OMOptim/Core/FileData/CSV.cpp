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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

        @file CSV.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */
#include "CSV.h"

// below include must be in .cpp, not in header file
// to avoid circular references
#include "Variable.h"
#include "MOOptVector.h"
#include "OptObjective.h"

CSV::CSV(void)
{
}

CSV::~CSV(void)
{
}

QString CSV::space()
{
        return "___";
}


QString CSV::optObjectivesToLines(MOVector<OptObjective>* objectives)
{
        QString text = "\t ObjectiveName \t Direction (Minimize = 1, Maximize = 0) \t Description -->\n";

        OptObjective *curObj;

        for (int i=0;i<objectives->size();i++)
        {
                curObj=objectives->at(i);

                //Objective fields
                text += ("#OBJ");

                for (int nf=0;nf<OptObjective::nbFields;nf++)
                {
                        text += "\t";
                        text += curObj->getFieldValue(nf).toString();
                }
                text += "\n";
        }
        return text;
}





void CSV::LinesToOptObjectives(MOVector<OptObjective>* objectives, QString lines)
{
        //Clear variables
        objectives->clear();

        QStringList lineList = lines.split("\n");
        QStringList fields;
        OptObjective *newObjective;

        for(int nl=0;nl<lineList.size();nl++)
        {
                if(!lineList[nl].isEmpty())
                {

                        fields = lineList[nl].split("\t");

                        //Treating only lines begininng with #OBJ
                        //left(4) is used to avoid space problems
                        if(fields[0].left(4)=="#OBJ")
                        {
                                newObjective = new OptObjective();
                                for (int nf=0;(nf<OptObjective::nbFields)&&(nf<fields.size()-1);nf++)
                                {
                                        newObjective->setFieldValue(nf,QVariant(fields[nf+1]));
                                }
                                objectives->addItem(newObjective);
                        }
                }
        }
}

void CSV::LinesToOptObjectives(MOVector<OptObjectiveResult>* objectives, QString lines)
{

        //Clear variables
        objectives->clear();

        QStringList lineList = lines.split("\n");
        QStringList fields;
        OptObjectiveResult *newObjective;

        for(int nl=0;nl<lineList.size();nl++)
        {
                if(!lineList[nl].isEmpty())
                {
                        fields = lineList[nl].split("\t");

                        //Treating only lines begininng with #OBJ
                        //left(4) is used to avoid space problems
                        if(fields[0].left(4)=="#OBJ")
                        {
                                newObjective = new OptObjectiveResult();
                                for (int nf=0;(nf<OptObjective::nbFields)&&(nf<fields.size()-1);nf++)
                                {
                                        newObjective->setFieldValue(nf,QVariant(fields[nf+1]));
                                }
                                objectives->addItem(newObjective);
                        }
                }
        }
}

