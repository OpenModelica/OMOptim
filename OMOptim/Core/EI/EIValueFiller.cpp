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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)


        @file EIValueFiller.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9

  */

#include "EIValueFiller.h"

EIValueFiller::EIValueFiller()
{
}

void EIValueFiller::getFilledEI(EIItem* referencedEI, EIItem* filledEI, QList<ModModel*> models,MOOptVector* inputVars,
                                Project* project)
{

    // get references
    QMap<EIItem*,QStringList> mapRef = getReferences(referencedEI,true,project);
    QMap<EIItem*,QStringList> mapMissingRef = getMissingReferences(referencedEI,inputVars,true,project);


    // Models missing

    // remove those already known
    ModModel* test;

    ModModelPlus* modPlus = project->modModelPlus(test);
//    OneSimulation oneSim(project,rootModClass,modReader,modPlusCtrl,modPlus);

//    oneSim.launch(ProblemConfig());


}


QMap<EIItem*,QStringList> EIValueFiller::getReferences(EIItem* referencedEI, bool onlyChecked,Project* project)
{
    QMap<EIItem*,QStringList> mapRefs;
    QMap<EIItem*,QStringList> childMapRefs;
    EIItem* curItem;

    QStringList curRefs;
    ModModel* curModel;
    ModModelPlus* curModModelPlus;
    QString tmpRef;
    bool refInModel;

    if(!onlyChecked || referencedEI->isChecked())
    {
        curRefs = referencedEI->references();
        // add model name in front of ref if it is in.
        // 1 : is referencedEI from a model
        curModel = dynamic_cast<ModModel*>(project->modReader()->findInDescendants(project->rootModClass(),referencedEI->model()));
        // 2 : if yes, for each reference, add modelName+"." in front of each reference
        if(curModel)
        {
            curModModelPlus = project->modModelPlus(curModel);
            curModModelPlus->readVariables();

            for(int i=0;i<curRefs.size();i++)
            {
                refInModel = project->modReader()->findInDescendants(curModel,curRefs.at(i));
                refInModel = refInModel || curModModelPlus->variables()->findItem(curRefs.at(i));

                if(refInModel)
                {
                    tmpRef = curRefs.at(i);
                    tmpRef = curModel->name()+"."+tmpRef;
                    curRefs.replace(i,tmpRef);
                }
            }
        }
        mapRefs.insert(referencedEI,curRefs);

        for(int iC=0;iC<referencedEI->childCount();iC++)
        {
            childMapRefs = EIValueFiller::getReferences(referencedEI->child(iC),onlyChecked,project);

            for(int iK=0;iK<childMapRefs.keys().size();iK++)
            {
                curItem = childMapRefs.keys().at(iK);
                mapRefs.insert(curItem,childMapRefs.value(curItem));
            }
        }
    }
    return mapRefs;
}


QMap<EIItem*,QStringList> EIValueFiller::getMissingReferences(EIItem* referencedEI,MOOptVector* inputVars,bool onlyChecked,Project* project)
{
    QMap<EIItem*,QStringList> mapRef = EIValueFiller::getReferences(referencedEI,onlyChecked,project);
    QMap<EIItem*,QStringList> mapMissingRefs;
    QStringList curMissingRefs;
    QStringList curRefs;

    EIItem* curItem;

    for(int iItem=0;iItem<mapRef.keys().size();iItem++)
    {
        curItem = mapRef.keys().at(iItem);
        curRefs = mapRef.value(curItem);
        curMissingRefs.clear();
        for(int iRef=0;iRef<curRefs.size();iRef++)
        {
            if(!inputVars->alreadyIn(curRefs.at(iRef)))
                curMissingRefs.push_back(curRefs.at(iRef));
        }

        if(curMissingRefs.size()>0)
            mapMissingRefs.insert(curItem,curMissingRefs);
    }
    return mapMissingRefs;
}
