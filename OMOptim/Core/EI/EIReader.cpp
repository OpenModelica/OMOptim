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

  @file EIReader.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "EIReader.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>
#include <vector>
#include <algorithm>
#include <functional>
#include "assert.h"

namespace EI
{
EIReader::EIReader()
{
}



bool EIReader::isInDescendants(EIItem* parent, QString fullName)
{
    EIItem* foundClass = findInDescendants(parent,fullName);
    return (bool)(foundClass);
}

EIItem* EIReader::findInDescendants(EIItem* parent,QString fullName)
{
    EIItem* curChild;
    QString curFullName = parent->name();

    int curDepth = parent->depth();
    int lookingDepth = fullName.split(".").size()-1;

    //if empty, return NULL
    if(fullName.isEmpty())
        return NULL;

    // check if it is this component
    if(curFullName == fullName)
        return parent;

    //first check name compatibility
    if(fullName.indexOf(curFullName)!=0)
        return NULL;

    //get child name to visit
    QString childShortName = fullName.section(".",curDepth,curDepth);

    // looking in children
    for(int iChild=0;iChild<parent->childCount();iChild++)
    {
        curChild = parent->child(iChild);
        if(curChild->name(EI::SHORT)==childShortName)
            return findInDescendants(curChild,fullName);
    }
    return NULL;
}

/**
* Find an item in descendants where fieldvalue of field iField == itemFieldValue.
* @warning if iField = Name : findInDescendants will compare with short name !! use findInDescendants instead.
* @sa findInDescendants
*/
EIItem* EIReader::findInDescendants(EIItem* parent, EI::Type eiType, QVariant itemFieldValue, int iField)
{

    // check if it is this item
    if((parent->getEIType()==eiType) && (parent->getFieldValue(iField) == itemFieldValue))
        return parent;

    // looking in children
    int iChild=0;
    EIItem* foundItem=NULL;
    while(!foundItem && iChild<parent->childCount())
    {
        foundItem = findInDescendants(parent->child(iChild),eiType,itemFieldValue,iField);
        iChild++;
    }
    return foundItem;
}



QList<EIStream*> EIReader::getStreams(const EIItem* parent)
{
    QList<EIStream*> result;

    for(int i=0;i<parent->streamChildCount();i++)
        result.push_back(dynamic_cast<EIStream*>(parent->streamChild(i)));

    for(int i=0;i<parent->childCount();i++)
    {
        if(parent->child(i)->getEIType()!=EI::STREAM)
            result.append(getStreams(parent->child(i)));
    }
    return result;
}

QList<EIItem*> EIReader::getItems(const EIItem* parent,bool recursive,EI::Type filter)
{
    QList<EIItem*> result;

    for(int i=0;i<parent->childCount();i++)
    {
        if((filter == EI::GENERIC)||(parent->child(i)->getEIType()==filter))
            result.push_back(parent->child(i));
    }

    if(recursive)
    {
        for(int i=0;i<parent->childCount();i++)
        {
            if(parent->child(i)->getEIType()!=EI::STREAM)
                result.append(getItems(parent->child(i),recursive,filter));
        }
    }

    return result;
}

QList<EIStream*> EIReader::getValidNumerizedStreams(const EIItem*parent,MOOptVector *variables,bool onlyChecked)
{
    QList<EIStream*> result = getStreams(parent);

    EIStream* curStream;
    bool keep;
    int i=0;
    while(i<result.size())
    {
        curStream = result.at(i);
        QString error;
        keep = !onlyChecked || curStream->isChecked();
        keep = keep && curStream->isValid(variables,error);
        keep = keep && curStream->numerize(variables);

        if(!keep)
            result.removeAt(i);
        else i++;
    }
    return result;
}

void EIReader::getValidTk(const EIItem* parent, QList<METemperature> & Tk,MOOptVector *variables,bool useCorrectedT)
{
    QList<EIStream*> streams = getValidNumerizedStreams(parent,variables,true);

    EIStream* curStream;
    double DTmin2prov; // Temperature in Kelvin

    bool ok1,ok2,ok;
    QString msg;

    Tk.clear();



    for(int i=0;i<streams.size();i++)
    {
        curStream = streams.at(i);

        if(curStream->TinNum(useCorrectedT)==curStream->ToutNum(useCorrectedT))
        {
            msg.clear();
            msg.sprintf("In stream %s, Tin = Tout. This stream won't be considered",curStream->name().toLatin1().data());
            infoSender.send(Info(msg,ListInfo::WARNING2));
            ok=false;
        }

        Tk.push_back(curStream->TinNum(useCorrectedT));
        Tk.push_back(curStream->ToutNum(useCorrectedT));
    }


    //sort
    qSort(Tk.begin(),Tk.end());

    //remove duplicates
    QList<METemperature>::iterator iter = std::unique(Tk.begin(),Tk.end());
    Tk.erase(iter,Tk.end());
}

/**
* Look in item hierarchy for the first group with a variable factor.
* If not found, return false (fact and group are set to NULL)
*/
bool EIReader::getFirstParentGroupFact( EIItem* item,EIGroupFact* &fact,EIGroup* &group)
{
    fact = NULL;
    group = NULL;
    EIItem* parentItem=item;
    bool found = false;
    while(fact==NULL && parentItem!=NULL)
    {
        parentItem = parentItem->parent();
        EIGroup* _group = dynamic_cast<EIGroup*>(parentItem);
        if(_group)
        {
            group = _group;
            if(_group->isFactVariable())
            {
                found = true;
                fact = _group->getFact();
            }
        }
    }
    return found;
}
/**
* Check if parent, grand parent (...) is a fact variable group.
*/

bool EIReader::isInFactVariable( EIItem* item)
{
    EIGroupFact* fact;
    EIGroup* group;

    EIGroup* curGroup = dynamic_cast<EIGroup*>(item);
    if(curGroup && (curGroup->isFactVariable()))
        return true;
    else
        return getFirstParentGroupFact(item,fact,group);
}

QStringList EIReader::getAllItemNames(const EIItem* item,EI::Type filter)
{
    QStringList itemNames;
    EIItem* curChild;
    for(int i=0;i<item->childCount();i++)
    {
        curChild = item->child(i);

        if((filter==EI::GENERIC)||(curChild->getEIType()==filter))
            itemNames.push_back(curChild->name(EI::FULL));

        itemNames.append(getAllItemNames(curChild,filter));
    }
    return itemNames;
}



// Streams filter and sort functions
QList<EIStream*> EIReader::getStreamsAboveT(METemperature T,QList<EIStream*> allStreams,bool useCorrectedT)
{
    QList<EIStream*> list;
    EIStream* curStream;


    for(int i=0;i<allStreams.size();i++)
    {
        curStream = allStreams.at(i);

        if((curStream->TinNum(useCorrectedT)>T)||(curStream->ToutNum(useCorrectedT)>T))
            list.push_back(curStream);
    }
    return list;
}

QList<EIStream*> EIReader::getStreamsBelowT(METemperature T,QList<EIStream*> allStreams,bool useCorrectedT)
{
    QList<EIStream*> list;
    EIStream* curStream;

    for(int i=0;i<allStreams.size();i++)
    {
        curStream = allStreams.at(i);

        if((curStream->TinNum(useCorrectedT)<T)||(curStream->ToutNum(useCorrectedT)<T))
            list.push_back(curStream);
    }
    return list;

}

QList<EIStream*> EIReader::getStreamsPresentInDT(METemperature Thot,METemperature Tcold,QList<EIStream*> allStreams,bool useCorrectedT)
{
    QList<EIStream*> list;
    EIStream* curStream;

    list = getStreamsBelowT(Thot,allStreams,useCorrectedT);
    list = getStreamsAboveT(Tcold,list,useCorrectedT);

    return list;
}


MEQflow EIReader::getIntervalQFlow(METemperature T1,METemperature T2,EIStream* stream,bool useCorrectedT)
{
    METemperature TIntHot = std::max(T1,T2);
    METemperature TIntCold = std::min(T1,T2);
    Q_ASSERT(stream->ToutNum(useCorrectedT)!=stream->TinNum(useCorrectedT));

    METemperature TStreamHot = std::max(stream->TinNum(useCorrectedT),stream->ToutNum(useCorrectedT));
    METemperature TStreamCold = std::min(stream->TinNum(useCorrectedT),stream->ToutNum(useCorrectedT));



    //if stream not in interval, return 0;
    if((TStreamHot<TIntCold)||(TStreamCold>TIntHot))
        return 0;


    METemperature dTint = std::min(TStreamHot,TIntHot)-std::max(TStreamCold,TIntCold);
    METemperature dTstream = TStreamHot-TStreamCold;
    double ratio = dTint.value()/dTstream.value();

    MEQflow dQ = stream->QflowNum()*ratio;
    Q_ASSERT(dQ.value()>=0);
    return dQ;
}

QList<int> EIReader::getTIntervalsConcerned(const QList<METemperature> &Tk,EIStream* stream,bool useCorrectedT)
{
    QList<int> result;


    METemperature TStreamHot = std::max(stream->TinNum(useCorrectedT),stream->ToutNum(useCorrectedT));
    METemperature TStreamCold = std::min(stream->TinNum(useCorrectedT),stream->ToutNum(useCorrectedT));



    for(int i=0;i<Tk.size()-1;i++)
    {
        if((TStreamCold<std::max(Tk.at(i),Tk.at(i+1)))
                &&(TStreamHot>std::min(Tk.at(i),Tk.at(i+1))))
            result.push_back(i);
    }
    return result;
}

QList<EIStream*> EIReader::getColdStreams(QList<EIStream*> allStreams)
{
    QList<EIStream*> list;
    EIStream* curStream;

    for(int i=0;i<allStreams.size();i++)
    {
        curStream = allStreams.at(i);

        if(curStream->TinNum(false) < curStream->ToutNum(false))
            list.push_back(curStream);
    }
    return list;
}

QList<EIStream*> EIReader::getHotStreams(QList<EIStream*> allStreams)
{
    QList<EIStream*> list;
    EIStream* curStream;

    for(int i=0;i<allStreams.size();i++)
    {
        curStream = allStreams.at(i);

        if(curStream->TinNum(false) > curStream->ToutNum(false))
            list.push_back(curStream);
    }
    return list;
}

void EIReader::sortByCp(QList<EIStream*> & allStreams,Qt::SortOrder order)
{
    if(order==Qt::AscendingOrder)
        qSort(allStreams.begin(),allStreams.end(),EIReader::CpLowerThan);
    else
        qSort(allStreams.begin(),allStreams.end(),EIReader::CpGreaterThan);
}

bool EIReader::CpLowerThan(EIStream* s1, EIStream* s2)
{
    bool ok1,ok2;
    return s1->Cp(ok1) < s1->Cp(ok2);
}

bool EIReader::CpGreaterThan( EIStream* s1,  EIStream* s2)
{
    return !CpLowerThan(s1,s2);
}
}

