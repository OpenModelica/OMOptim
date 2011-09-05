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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file BlockSubstitution.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#include "BlockSubstitution.h"

BlockSubstitution::BlockSubstitution(Project* _project,ModModelPlus* _model,ModClassTree* _modClassTree, QString _orgComponent, QString _subComponent,
									 QStringList _orgPorts,QList<QStringList> _orgConnectedComps,
									 QStringList _subPorts,QList<QStringList> _subConnectedComps)
{
	project = _project;
	model = _model;
        modClassTree = _modClassTree;
	orgComponent = _orgComponent;
	subComponent = _subComponent;
	orgPorts = _orgPorts;
	orgConnectedComps = _orgConnectedComps;
	subPorts = _subPorts;
	subConnectedComps = _subConnectedComps;
}



BlockSubstitution::BlockSubstitution(Project* _project,ModModelPlus* _model, ModelicaConnections* _connections, QString _orgComponent,QString _subComponent,ModClassTree* _modClassTree,bool doAutoConnect, bool &ok)
{

        ok = init(_project,_model,_connections,_orgComponent,_subComponent,_modClassTree);

	if(doAutoConnect)
		autoConnect();
}

BlockSubstitution::BlockSubstitution(Project* _project,ModModelPlus* _model, ModelicaConnections* _connections,QDomElement _domEl,ModClassTree* _modClassTree)
{

	if( _domEl.tagName() != "BlockSubstitution" )
	{
		//sendInfo( Info(ListInfo::PROJECTFILECORRUPTED,filePath));
		return;
	}

	// get component names
	QString _orgComponent;
	QString _subComponent;

	QDomNode n1 = _domEl.firstChild();
	while( !n1.isNull() )
	{
		QDomElement e1 = n1.toElement();
		if( !e1.isNull() )
		{
			if( e1.tagName() == "ReplacedComponent" )
			{
				_orgComponent = e1.attribute("name", "" );
			}

			if( e1.tagName() == "ReplacingComponent" )
			{
				_subComponent = e1.attribute("class", "" );
			}
		}
		n1 = n1.nextSibling();
	}

	//build first version
        bool ok = init(_project,_model,_connections,_orgComponent,_subComponent, _modClassTree);

	// getting replacing connections
	n1 = _domEl.firstChild();
	while( !n1.isNull() )
	{
		QDomElement e1 = n1.toElement();
		if( !e1.isNull() && e1.tagName() == "ReplacingComponent")
		{
			QDomNode n2 = n1.firstChild();
			while( !n2.isNull() )
			{
				QDomElement e2 = n2.toElement();
				if( e2.tagName() == "Connection" )
				{
					QString  _from = e2.attribute("from", "" );
					QString  _to = e2.attribute("to", "" );

					QString _subPort = subComponent+"." + _from;
					
					//looking from corresponding port
					int indexPort = subPorts.indexOf(_subPort);
					if(indexPort>-1)
					{
						QStringList curList = subConnectedComps.at(indexPort);
						curList.append(_to);
						subConnectedComps.replace(indexPort,curList);
					}
				}
				n2 = n2.nextSibling();
			}
		}
		n1 = n1.nextSibling();
	}
}


bool BlockSubstitution::init(Project * _project, ModModelPlus *_model, ModelicaConnections* _connections,QString _orgComponent, QString _subComponent, ModClassTree *_modClassTree)
{
	project = _project;
	model = _model;
        modClassTree = _modClassTree;


	orgComponent = _orgComponent;
	subComponent = _subComponent;


        ModClass* _orgElement = _modClassTree->findInDescendants(orgComponent);
	if(_orgElement==NULL)
	{
                infoSender.debug(orgComponent + " not found.");
		return false;
	}
	_connections->getOutside(_orgElement,true,orgPorts,orgConnectedComps);

	if(!subComponent.isEmpty())
	{
		// reading subcomponent ports
		ModClass* _subElement;
		QString _libraryName = subComponent.section(".",0,0);
		
                        _subElement =  _modClassTree->findInDescendants(subComponent);
			if(_subElement)
			{
                                subPorts = modClassTree->getPorts(_subElement,Modelica::FULL);
				for(int i=0;i<subPorts.size();i++)
				{
					subConnectedComps.push_back(QStringList());
				}
			}
	}
	return true;
}

void BlockSubstitution::setSubComponent(QString _subComponent,bool doAutoConnect)
{
	subComponent = _subComponent;
	subPorts.clear();
	subConnectedComps.clear();

	// reading subcomponent ports
	ModClass* _subElement;
        _subElement = modClassTree->findInDescendants(subComponent);
	
	if(_subElement)
	{
                subPorts = modClassTree->getPorts(_subElement,Modelica::FULL);
		for(int i=0;i<subPorts.size();i++)
		{
			subConnectedComps.push_back(QStringList());
		}
	}
}

BlockSubstitution::~BlockSubstitution(void)
{
}

BlockSubstitution* BlockSubstitution::clone() const
{
	BlockSubstitution* newBSub = new BlockSubstitution(
                project,model,modClassTree,orgComponent, subComponent,
									 orgPorts,orgConnectedComps,
									 subPorts,subConnectedComps);
	return newBSub;
}

void BlockSubstitution::copyFrom(BlockSubstitution *_org)
{
	project = _org->project;
	model = _org->model;
	orgComponent = _org->orgComponent;
	subComponent = _org->subComponent;
	orgPorts= _org->orgPorts;
	orgConnectedComps = _org->orgConnectedComps;
	subPorts = _org->subPorts;
	subConnectedComps = _org->subConnectedComps;
}

void BlockSubstitution::autoConnect()
{
	for(int iPort=0;iPort<subPorts.size();iPort++)
	{
		QString _portName = subPorts.at(iPort);
		
		int index = orgPorts.indexOf(QRegExp(".*"+_portName.section(".",-1,-1)));
		
		if(index>-1)
			subConnectedComps.replace(iPort,orgConnectedComps.at(index));
	}
}

QDomElement BlockSubstitution::toXmlData(QDomDocument & doc)
{

	// Root element
	QDomElement cBlock = doc.createElement("BlockSubstitution");


	// Replaced component
	QDomElement cReplaced = doc.createElement("ReplacedComponent");
	cReplaced.setAttribute("name", orgComponent);
	cBlock.appendChild(cReplaced);

	// Replacing component
	QDomElement cReplacing = doc.createElement("ReplacingComponent");
	cReplacing.setAttribute("class", subComponent);
	
	// Org connections
	for(int i=0; i < subPorts.size(); i++)
	{
		for(int j=0;j<subConnectedComps.at(i).size();j++)
		{
			QDomElement cConnect = doc.createElement("Connection");
			cConnect.setAttribute("from", subPorts.at(i).section(".",-1,-1));
			cConnect.setAttribute("to", subConnectedComps.at(i).at(j));
			cReplacing.appendChild(cConnect);
		}
	}
	cBlock.appendChild(cReplacing);
	

	return cBlock;
}
