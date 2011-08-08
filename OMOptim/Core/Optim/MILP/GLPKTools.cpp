// $Id: GLPKTools.cpp 9418 2011-07-06 11:44:40Z hubert.thieriot $
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

        @file GLPKTools.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
  */

#include "GLPKTools.h"

	
QString GlpkTools::listToSet(QString setName, QStringList list)
{
	// create a textline set (mathprog language) from a list of values
	QString result;
	result += "set " + setName + ":= ";

	for(int i=0;i<list.size();i++)
	{
		result += list.at(i)+" ";
	}

	result += "; \n";

	return result;
}

QString GlpkTools::listToSet(QString setName, QList<double> _list)
{
	// create a textline set (mathprog language) from a list of values
	QStringList listStr;
	
	for(int i=0;i<_list.size();i++)
	{
		listStr.push_back(QString::number(_list.at(i)));
	}

	return listToSet(setName,listStr);
}


void GlpkTools::fill(MilpVariableResult0D & var, glp_prob * glpProblem,double defaultValue,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    // read value
    int iCol=colNames.indexOf(var.name(),0);
    if(iCol>-1)
        var.setValue(glp_mip_col_val(glpProblem,iCol+1)); //iCol+1 since glp cols start at 1.
    else
        var.setValue(defaultValue);

}

void GlpkTools::fill(MilpVariableResult1D & var, glp_prob * glpProblem,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    QRegExp regExp(var.name() + "\\[([\\w|\.|\\d]+)\\][.]*");


    double curValue;
    QString curName;

    int iCol=colNames.indexOf(regExp,0);
    while(iCol>-1)
    {
        curName = colNames.at(iCol);
        curName.remove(var.name());
        curName.remove("[");
        curName.remove("\'");
        curName.remove("]");
        curName.remove(QRegExp("^\\."));
        curValue = glp_mip_col_val(glpProblem, iCol+1); //iCol+1 since glp cols start at 1.
        var.insertValue(curName,curValue);
        iCol = colNames.indexOf(regExp,iCol+1);
    }

}

void GlpkTools::fill(MilpVariableResult2D & var, glp_prob * glpProblem,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    // read Qijk
    QRegExp regExp(var.name()+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][.]*");

    QString i1,i2;
    int iCol=colNames.indexOf(regExp,0);
    QString curColName;
    double curValue;
    while(iCol>-1)
    {
        curColName = colNames.at(iCol);
        if ((regExp.indexIn(curColName)>-1)&&(regExp.captureCount()==2))
        {
            i1 = regExp.cap(1);
            i2 = regExp.cap(2);

            curValue = glp_mip_col_val(glpProblem,iCol+1); //iCol+1 since glp cols start at 1

            var.insertValue(MilpKey2D(i1,i2),curValue);

        }
        iCol = colNames.indexOf(regExp,iCol+1);
    }

}

void GlpkTools::fill(MilpVariableResult3D & var, glp_prob * glpProblem,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    // read Qijk
    QRegExp regExp(var.name()+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][.]*");

    QString i1,i2,i3;
    int iCol=colNames.indexOf(regExp,0);
    QString curColName;
    double curValue;
    while(iCol>-1)
    {
        curColName = colNames.at(iCol);
        if ((regExp.indexIn(curColName)>-1)&&(regExp.captureCount()==3))
        {
            i1 = regExp.cap(1);
            i2 = regExp.cap(2);
            i3 = regExp.cap(3);

            curValue = glp_mip_col_val(glpProblem,iCol+1); //iCol+1 since glp cols start at 1

            var.insertValue(MilpKey3D(i1,i2,i3),curValue);

        }
        iCol = colNames.indexOf(regExp,iCol+1);
    }

}

void GlpkTools::fill(MilpVariableResult4D & var, glp_prob * glpProblem,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    // read Qijk
    QRegExp regExp(var.name()+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][.]*");

    QString i1,i2,i3,i4;
    int iCol=colNames.indexOf(regExp,0);
    QString curColName;
    double curValue;
    while(iCol>-1)
    {
        curColName = colNames.at(iCol);
        if ((regExp.indexIn(curColName)>-1)&&(regExp.captureCount()==4))
        {
            i1 = regExp.cap(1);
            i2 = regExp.cap(2);
            i3 = regExp.cap(3);
            i4 = regExp.cap(4);

            curValue = glp_mip_col_val(glpProblem,iCol+1); //iCol+1 since glp cols start at 1

            //_values.insert(MilpKey4D(i1,i2,i3,i4),1.1);
            var.insertValue(MilpKey4D(i1,i2,i3,i4),curValue);
        }
        iCol = colNames.indexOf(regExp,iCol+1);
    }

}

QStringList GlpkTools::getColNames(glp_prob *glpProblem)
{
    QStringList colNames;

    // read all column names
    int nbCols = glp_get_num_cols(glpProblem);
    for(int iCol = 1;iCol<=nbCols; iCol++)
    {
        colNames.push_back(QString(glp_get_col_name(glpProblem, iCol)));
    }
    return colNames;
}

