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

 	@file EIControler.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "EIControler.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>
#include <vector>
#include <algorithm>
#include <functional>
#include "assert.h"

        EIControler::EIControler()
{
}



void EIControler::addEmptyStream(EIItem* parent)
{
    EIStream* newStream = new EIStream(parent);
    bool ok = parent->addChild(newStream);
    if(!ok)
        delete newStream;
}

void EIControler::addEmptyGroup(EIItem* parent)
{
    EIGroup* newGroup = new EIGroup();
    bool ok = parent->addChild(newGroup);
    if(!ok)
        delete newGroup;
}

void EIControler::removeItem(EIItem* _item)
{
    EIItem* parentItem = _item->parent();
    if(parentItem)
    {
        int i=0;
        while((i<parentItem->childCount())&&(parentItem->child(i)!=_item))
            i++;
        if(i<parentItem->childCount())
        {
            delete parentItem->child(i);
            parentItem->removeChild(i);
        }
    }

}

void EIControler::resetProcessFacts(EIItem* root)
{
    QList<EIItem*> groups = EIReader::getItems(root,true,EI::GROUP);
    EIGroup* curGroup;
    for(int i=0;i<groups.size();i++)
    {
        if(!((EIGroup*)groups.at(i))->isFactVariable())
        {
            curGroup = ((EIGroup*)groups.at(i));
            curGroup->getFact()->max=1;
            curGroup->getFact()->min=1;
            curGroup->getFact()->value=1;
            curGroup->setFieldValue(EIGroup::COSTMULT,0);
        }
    }
}

void EIControler::resetAllDTMin_2(EIItem* root,double value)
{
    QList<EIItem*> streams = EIReader::getItems(root,true,EI::STREAM);
    EIStream* curStream;
    for(int i=0;i<streams.size();i++)
    {
        curStream = ((EIStream*)streams.at(i));
        curStream->setFieldValue(EIStream::DTMIN2,value);
    }
}



void EIControler::setItems(QDomElement & domEl,EIItem* rootEI)
{
    rootEI->clearDescendants();

    int curDepth=0;
    QDomNode n;
    QDomElement domItem;
    QString parentName;
    bool curDepthFound = true;
    EIItem* newEIitem=NULL;

    n = domEl.firstChild();
    while( !n.isNull() )
    {
        // curLevel elements
        domItem = n.toElement();

        QString node = domItem.nodeName();

        if(node.compare("EIStream",Qt::CaseInsensitive)==0)
        {
            newEIitem = new EIStream(domItem);
            rootEI->addChild(newEIitem);
        }

        if(node.compare("EIGroup",Qt::CaseInsensitive)==0)
        {
            newEIitem = new EIGroup(domItem);
            rootEI->addChild(newEIitem);
        }

        if(node.compare("EIModelContainer",Qt::CaseInsensitive)==0)
        {
            newEIitem = new EIModelContainer(domItem);
            rootEI->addChild(newEIitem);
        }


        if(node.compare("EIItem",Qt::CaseInsensitive)==0)
        {
            newEIitem = new EIItem(domItem);
            rootEI->addChild(newEIitem);
        }

        //fill children
        if(newEIitem)
            setItems(domItem,newEIitem);

        n = n.nextSibling();
    }
}


QList<EIStream*> EIControler::splitStream(EIStream* toSplit,double firstFraction)
{
    if((firstFraction<0)||(firstFraction>1))
        infoSender.debug("Error: firstFraction should be between 0 and 1 (here = "+QString::number(firstFraction)+")");

    if(!toSplit->numerized())
        infoSender.debug("Stream should be numerized before being splitted.");

    QList<EIStream*> streams;
    EIStream* streamA = new EIStream(*toSplit);
    EIStream* streamB = new EIStream(*toSplit);

    streamA->QflowNum * firstFraction;
    streamA->QflowNum = streamA->QflowNum * (1-firstFraction);
}
