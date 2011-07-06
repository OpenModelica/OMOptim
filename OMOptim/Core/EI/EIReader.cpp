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
 	@version 0.9 

  */
#include "EIReader.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>
#include <vector>
#include <algorithm>
#include <functional>
#include "assert.h"

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
* Description Find an item in descendants where fieldvalue of field iField == itemFieldValue.
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



QList<EIStream*> EIReader::getStreams(EIItem* parent)
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

QList<EIItem*> EIReader::getItems(EIItem* parent,bool recursive,EI::Type filter)
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

QList<EIStream*> EIReader::getValidStreams(EIItem*parent,MOOptVector *variables,bool onlyChecked)
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
        if(!keep)
			result.removeAt(i);
        else i++;
	}
	return result;
}

void EIReader::getValidTk(EIItem* parent, QList<METemperature> & Tk,MOOptVector *variables)
{
	QList<EIStream*> streams = getValidStreams(parent,variables,true);

	EIStream* curStream;
    double DTmin2prov; // Temperature in Kelvin
	
    bool ok1,ok2,ok;
    QString msg;
	
    Tk.clear();

    METemperature TinCorr,ToutCorr;

	for(int i=0;i<streams.size();i++)
	{
		curStream = streams.at(i);
		DTmin2prov = curStream->getFieldValue(EIStream::DTMIN2).toDouble();
        TinCorr = curStream->TinNum;
        ToutCorr = curStream->ToutNum;
        if(curStream->TinNum==curStream->ToutNum)
		{
            msg.clear();
            msg.sprintf("In stream %s, Tin = Tout. This stream won't be considered",curStream->name().utf16());
            infoSender.send(Info(msg,ListInfo::WARNING2));
            ok=false;
        }

        if(curStream->TinNum<curStream->ToutNum)
        {
			//Cold stream
            TinCorr += DTmin2prov;
            ToutCorr += DTmin2prov;
		}
		else
		{
			//Hot stream
            TinCorr+= -DTmin2prov;
            ToutCorr += -DTmin2prov;
		}

        Tk.push_back(TinCorr);
        Tk.push_back(ToutCorr);
	}


	//sort
    qSort(Tk.begin(),Tk.end());

	//remove duplicates
    QList<METemperature>::iterator iter = std::unique(Tk.begin(),Tk.end());
    Tk.erase(iter,Tk.end());
	}

/**
* Description Look in item hierarchy for the first group with a variable factor.
*/
void EIReader::getFirstParentGroupFact(EIItem* item,EIGroupFact* &fact,EIGroup* &group)
{
	fact = NULL;
	group = NULL;
        EIItem* parentItem=item;
    while(fact==NULL && parentItem!=NULL)
	{
                parentItem = parentItem->parent();
                EIGroup* _group = dynamic_cast<EIGroup*>(parentItem);
		if(_group)
		{
            group = _group;
			if(_group->isFactVariable())
			{
				fact = _group->getFact();
			}
		}
	}
}

QStringList EIReader::getAllItemNames(EIItem* item,EI::Type filter)
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
QList<EIStream*> EIReader::getStreamsAboveT(METemperature T,QList<EIStream*> allStreams)
{
    QList<EIStream*> list;
    EIStream* curStream;

    for(int i=0;i<allStreams.size();i++)
    {
        curStream = allStreams.at(i);

        if((curStream->TinNum>T)||(curStream->ToutNum>T))
            list.push_back(curStream);
    }
    return list;
}

QList<EIStream*> EIReader::getStreamsBelowT(METemperature T,QList<EIStream*> allStreams)
{
    QList<EIStream*> list;
    EIStream* curStream;

    for(int i=0;i<allStreams.size();i++)
    {
        curStream = allStreams.at(i);

        if((curStream->TinNum<T)||(curStream->ToutNum<T))
            list.push_back(curStream);
    }
    return list;

}

