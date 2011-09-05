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
#ifdef WIN32
    QFile modelExeFile(storeFolder+QDir::separator()+modelToConsider+".exe");
#else
    QFile modelExeFile(storeFolder+QDir::separator()+modelToConsider);
#endif
    if(modelExeFile.exists())
        modelExeFile.remove();

    // Run script
    _omc->runScript(filePath);

    //look if it succeed
    bool success = modelExeFile.exists();
    return success;
}

void OpenModelica::getInputVariablesFromTxtFile(MOomc *_omc,QString filePath, MOVector<Variable> * variables,QString _modelName)
{
    variables->clear();
    QFileInfo fileinfo = QFileInfo(filePath);

    if (fileinfo.exists())
    {
        QFile file(fileinfo.filePath());
        file.open(QIODevice::ReadOnly);
        QTextStream* in = new QTextStream(&file);
        getInputVariablesFromTxtFile(_omc,in, variables,_modelName);
        file.close();
    }
}
void OpenModelica::getInputVariablesFromTxtFile(MOomc *_omc,QTextStream * text, MOVector<Variable> * variables,QString _modelName)
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

bool OpenModelica::getInputVariablesFromXmlFile(MOomc *omc,QString filePath, QString modModelName, MOVector<Variable> * variables)
{
    variables->clear();
    QString error;

    QDomDocument doc;
    QFile file(filePath);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        // infoSender.send( Info(ListInfo::PROBLEMFILENOTEXISTS,filePath));
        return false;
    }
    else if( !doc.setContent(&file,&error) )
    {
        file.close();
        // infoSender.send( Info(ListInfo::PROBLEMFILECORRUPTED,error,filePath));
        return false;
    }
    file.close();

    // create problem
    return getInputVariablesFromXmlFile(omc,doc,modModelName,variables);


}
bool OpenModelica::getInputVariablesFromXmlFile(MOomc *omc,const QDomDocument & doc , QString modModelName,MOVector<Variable> * variables)
{
    variables->clear();

    QDomElement xfmi = doc.firstChildElement("fmiModelDescription");
    QDomElement xModelVars = xfmi.firstChildElement("ModelVariables");
    QDomNodeList listScalarVars = xModelVars.elementsByTagName("ScalarVariable");

    bool ok;
    for(int i=0;i<listScalarVars.size();i++)
    {
        Variable* newVar = variableFromFmi(listScalarVars.at(i).toElement(),modModelName,ok);
        if(ok)
            variables->addItem(newVar);
    }


}

