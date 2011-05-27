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

@file OpenModelica.cpp
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version


*/



#include "OpenModelica.h"




OpenModelica::OpenModelica()
{

}

OpenModelica::~OpenModelica(void)
{
}

bool OpenModelica::compile(MOomc *_omc,QString moPath,QString modelToConsider,QString storeFolder)
{
	// check if model already loaded
	QString loadedMoPath = _omc->getFileOfClass(modelToConsider);

	bool loadOk;
	QString loadError;
	// if not already loaded, reload
	if(loadedMoPath.compare(moPath))
	{
		_omc->loadModel(moPath,true,loadOk,loadError);
	}

	// Create OM script
	QString filePath = storeFolder+QDir::separator()+"MOFirstRun.mos";
	QFile file(filePath);
	if(file.exists())
	{
		file.remove();
	}
	file.open(QIODevice::WriteOnly);

	QString scriptText;
	scriptText.append("cd(\""+storeFolder+"\");\n");
	scriptText.append("simulate("+modelToConsider+",outputFormat=\"csv\");\n");

	QTextStream ts( &file );
	ts << scriptText;
	file.close();

        // delete previous model .exe
    QFile modelExeFile(storeFolder+QDir::separator()+modelToConsider+".exe");
	if(modelExeFile.exists())
		modelExeFile.remove();

	// Run script
	_omc->runScript(filePath);

	//look if it succeed
	bool success = modelExeFile.exists();
	return success; 
}

void OpenModelica::getInputVariablesFromFile(MOomc *_omc,QString filePath, MOVector<Variable> * variables,QString _modelName)
{
	variables->clear();
	QFileInfo fileinfo = QFileInfo(filePath);

	if (fileinfo.exists())
	{
		QFile file(fileinfo.filePath());
		file.open(QIODevice::ReadOnly);
		QTextStream* in = new QTextStream(&file);
		getInputVariablesFromFile(_omc,in, variables,_modelName);		
		file.close();
	}
}
void OpenModelica::getInputVariablesFromFile(MOomc *_omc,QTextStream * text, MOVector<Variable> * variables,QString _modelName)
{
	variables->clear();
	QString line;
	QStringList linefields;

	Variable *newVariable;
	text->seek(0);

	// Get variables' names
	line = text->readLine();
	QRegExp rx("(\\S)+\\s*//(\\S+)\\s*$");
	QStringList fields;
	while (!line.isEmpty()){
		if(rx.indexIn(line)>-1)
		{
			fields = rx.capturedTexts();
			newVariable = new Variable();
			newVariable->setName(_modelName+"."+rx.cap(2));
			newVariable->setValue(rx.cap(1).toDouble());
			variables->addItem(newVariable);
			//get datatype
			newVariable->setDataType(_omc->getPrimitiveDataType(newVariable->name()));
		}
		line=text->readLine();
	} 
}

bool OpenModelica::getFinalVariablesFromFile(QString fileName_, MOVector<Variable> *variables,QString _modelName)
{
	variables->clear();
	QFileInfo fileinfo = QFileInfo(fileName_);

	if (fileinfo.exists())
	{
		QFile file(fileinfo.filePath());
		file.open(QIODevice::ReadOnly);
		QTextStream* in = new QTextStream(&file);
		bool ok = getFinalVariablesFromFile(in, variables,_modelName);		
		file.close();
		delete in;
		return ok;
	}
	else
		return true;
}

bool OpenModelica::getFinalVariablesFromFile(QTextStream *text, MOVector<Variable> * variables,QString _modelName)
{
	variables->clear();
	QString line;
	QStringList varNames;
	QStringList varValues;

	Variable *newVar;
	text->seek(0);
	QString str = text->readLine(); 

	str.remove("\"");
	varNames = str.split(",",QString::SkipEmptyParts);
	
	while(!text->atEnd())
		line=text->readLine();

	varValues = line.split(",",QString::SkipEmptyParts);

	if(varValues.size()!=varNames.size())
		return false;

	for(int i=0;i<varValues.size();i++)
	{
		newVar = new Variable();
		newVar->setName(_modelName+"."+varNames.at(i));
		newVar->setValue(varValues.at(i).toDouble());
		variables->addItem(newVar);
	}
	return true;
}

