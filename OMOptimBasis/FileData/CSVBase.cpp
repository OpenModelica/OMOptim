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
  @version

  */
#include "CSVBase.h"

// below include must be in .cpp, not in header file
// to avoid circular references
#include "Variable.h"
#include "MOOptVector.h"


QString CSVBase::space()
{
  return "___";
}

QString CSVBase::variablesToLines(MOVector<Variable> * variables)
{
  QString text = "";
  Variable *curVar;

  for (int i=0;i<variables->size();i++)
  {
          curVar=variables->at(i);

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

void CSVBase::LinesToVariables(MOVector<Variable>* variables, QString lines)
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



QString CSVBase::scannedVariablesToLines(MOVector<ScannedVariable> * scannedVariables)
{
  QString text = "";
  ScannedVariable *curVar;

  for (int i=0;i<scannedVariables->size();i++)
  {
          curVar=scannedVariables->at(i);

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




void CSVBase::LinesToScannedVariables(MOVector<ScannedVariable>* scannedVariables, QString lines)
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




void CSVBase::FileToVariables(MOVector<Variable>* variables, QString fileName)
{
  QFile frontFile(fileName);
  if(!frontFile.exists())
  {
          /// \todo ERROR
  }
  frontFile.open(QIODevice::ReadOnly);
  QTextStream tsfront( &frontFile );
  QString text = tsfront.readAll();
  frontFile.close();
  LinesToVariables(variables, text);

}