Variable* OpenModelica::variableFromFmi(const QDomElement & el,QString modModelName,  bool & ok)
{
    ok = true;
    QString name = el.attribute("name");
    if(name.isEmpty())
    {
        ok = false;
        return NULL;
    }

    Variable* newVar = new Variable();
    newVar->setName(modModelName+"."+name);
    newVar->setDescription(el.attribute("description"));

    //look for type
    QDomElement elType = el.firstChildElement("Real");
    if(!elType.isNull())
    {
        newVar->setDataType(OMREAL);
        newVar->setValue(elType.attribute("start").toDouble(&ok));
    }
    elType = el.firstChildElement("Integer");
    if(!elType.isNull())
    {
        newVar->setDataType(OMINTEGER);
        newVar->setValue(elType.attribute("start").toInt(&ok));
    }
    elType = el.firstChildElement("Boolean");
    if(!elType.isNull())
    {
        newVar->setDataType(OMBOOLEAN);
        newVar->setValue(elType.attribute("start")=="true");
    }

    if(!ok)
    {
        delete newVar;
        return NULL;
    }
    else
        return newVar;
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

void OpenModelica::setInputVariablesTxt(QString fileName, MOVector<Variable> *variables,QString modModelName,MOParameters *parameters)
{
    QFileInfo fileinfo = QFileInfo(fileName);
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
	
        for(int iV=0;iV<variables->size();iV++)
        {
            curVar = variables->at(iV);
            varName = curVar->name(Modelica::FULL);
            varName = varName.remove(modModelName+".");
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

        // Parameters to write in init file

        if(parameters)
        {
            QList<OMParameters> initParameters; // parameters to specify in init file
            initParameters << STOPVALUE;


            QVariant paramValue;
            QString paramName;
            MOParameter * curParam;
            int iP;
            for(int i=0;i<initParameters.size();i++)
            {
                iP = parameters->findItem((int)initParameters.at(i),MOParameter::INDEX);
                if(iP>-1)
                {
                    curParam = parameters->at(iP);
                    paramName = curParam->name();
                    paramValue = curParam->getFieldValue(MOParameter::VALUE);
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
        }


        fileinfo.setFile(fileName);
        file.setFileName(fileinfo.filePath());
        bool ok = file.open(QIODevice::WriteOnly);
        QTextStream textWrite(&file);
        textWrite<<allText;
        file.close();
    }
}

bool OpenModelica::setInputXml(QString fileName, MOVector<Variable> *variables, QString modelName, MOParameters* parameters)
{
    QString error;
    bool ok;
    QDomDocument doc;
    QFile file(fileName);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        // infoSender.send( Info(ListInfo::PROBLEMFILENOTEXISTS,filePath));
        return false;
    }
    else if( !doc.setContent(&file,&error) )
    {
        file.close();
        // infoSender.send( Info(ListInfo::PROBLEMFILECORRUPTED,error,filePath));
        return false;
    }

    // updates variables
    setInputVariablesXml(doc,modelName,variables);


    // updates parameters
    if(parameters)
        setInputParametersXml(doc,parameters);

    //Writing in .min file
    if(file.exists())
    {
        file.remove();
    }
    ok = ok && file.open(QIODevice::WriteOnly);
    QTextStream ts( &file );
    ts << doc.toString();
    file.close();

    return ok;
}

void OpenModelica::setInputVariablesXml(QDomDocument & doc, QString modelName, MOVector<Variable> *variables)
{
    QDomElement xfmi = doc.firstChildElement("fmiModelDescription");
    QDomElement oldxfmi = xfmi;

    QDomElement xModelVars = xfmi.firstChildElement("ModelVariables");
    QDomElement oldxModelVars = xModelVars;

    QDomNodeList listScalarVars = xModelVars.elementsByTagName("ScalarVariable");


    // filling map
    QMap<QString,int> mapScalarVars; //<name,index in listScalarVars>
    QMap<QDomElement,QDomElement> mapNewScalarVars; // <old node,new node>
    QDomElement curVar;
    QDomElement oldVar;
    QDomElement newVar;
    int index;
    QDomElement oldType;
    QDomElement newType;
    QString localVarName;

    // create map for index looking
    for(int i=0;i<listScalarVars.size();i++)
    {
        curVar = listScalarVars.at(i).toElement();
        mapScalarVars.insert(curVar.attribute("name"),i);
    }

    // change variables values
    for(int i=0;i<variables->size();i++)
    {
        // getting local var name (name in init file does not contain model name)
        localVarName = variables->at(i)->name(Modelica::FULL);
        localVarName = localVarName.remove(modelName+".");

        index = mapScalarVars.value(localVarName,-1);
        if(index>-1)
        {
            oldVar = listScalarVars.at(index).toElement();
            newVar = oldVar;

            oldType = newVar.firstChildElement("Real");
            if(oldType.isNull())
                oldType = newVar.firstChildElement("Integer");
            if(oldType.isNull())
                oldType = newVar.firstChildElement("Boolean");

            if(!oldType.isNull())
            {
                newType = oldType;
                newType.setAttribute("start",variables->at(i)->value());
                newVar.replaceChild(newType,oldType);
                xModelVars.replaceChild(newVar,oldVar);
            }
            xModelVars.replaceChild(newVar,oldVar);
        }
    }

    // update xfmi with new vars
    xfmi.replaceChild(xModelVars,oldxModelVars);
    doc.replaceChild(xfmi,oldxfmi);
}


void OpenModelica::setInputParametersXml(QDomDocument &doc,MOParameters *parameters)
{




}


void OpenModelica::start(QString exeFile,int maxnsec)
{

    QFileInfo exeFileInfo(exeFile);
    QString exeDir = exeFileInfo.absolutePath();

    QProcess simProcess;
    simProcess.setWorkingDirectory(exeDir);


#ifdef WIN32
    QString appPath = "\""+exeFile+"\"";
    // add OM path in PATH
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString omHome = env.value("OpenModelicaHome");
    omHome = omHome+QDir::separator()+"bin";
    env.insert("PATH", env.value("Path") + ";"+omHome);
    simProcess.setProcessEnvironment(env);


    //start process
    simProcess.start(appPath, QStringList());
#else
    QStringList arguments;
    arguments << "-c";
    arguments << "./"+exeFileInfo.fileName() << " > log.txt";
    simProcess.start("sh", arguments);
#endif

    int nmsec;
    if(maxnsec==-1)
        nmsec = -1;
    else
        nmsec = maxnsec*1000;
    bool ok = simProcess.waitForFinished(nmsec);
    if(!ok)
    {
        QString msg("CreateProcess failed (%d).");
        infoSender.debug(msg);
        return;
    }


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