void OpenModelica::setInputVariables(QString fileName_, MOVector<Variable> *variables,QString _modModelName,MOVector<ModModelParameter> *parameters)
{
	QFileInfo fileinfo = QFileInfo(fileName_);
	if (fileinfo.exists())
	{
		QFile file(fileinfo.filePath());
		file.open(QIODevice::ReadOnly);
		QTextStream textRead(&file);
		QString allText = textRead.readAll();
		file.close();
		// change variable values
		QRegExp rxLine;
		int index=0;
		QString newLine;
		QString varName;
		int iCurVar;
		Variable* curVar;
		QStringList fields;
	
		for(int iV=0;iV<variables->items.size();iV++)
		{
			curVar = variables->items.at(iV);
			varName = curVar->name(Modelica::FULL);
			varName = varName.remove(_modModelName+".");
			rxLine.setPattern(sciNumRx()+"\\s*(//[\\w*|\\s*]*//|//)\\s*"+varName);
			index = rxLine.indexIn(allText);

			if(index>-1)
			{
				fields = rxLine.capturedTexts();
				newLine = curVar->getFieldValue(Variable::VALUE).toString() +"\t";
				newLine += fields.at(2)+varName;
				allText = allText.replace(rxLine.cap(0),newLine);
				qDebug(newLine.toLatin1().data());
			}
			else
			{
				infoSender.send(Info("Warning : unable to set variable value (not found in init file):"+varName,ListInfo::ERROR2));
			}
		}

		// Parameters
		if(parameters)
		{
			QVariant paramValue;
			QString paramName;
			ModModelParameter * curParam;
			for(int iP=0;iP<parameters->items.size();iP++)
			{
				curParam = parameters->items.at(iP);
				paramName = curParam->name();
				paramValue = curParam->getFieldValue(ModModelParameter::VALUE);
				rxLine.setPattern(sciNumRx()+"\\s*(//[\\w*|\\s*]*//|//)\\s*"+paramName);
				index = rxLine.indexIn(allText);

				if(index>-1)
				{
					fields = rxLine.capturedTexts();
					newLine = paramValue.toString() +"\t";
					newLine += fields.at(2)+paramName;
					allText = allText.replace(rxLine.cap(0),newLine);
					qDebug(newLine.toLatin1().data());
				}
				else
				{
					infoSender.send(Info("Warning : unable to set parameter value (not found in init file):"+paramName,ListInfo::ERROR2));
				}
			}
		}


		fileinfo.setFile(fileName_);
		file.setFileName(fileinfo.filePath());
		bool ok = file.open(QIODevice::WriteOnly);
		QTextStream textWrite(&file);
		textWrite<<allText;
		file.close();
	}
}

void OpenModelica::start(QString exeFile)
{
#ifdef WIN32
	QFileInfo exeFileInfo(exeFile);
	QString exeDir = exeFileInfo.absolutePath();
    bool cdOk = SetCurrentDirectory(exeDir.utf16());


    QString appPath = "\""+exeFile+"\"";

       
	


        QProcess simProcess;
        // add OM path in PATH
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString omHome = env.value("OpenModelicaHome");
        omHome = omHome+QDir::separator()+"bin";
        env.insert("PATH", env.value("Path") + ";"+omHome);
        simProcess.setProcessEnvironment(env);

        //start process
    simProcess.start(appPath, QStringList());
    bool ok = simProcess.waitForFinished(-1);
        if(!ok)
        {
        QString msg("CreateProcess failed (%d).\n");
        msg.sprintf(msg.toLatin1().data(),GetLastError());
            infoSender.debug(msg);
            return;
        }


#endif
}

QString OpenModelica::sciNumRx()
{
	QString rx = "([+-]?[0-9]*\\.?[0-9]+|[+-]?[0-9]+\\.?[0-9]*[eE][+-]?[0-9]+)";
	return rx;
}


QString OpenModelica::home()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString omHome = env.value("OpenModelicaHome");
    return omHome;
}
