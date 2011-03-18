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

 	@file XML.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "XML.h"
#include "Variable.h"

XML::XML(void)
{
}

XML::~XML(void)
{
}

void XML::XMLToVariableResults(MOVector<VariableResult> &variables,QDomElement &element)
{
	variables.clear();

	QDomElement e;
	QDomNode n = element.firstChild();

	QString fieldName;
	int iField;

	while( !n.isNull() )
	{
		e = n.toElement();
		if( !e.isNull() && (e.tagName()=="VariableResult"))
		{
			VariableResult* newVar = new VariableResult();
			QDomNamedNodeMap attributes = e.attributes();
			for(int i=0;i<attributes.count();i++)
			{
				fieldName = attributes.item(i).toAttr().name();
				iField = newVar->getFieldIndex(fieldName);
				if(iField>-1)
					newVar->setFieldValue(iField,QVariant(attributes.item(i).toAttr().value()));

				if(!fieldName.compare("Values"))
				{
					QString text = attributes.item(i).toAttr().value();
					//#TODO Fill values
                                        infoSender.debug("XMLToVariableResults not implemented. Do not use!!");
				}
			}
			variables.addItem(newVar);		
		}
		n = n.nextSibling();
	}
}


QDomElement XML::VariableResultsToXml(MOVector<VariableResult> & vector,QDomDocument & doc,QString listTitle,QString itemTitle)
{
    // Root element
    QDomElement cList = doc.createElement(listTitle);

    for(int i=0;i<vector.items.size();i++)
    {
        QDomElement cItem = doc.createElement(itemTitle);
        for(int iF=0;iF<vector.items.at(i)->getNbFields();iF++)
        {
            if(iF!=VariableResult::VALUE)
                cItem.setAttribute(vector.items.at(i)->getFieldName(iF),vector.items.at(i)->getFieldValue(iF).toString());
        }
        QDomElement _values = doc.createElement("Values");
        QDomText _text = doc.createTextNode(CSV::oneVariableResultToValueLines(vector.items.at(i)));
        _values.appendChild(_text);
        cItem.appendChild(_values);

        cList.appendChild(cItem);
    }
    return cList;
}

void  XML::XMLToVariables(MOVector<Variable> & variables,QDomElement &element)
{
	variables.clear();

	QDomElement e;
	QDomNode n = element.firstChild();

	QString fieldName;
	int iField;

	while( !n.isNull() )
	{
		e = n.toElement();
		if( !e.isNull() && (e.tagName()=="Variable"))
		{
			Variable* newVar = new Variable();
			QDomNamedNodeMap attributes = e.attributes();
			for(int i=0;i<attributes.count();i++)
			{
				iField = newVar->getFieldIndex(attributes.item(i).toAttr().name());
				if(iField>-1)
					newVar->setFieldValue(iField,QVariant(attributes.item(i).toAttr().value()));
			}
			variables.addItem(newVar);		
		}
		n = n.nextSibling();
	}
}
