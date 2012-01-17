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
#include "Project.h"

BlockSubstitution::BlockSubstitution(Project* project,ModModelPlus* model, QString orgComponent, QString subComponent,
                                     QStringList orgPorts,QList<QStringList> orgConnectedComps,
                                     QStringList subPorts,QList<QStringList> subConnectedComps)
{
    _project = project;
    _model = model;
    _orgComponent = orgComponent;
    _subComponent = subComponent;
    _orgPorts = orgPorts;
    _orgConnectedComps = orgConnectedComps;
    _subPorts = subPorts;
    _subConnectedComps = subConnectedComps;
}



BlockSubstitution::BlockSubstitution(Project* project,ModModelPlus* model, ModelicaConnections* connections, QString orgComponent,QString subComponent,bool doAutoConnect, bool &ok)
{

    ok = init(project,model,connections,orgComponent,subComponent);

	if(doAutoConnect)
		autoConnect();
}

BlockSubstitution::BlockSubstitution(Project* project,ModModelPlus* model, ModelicaConnections* connections,QDomElement domEl)
{

    if( domEl.tagName() != "BlockSubstitution" )
	{
		//sendInfo( Info(ListInfo::PROJECTFILECORRUPTED,filePath));
		return;
	}

	// get component names
    QString orgComponent;
    QString subComponent;

    QDomNode n1 = domEl.firstChild();
	while( !n1.isNull() )
	{
		QDomElement e1 = n1.toElement();
		if( !e1.isNull() )
		{
			if( e1.tagName() == "ReplacedComponent" )
			{
                orgComponent = e1.attribute("name", "" );
			}

			if( e1.tagName() == "ReplacingComponent" )
			{
                subComponent = e1.attribute("class", "" );
			}
		}
		n1 = n1.nextSibling();
	}

	//build first version
    bool ok = init(project,model,connections,orgComponent,subComponent);

	// getting replacing connections
    n1 = domEl.firstChild();
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
                    QString  from = e2.attribute("from", "" );
                    QString  to = e2.attribute("to", "" );

                    QString subPort = subComponent+"." + from;

					//looking from corresponding port
                    int indexPort = _subPorts.indexOf(subPort);
					if(indexPort>-1)
					{
                        QStringList curList = _subConnectedComps.at(indexPort);
                        curList.append(to);
                        _subConnectedComps.replace(indexPort,curList);
					}
				}
				n2 = n2.nextSibling();
			}
		}
		n1 = n1.nextSibling();
	}
}


bool BlockSubstitution::init(Project * project, ModModelPlus *model, ModelicaConnections* connections,QString orgComponent, QString subComponent)
{
    _project = project;
    _model = model;
    _orgComponent = orgComponent;
    _subComponent = subComponent;


    ModItem* orgElement = project->modClassTree()->findInDescendants(_orgComponent);
    if(orgElement==NULL)
	{
        InfoSender::instance()->debug(_orgComponent + " not found.");
		return false;
	}
    connections->getOutside(orgElement,true,_orgPorts,_orgConnectedComps);

	if(!subComponent.isEmpty())
	{
		// reading subcomponent ports
        ModItem* subElement;
        QString libraryName = _subComponent.section(".",0,0);

        subElement =  project->modClassTree()->findInDescendants(_subComponent);
        if(subElement)
			{
            _subPorts = _project->modClassTree()->getPorts(subElement,Modelica::FULL);
            for(int i=0;i<_subPorts.size();i++)
				{
                _subConnectedComps.push_back(QStringList());
				}
			}
	}
	return true;
}

void BlockSubstitution::setSubComponent(QString subComponent,bool doAutoConnect)
{
    _subComponent = subComponent;
    _subPorts.clear();
    _subConnectedComps.clear();

	// reading subcomponent ports
    ModItem* subElement;
    subElement = _project->modClassTree()->findInDescendants(_subComponent);

    if(subElement)
	{
        _subPorts = _project->modClassTree()->getPorts(subElement,Modelica::FULL);
        for(int i=0;i<_subPorts.size();i++)
		{
            _subConnectedComps.push_back(QStringList());
		}
	}
}

BlockSubstitution::~BlockSubstitution(void)
{
}

BlockSubstitution* BlockSubstitution::clone() const
{
	BlockSubstitution* newBSub = new BlockSubstitution(
                _project,_model,_orgComponent, _subComponent,
                _orgPorts,_orgConnectedComps,
                _subPorts,_subConnectedComps);
	return newBSub;
}

void BlockSubstitution::copyFrom(BlockSubstitution *org)
{
    _project = org->_project;
    _model = org->_model;
    _orgComponent = org->_orgComponent;
    _subComponent = org->_subComponent;
    _orgPorts= org->_orgPorts;
    _orgConnectedComps = org->_orgConnectedComps;
    _subPorts = org->_subPorts;
    _subConnectedComps = org->_subConnectedComps;
}

void BlockSubstitution::autoConnect()
{
    for(int iPort=0;iPort<_subPorts.size();iPort++)
	{
        QString portName = _subPorts.at(iPort);

        int index = _orgPorts.indexOf(QRegExp(".*"+portName.section(".",-1,-1)));

		if(index>-1)
            _subConnectedComps.replace(iPort,_orgConnectedComps.at(index));
	}
}

QDomElement BlockSubstitution::toXmlData(QDomDocument & doc)
{

	// Root element
	QDomElement cBlock = doc.createElement("BlockSubstitution");


	// Replaced component
	QDomElement cReplaced = doc.createElement("ReplacedComponent");
    cReplaced.setAttribute("name", _orgComponent);
	cBlock.appendChild(cReplaced);

	// Replacing component
	QDomElement cReplacing = doc.createElement("ReplacingComponent");
    cReplacing.setAttribute("class", _subComponent);

	// Org connections
    for(int i=0; i < _subPorts.size(); i++)
	{
        for(int j=0;j<_subConnectedComps.at(i).size();j++)
		{
			QDomElement cConnect = doc.createElement("Connection");
            cConnect.setAttribute("from", _subPorts.at(i).section(".",-1,-1));
            cConnect.setAttribute("to", _subConnectedComps.at(i).at(j));
			cReplacing.appendChild(cConnect);
		}
	}
	cBlock.appendChild(cReplacing);


	return cBlock;
}
