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

        @file CSV.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version 0.9

  */
#include "CSV.h"

// below include must be in .cpp, not in header file
// to avoid circular references
#include "Variable.h"
#include "MOOptVector.h"
#include "OptObjective.h"

CSV::CSV(void)
{
}

CSV::~CSV(void)
{
}

QString CSV::space()
{
        return "___";
}

QString CSV::variablesToLines(MOVector<Variable> * variables)
{
        QString text = "";
        Variable *curVar;

        for (int i=0;i<variables->items.size();i++)
        {
                curVar=variables->items.at(i);

                //Variable fields
                text += ("#VAR");

                for (int nf=0;nf<Variable::nbFields;nf++)
                {
                        text += "\t";
                        text += curVar->getFieldValue(nf).toString();
                }
                text += "\n";
        }
        return text;
}

void CSV::LinesToVariables(MOVector<Variable>* variables, QString lines)
{
        //Clear variables
        variables->clear();

        QStringList lineList = lines.split("\n");
        QStringList fields;
        Variable *newVariable;
        for(int nl=0;nl<lineList.size();nl++)
        {
                if(!lineList[nl].isEmpty())
                {

                        fields = lineList[nl].split("\t");

                        //Treating only lines begininng with #VAR
                        //left(4) is used to avoid space problems
                        if(fields[0].left(4)=="#VAR")
                        {
                                newVariable = new Variable();
                                for (int nf=0;nf<Variable::nbFields;nf++)
                                {
                                        newVariable->setFieldValue(nf,QVariant(fields[nf+1]));
                                }
                                variables->addItem(newVariable);
                        }
                }
        }
}



QString CSV::oneVariableResultToValueLines(VariableResult * variable)
{
        QString text = "";

        for (int iScan=0;iScan<variable->nbScans();iScan++)
        {
                for(int iPoint=0;iPoint<variable->nbPoints();iPoint++)
                {
                        if(variable->isComputedPoint(iScan,iPoint))
                                text += QString::number(variable->finalValue(iScan,iPoint));
                        else
                                text += "-";

                        text += "\t";
                }
                text += "\n";
        }
        return text;
}
void CSV::linesToOneVariableResultValues(VariableResult * variable,QString text)
{
        QStringList lines = text.split("\n");
        QStringList pointsValues;
        int iScan=0;
        bool computed;

        for(int iLine = 0;iLine<lines.length();iLine++)
        {
                pointsValues = lines.at(iLine).split("\t");
                if(pointsValues.size()>0)
                {
                        for(int iPoint=0;iPoint<pointsValues.size();iPoint++)
                        {
                                computed = (pointsValues.at(iPoint)!="-");
                                variable->setFinalValue(iScan,iPoint,pointsValues.at(iPoint).toDouble(),computed);
                        }
                        iScan++;
                }
        }
}

/**
* \brief
* Creates lines with variables values at iPoint
* Schema :
* var1.name \t var2.name \n
* var1.value(point=iPoint,scan=0) \t var2.value(point=iPoint,scan=0) \n
* var1.value(point=iPoint,scan=1) \t var2.value(point=iPoint,scan=1) \n
*/
QString CSV::variableResultToValueLines(MOOptVector* variables, int iPoint)
{
        QString text = "";
        VariableResult *curVar;

        QStringList titleLine;
        QStringList scanGroups;
        QString curScanLine;
        for(int iScan=0;iScan<variables->nbScans();iScan++)
        {
                scanGroups.append(QString());
        }


        for (int iVar=0;iVar<variables->items.size();iVar++)
        {
                curVar=variables->items.at(iVar);
                titleLine.push_back(curVar->name());

                for(int iScan=0;iScan<variables->nbScans();iScan++)
                {
                        curScanLine = scanGroups.at(iScan);
                        curScanLine += QString::number(curVar->finalValue(iScan,iPoint))+"\t";
                        scanGroups.replace(iScan,curScanLine);
                }
        }
        text = titleLine.join("\t") + "\n";
        for(int iScan=0;iScan<variables->nbScans();iScan++)
        {
                text+= scanGroups.at(iScan)+"\n\n";
        }
        return text;
}

/**
* \brief
* Read lines and set values of corresponding variables at iPoint.
* @param addIfNotFound : if a variable in file is not found in variables, addIfNotFound says
*  if it should be added in variables.
*/
void CSV::linesToVariableResult(MOOptVector * variables, int iPoint, QString lines, bool addIfNotFound)
{

    //Clear variables at iPoint
    variables->clearAtiPoint(iPoint);

    QStringList lineList = lines.split("\n",QString::SkipEmptyParts);
    QStringList varNames;
    QStringList curScanValues;
    VariableResult *newVariableResult;
    int indexVar;
    varNames = lineList.at(0).split("\t",QString::SkipEmptyParts);
    int iScan;
    double value;
    for(int iCol=0;iCol<varNames.size();iCol++)
    {
        indexVar = variables->findItem(varNames.at(iCol));
        if((indexVar==-1) && (addIfNotFound))
        {
            //variable found in file does not exist in _recomputedVariables
            // add it
            newVariableResult =  new VariableResult();
            newVariableResult->setName(varNames.at(iCol));
            variables->addItem(newVariableResult);
            indexVar = variables->findItem(varNames.at(iCol));

            // display info
            QString msg;
            msg.sprintf("Variable %s added in recomputed variables list",
                        newVariableResult->name().utf16());
            infoSender.send(Info(msg,ListInfo::NORMAL2));
        }

        if(indexVar>-1)
        {
            for(int iLine=1;iLine<lineList.size();iLine++)
            {
                curScanValues = lineList.at(iLine).split("\t",QString::SkipEmptyParts);
                if(curScanValues.size()==varNames.size())
                {
                    iScan = iLine-1;
                    variables->items.at(indexVar)->setFinalValue(iScan,iPoint,curScanValues.at(iCol).toDouble(),true);
                }
            }
        }
    }
}




