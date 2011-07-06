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



/**
* This function is used to get a EIItem (with its children) where references are replaced by their corresponding values.
* If references are not found in variables, corresponding models will be simulated.
* @param referencedEI is the EIItem* root. It will not be modified.
* @param variables where it will be looked for references values.
* @return Result is a copy of referencedEI where references haves been replaced by their corresponding values.
*/
EITree* EIValueFiller::getFilledEI(EITree* referencedEI,MOOptVector* variables,
                                Project* project)
{

    EITree* filledEI = new EITree(*referencedEI);

    filledEI->removeUnchecked();

    // get references
    QMap<EIItem*,QStringList> mapMissingRef = getMissingReferences(filledEI->rootElement(),variables,true,project);
    QMap<EIItem*,QStringList> mapAllRefs = getReferences(filledEI->rootElement(),true,project);


    // Get Models missing
    QStringList refs;
    QList<QStringList> tmpRefs = mapMissingRef.values();
    ModModel* corrModel;
    ModModelPlus* corrModModelPlus;
    QList<ModModel*> missingModels;

    missingModels = corrModels(mapMissingRef,project);

    // Simulate models missing and add onesim outputVars in target inputVars
    for(int iModel=0;iModel<missingModels.size();iModel++)
    {
        corrModModelPlus = project->modModelPlus(missingModels.at(iModel));
        OneSimulation oneSim(project,project->modClassTree(),project->modPlusCtrl(),corrModModelPlus);

        OneSimResult* result = oneSim.launch(ProblemConfig());

        if(result->isSuccess())
        {
            variables->append(*result->finalVariables(),true);
        }
    }


    // fill values and remove those still incomplete
    QList<EIItem*> referenceItems = mapAllRefs.uniqueKeys();
    EIItem* curEIItem;
    bool numerizeOk;
    for(int i=0;i<referenceItems.size();i++)
    {
        curEIItem = referenceItems.at(i);
        numerizeOk = curEIItem->numerize(variables);
        if(!numerizeOk)
        {
            infoSender.send(Info("Removing EIItem "+curEIItem->name()+" because contains an unsolved reference",ListInfo::WARNING2));
            filledEI->removeItem(curEIItem);
}
    }

    return filledEI;
}


/**
* This function is used to get map of references involved in referencedEI and its children.
* @param onlyChecked specifies if only checked EIItem 's should be considered.
* @return A map where keys are EIItems with references and corresponding values are corresponding lists of references.
*/
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
        curModel = dynamic_cast<ModModel*>(project->modClassTree()->findInDescendants(referencedEI->model()));
        // 2 : if yes, for each reference, add modelName+"." in front of each reference
        if(curModel)
        {
            curModModelPlus = project->modModelPlus(curModel);
            curModModelPlus->readVariables();

            for(int i=0;i<curRefs.size();i++)
            {
                refInModel = project->modClassTree()->findInDescendants(curRefs.at(i),curModel);
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


/**
* This function is used to get map of missing references involved in referencedEI and its children.
* A missing reference is a reference which could not be found in variables.
* @param onlyChecked specifies if only checked EIItem 's should be considered.
* @return A map where keys are EIItems with missing references and corresponding values are corresponding lists of missing references.
*/
QMap<EIItem*,QStringList> EIValueFiller::getMissingReferences(EIItem* referencedEI,MOOptVector* variables,bool onlyChecked,Project* project)
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
            if(!variables->alreadyIn(curRefs.at(iRef)))
                curMissingRefs.push_back(curRefs.at(iRef));
        }

        if(curMissingRefs.size()>0)
            mapMissingRefs.insert(curItem,curMissingRefs);
    }
    return mapMissingRefs;
}

/**
* This function is returns corresponding models of references indicated in mapRefs.
* @param mapRefs is a map of EIItem* and corresponding references.
* @return A map where keys are EIItems with missing references and corresponding values are corresponding lists of missing references.
*/
QList<ModModel*> EIValueFiller::corrModels(QMap<EIItem*,QStringList> mapRefs,Project* project)
{

    QStringList refsNames;
    QList<QStringList> tmpRefs = mapRefs.values();
    QList<ModModel*> models;
    ModModel* corrModel;

    for(int iK=0;iK<tmpRefs.size();iK++)
    {
        refsNames.append(tmpRefs.at(iK));
    }
    LowTools::removeDuplicates(refsNames);

    for(int i=0;i<refsNames.size();i++)
    {
        corrModel = project->modClassTree()->modelOf(refsNames.at(i));
        if(corrModel && !models.contains(corrModel))
            models.push_back(corrModel);
    }

    return models;
}