QList<EIStream*> EIReader::getStreamsPresentInDT(METemperature Thot,METemperature Tcold,QList<EIStream*> allStreams)
{
    QList<EIStream*> list;
    EIStream* curStream;

    list = getStreamsBelowT(Thot,allStreams);
    list = getStreamsAboveT(Tcold,list);

    return list;
}


static MEQflow EIReader::getIntervalQFlow(METemperature T1,METemperature T2,EIStream* stream,bool useCorrectedT)
{
    METemperature TIntHot = std::max(T1,T2);
    METemperature TIntCold = std::min(T1,T2);
    Q_ASSERT(stream->ToutNum!=stream->TinNum);

    METemperature TStreamHot = std::max(stream->TinNum,stream->ToutNum);
    METemperature TStreamCold = std::min(stream->TinNum,stream->ToutNum);

    if(useCorrectedT)
    {
        bool isHotStream = (stream->TinNum > stream->ToutNum);
        double DTmin2prov = stream->getFieldValue(EIStream::DTMIN2).toDouble();

        if(!isHotStream)
        {
            //Cold stream
            TStreamHot += DTmin2prov;
            TStreamCold += DTmin2prov;
        }
        else
        {
            //Hot stream
            TStreamHot += -DTmin2prov;
            TStreamCold += -DTmin2prov;
        }

    }


    //if stream not in interval, return 0;
    if((TStreamHot<TIntCold)||(TStreamCold>TIntHot))
        return 0;


    METemperature dTint = std::min(TStreamHot,TIntHot)-std::max(TStreamCold,TIntCold);
    METemperature dTstream = TStreamHot-TStreamCold;
    double ratio = dTint.value()/dTstream.value();

    MEQflow dQ = stream->QflowNum*ratio;
    Q_ASSERT(dQ.value()>=0);
    return dQ;
}

QList<int> EIReader::getTIntervalsConcerned(const QList<METemperature> &Tk,EIStream* stream,bool useCorrectedT)
{
    QList<int> result;


    METemperature TStreamHot = std::max(stream->TinNum,stream->ToutNum);
    METemperature TStreamCold = std::min(stream->TinNum,stream->ToutNum);

    if(useCorrectedT)
    {
        bool isHotStream = (stream->TinNum > stream->ToutNum);
        double DTmin2prov = stream->getFieldValue(EIStream::DTMIN2).toDouble();

        if(!isHotStream)
        {
            //Cold stream
            TStreamHot += DTmin2prov;
            TStreamCold += DTmin2prov;
        }
        else
        {
            //Hot stream
            TStreamHot += -DTmin2prov;
            TStreamCold += -DTmin2prov;
        }
    }

    for(int i=0;i<Tk.size()-1;i++)
    {
        if((TStreamCold<std::max(Tk.at(i),Tk.at(i+1)))
            &&(TStreamHot>std::min(Tk.at(i),Tk.at(i+1))))
            result.push_back(i);
    }
    return result;
}

static QList<EIStream*> EIReader::getColdStreams(QList<EIStream*> allStreams)
{
    QList<EIStream*> list;
    EIStream* curStream;

    for(int i=0;i<allStreams.size();i++)
    {
        curStream = allStreams.at(i);

        if(curStream->TinNum < curStream->ToutNum)
            list.push_back(curStream);
    }
    return list;
}

static QList<EIStream*> EIReader::getHotStreams(QList<EIStream*> allStreams)
{
    QList<EIStream*> list;
    EIStream* curStream;

    for(int i=0;i<allStreams.size();i++)
    {
        curStream = allStreams.at(i);

        if(curStream->TinNum > curStream->ToutNum)
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

bool EIReader::CpLowerThan(const EIStream* s1, const EIStream* s2)
{
    bool ok1,ok2;
    return s1->Cp(ok1) < s1->Cp(ok2);
}

bool EIReader::CpGreaterThan(const EIStream* s1, const EIStream* s2)
{
   return !CpLowerThan(s1,s2);
}