QString CSV::scannedVariablesToLines(MOVector<ScannedVariable> * scannedVariables)
{
        QString text = "";
        ScannedVariable *curVar;

        for (int i=0;i<scannedVariables->items.size();i++)
        {
                curVar=scannedVariables->items.at(i);

                //Variable fields
                text += ("#SCANVAR");

                for (int nf=0;nf<ScannedVariable::nbFields;nf++)
                {
                        text += "\t";
                        text += curVar->getFieldValue(nf).toString();
                }
                text += "\n";
        }
        return text;
}


QString CSV::optObjectivesToLines(MOVector<OptObjective>* objectives)
{
        QString text = "\t ObjectiveName \t Direction (Minimize = 1, Maximize = 0) \t Description -->\n";

        OptObjective *curObj;

        for (int i=0;i<objectives->items.size();i++)
        {
                curObj=objectives->items.at(i);

                //Objective fields
                text += ("#OBJ");

                for (int nf=0;nf<OptObjective::nbFields;nf++)
                {
                        text += "\t";
                        text += curObj->getFieldValue(nf).toString();
                }
                text += "\n";
        }
        return text;
}




void CSV::LinesToScannedVariables(MOVector<ScannedVariable>* scannedVariables, QString lines)
{
        //Clear variables
        scannedVariables->clear();

        QStringList lineList = lines.split("\n");
        QStringList fields;
        ScannedVariable *newScannedVariable;
        for(int nl=0;nl<lineList.size();nl++)
        {
                if(!lineList[nl].isEmpty())
                {

                        fields = lineList[nl].split("\t");

                        //Treating only lines begininng with #VAR
                        //left(4) is used to avoid space problems
                        if(fields[0].left(8)=="#SCANVAR")
                        {
                                newScannedVariable = new ScannedVariable();
                                for (int nf=0;nf<ScannedVariable::nbFields;nf++)
                                {
                                        newScannedVariable->setFieldValue(nf,QVariant(fields[nf+1]));
                                }
                                scannedVariables->addItem(newScannedVariable);
                        }
                }
        }
}


void CSV::FileToVariableResult(MOOptVector *variables, int iPoint, QString fileName, bool addIfNotFound)
{
        QFile frontFile(fileName);
        if(!frontFile.exists())
        {
                // ERROR
                return;
        }
        frontFile.open(QIODevice::ReadOnly);
        QTextStream tsfront( &frontFile );
        QString text = tsfront.readAll();
        frontFile.close();
        linesToVariableResult(variables,iPoint,text,addIfNotFound);
}




void CSV::FileToVariables(MOVector<Variable>* variables, QString fileName)
{
        QFile frontFile(fileName);
        if(!frontFile.exists())
        {
                // #TODO ERROR
        }
        frontFile.open(QIODevice::ReadOnly);
        QTextStream tsfront( &frontFile );
        QString text = tsfront.readAll();
        frontFile.close();
        LinesToVariables(variables, text);

}


void CSV::LinesToOptObjectives(MOVector<OptObjective>* objectives, QString lines)
{
        //Clear variables
        objectives->clear();

        QStringList lineList = lines.split("\n");
        QStringList fields;
        OptObjective *newObjective;

        for(int nl=0;nl<lineList.size();nl++)
        {
                if(!lineList[nl].isEmpty())
                {

                        fields = lineList[nl].split("\t");

                        //Treating only lines begininng with #OBJ
                        //left(4) is used to avoid space problems
                        if(fields[0].left(4)=="#OBJ")
                        {
                                newObjective = new OptObjective();
                                for (int nf=0;(nf<OptObjective::nbFields)&&(nf<fields.size()-1);nf++)
                                {
                                        newObjective->setFieldValue(nf,QVariant(fields[nf+1]));
                                }
                                objectives->addItem(newObjective);
                        }
                }
        }
}

void CSV::LinesToOptObjectives(MOVector<OptObjectiveResult>* objectives, QString lines)
{

        //Clear variables
        objectives->clear();

        QStringList lineList = lines.split("\n");
        QStringList fields;
        OptObjectiveResult *newObjective;

        for(int nl=0;nl<lineList.size();nl++)
        {
                if(!lineList[nl].isEmpty())
                {
                        fields = lineList[nl].split("\t");

                        //Treating only lines begininng with #OBJ
                        //left(4) is used to avoid space problems
                        if(fields[0].left(4)=="#OBJ")
                        {
                                newObjective = new OptObjectiveResult();
                                for (int nf=0;(nf<OptObjective::nbFields)&&(nf<fields.size()-1);nf++)
                                {
                                        newObjective->setFieldValue(nf,QVariant(fields[nf+1]));
                                }
                                objectives->addItem(newObjective);
                        }
                }
        }
}

