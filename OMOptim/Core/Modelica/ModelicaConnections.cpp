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

  @file ModelicaConnections.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "ModelicaConnections.h"
#include "ModItem.h"

ModelicaConnections::ModelicaConnections(ModItemsTree* modClassTree)
    :MOVector<ModelicaConnection>(true)
{
    _modClassTree = modClassTree;
}

ModelicaConnections::~ModelicaConnections(void)
{
    clear();
}

//void ModelicaConnections::clear()
//{
//	for(int i=0;i<items.size();i++)
//	{
//		delete items.at(i);
//	}
//}
//
//void ModelicaConnections::addConnection(ModelicaConnection* newConnection)
//{
//	int index=items.size();
//	insertRow(index);//,createIndex(0,0));
//	beginInsertRows(QModelIndex(),index,index);
//	items.push_back(newConnection);
//	endInsertRows();
//}
//
//
//
//int ModelicaConnections::columnCount(const QModelIndex &parent) const
// {
//     return 2;
// }
//
// QVariant ModelicaConnections::data(const QModelIndex &index, int role) const
// {
//     if (!index.isValid())
//         return QVariant();
//
//     if (role != Qt::DisplayRole)
//         return QVariant();
//
//	 ModelicaConnection *connect = items.at(index.row());
//
//	 if(index.column()==0)
//	 {
//		 return connect->getA()->name(Modelica::DYMOLA);
//	 }
//	 if(index.column()==1)
//	 {
//		 return connect->getB()->name(Modelica::DYMOLA);
//	 }
//	 return QVariant();
// }
//
// Qt::ItemFlags ModelicaConnections::flags(const QModelIndex &index) const
// {
//     if (!index.isValid())
//         return Qt::ItemIsEnabled;
//
//     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
// }
//
// QVariant ModelicaConnections::headerData(int section, Qt::Orientation orientation,int role) const
// {
//     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
//	 {
//		 switch(section)
//		 {
//		 case 0:
//			 return "A Name";
//		 case 1:
//			 return "B Name";
//		 default :
//			 return "Unknown field";
//		 }
//	 }
//
//     return QVariant();
// }
//
// QModelIndex ModelicaConnections::index(int row, int column, const QModelIndex &parent)const
// {
//         return createIndex(row, column);
// }
//
// QModelIndex ModelicaConnections::parent(const QModelIndex &index) const
// {
//   return QModelIndex();
//
// }
//
// int ModelicaConnections::rowCount(const QModelIndex &parent) const
// {  
//		 return items.size();
// }
//

ModelicaConnections* ModelicaConnections::filter(ModItem* element,bool includeChildren,bool acceptInternal)
{
    ModelicaConnections* elConn = new ModelicaConnections(_modClassTree);
    QString elName = element->name();
    QString a,b;
    ModelicaConnection* curConn;

    for(int i=0;i<this->size();i++)
    {
        curConn = this->at(i);
        a = curConn->getA();
        b = curConn->getB();

        if((QString::compare(elName,a,Qt::CaseInsensitive)==0)
                ||(QString::compare(elName,b,Qt::CaseInsensitive)==0))
            elConn->addItem(curConn);
        else
        {
            if(includeChildren)
            {
                //bool aIsInChildren = (_modClassTree->findInDescendants(a,element)!=NULL);
                bool aIsInChildren = portIsInElement(a,elName);

                if(acceptInternal && aIsInChildren)
                    elConn->addItem(curConn);
                else
                {
                    //bool bIsInChildren = (_modClassTree->findInDescendants(b,element)!=NULL);
                    bool bIsInChildren = portIsInElement(b,elName);
                    bool isConcerned = (aIsInChildren || bIsInChildren);
                    bool isInternal = (aIsInChildren && bIsInChildren);

                    if(isConcerned && !(isInternal&&!acceptInternal))
                        elConn->addItem(curConn);
                }
            }
        }
    }
    return elConn;
}




//void ModelicaConnections::getOutside(ModItem* element,bool includeChildren,QList<ModItem*> &_ports,QList<ModItem*> &_outsideComps)
//{
//	ModelicaConnections* elConn = filter(element,includeChildren,false);
//
//	ModItem* aModItem,*bModItem;
//	QString aName,bName;
//	for(int i=0;i<elConn->size();i++)
//	{
//		aName = elConn->at(i)->getA();
//		aModItem = modLoader->findInDescendants(element,aName);
//		
//		bName = elConn->at(i)->getB();
//		bModItem = modLoader->findInDescendants(element,bName);
//		
//
//		if(aModItem)
//		{
//			_ports.push_back(aModItem);
//			_outsideComps.push_back(bModItem);
//		}
//		else
//		{
//			_ports.push_back(bModItem);
//			_outsideComps.push_back(aModItem);
//		}
//	}
//}

void ModelicaConnections::getOutside(ModItem* element,bool includeChildren,QStringList &ports, QStringList &outsideComps)
{
    QString aName;
    QString bName;

    ModelicaConnections* elConn = filter(element,includeChildren,false);

    ports.clear();
    outsideComps.clear();

    for(int i=0;i<elConn->size();i++)
    {
        aName = elConn->at(i)->getA();
        bool aIsInside = portIsInElement(aName,element->name(Modelica::FULL));

        bName = elConn->at(i)->getB();
        bool bIsInside = portIsInElement(bName,element->name(Modelica::FULL));

        if(aIsInside && ! bIsInside)
        {
            ports.push_back(aName);
            outsideComps.push_back(bName);
        }

        if(!aIsInside && bIsInside)
        {
            ports.push_back(bName);
            outsideComps.push_back(aName);
        }
    }
}

void ModelicaConnections::getOutside(ModItem* element,bool includeChildren,QStringList &uniquePorts, QList<QStringList> &outsideComps)
{
    QStringList listPorts;
    QStringList listOutsideComps;

    getOutside(element,includeChildren,listPorts,listOutsideComps);

    QMultiMap<QString,QString> map;
    for(int i=0;i<listPorts.size();i++)
    {
        map.insert(listPorts.at(i),listOutsideComps.at(i));
    }

    uniquePorts = map.uniqueKeys();
    outsideComps.clear();
    for(int i=0;i<uniquePorts.size();i++)
    {
        outsideComps.push_back(map.values(uniquePorts.at(i)));
    }
}

//QList<ModItem*> ModelicaConnections::getOutsideConnectedComps(ModItem* element,bool includeChildren)
//{
//    ModelicaConnections* elConns= filter(element,includeChildren,false);
//    QList<ModItem*> connectedComps ;

//    ModItem *aModItem,*bModItem;
//    QString aName,bName;


//    // looking for connections from itself
//    for(int iConn=0;iConn<elConns->size();iConn++)
//    {
//        aName = elConns->at(iConn)->getA();
//        bName = elConns->at(iConn)->getB();


//        aModItem = _modClassTree->findInDescendants(aName,element);
//        bModItem = _modClassTree->findInDescendants(bName,element);

//        if(aModItem)
//            connectedComps.push_back(bModItem);
//        else
//            connectedComps.push_back(aModItem);
//    }
//    return connectedComps;
//}

//QStringList ModelicaConnections::getOutsideConnectedCompsNames(ModItem* element,bool includeChildren,Modelica::NameFormat format)
//{
//    QList<ModItem*> connectedComps = getOutsideConnectedComps(element,includeChildren);
//    QStringList names;
//    for(int i=0;i<connectedComps.size();i++)
//    {
//        names.push_back(connectedComps.at(i)->name(format));
//    }
//    return names;
//}

//QList<ModItem*> ModelicaConnections::getOutsideConnectingPorts(ModItem* element,bool includeChildren)
//{
//    ModelicaConnections* elConns= filter(element,includeChildren,false);

//    QList<ModItem*> connectedComps ;
//    ModItem *aModItem,*bModItem;
//    QString aName,bName;


//    // looking for connections from itself
//    for(int iConn=0;iConn<elConns->size();iConn++)
//    {
//        aName = elConns->at(iConn)->getA();
//        bName = elConns->at(iConn)->getB();
//        aModItem = _modClassTree->findInDescendants(aName,element);
//        bModItem = _modClassTree->findInDescendants(bName,element);
//        if(aModItem)
//            connectedComps.push_back(aModItem);
//        else
//            connectedComps.push_back(bModItem);
//    }
//    return connectedComps;

//}

//QStringList ModelicaConnections::getOutsideConnectingPortsNames(ModItem* element,bool includeChildren,Modelica::NameFormat format)
//{
//    QList<ModItem*> connectedComps = getOutsideConnectingPorts(element,includeChildren);
//    QStringList names;
//    for(int i=0;i<connectedComps.size();i++)
//    {
//        names.push_back(connectedComps.at(i)->name(format));
//    }
//    return names;
//}

bool ModelicaConnections::portIsInElement(QString portName,QString elementName)
{

    return portName.contains(QRegExp("^"+elementName+"[$|\\.]"));
}


