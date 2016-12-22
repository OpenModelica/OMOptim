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

     @file MOOptVector.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */

#ifdef WIN32
#include "windows.h"
#endif

//STD Headers
#include <iostream>
#include <stdexcept>

#include "MOomc.h"
#include "OMCThreads.h"
#include "MOSettings.h"

//IAEX Headers
#include "omcinteractiveenvironment.h"
#include "MOThreads.h"
#include "LowTools.h"
#include "SleeperThread.h"
#include "omc_config.h"
#include "Utilities.h"
#include "StringHandler.h"
#include "ComponentInfo.h"

#include <QApplication>

class Project;

using namespace std;


MOomc::MOomc(QString appName,bool start)
{
    nbCalls = 0;
    mName = appName;
    //delegate_ = 0;
    mHasInitialized= false;
    // isStarted = false;
    omc_version_ = "(version)";

    InfoSender::instance()->send( Info("OMC: attempt to start",ListInfo::OMCNORMAL2));

    if (start)
    {
        startServer();
    }

}

MOomc::~MOomc()
{
    stopServer();
    //delete delegate_;
}

// threads managment
void MOomc::addUsingThread(QThread* _thread,QString _threadName)
{
    threads.append(_thread);
    threadsNames.append(_threadName);
    emit startOMCThread(_threadName);
}
void MOomc::removeUsingThread(QThread* _thread)
{
    int index = threads.indexOf(_thread);
    if(index>-1)
    {
        emit finishOMCThread(threadsNames.at(index));
        threads.removeAt(index);
        threadsNames.removeAt(index);
    }
}

QList<QThread*> MOomc::getThreads()
{
    return threads;
}

QStringList MOomc::getThreadsNames()
{
    return threadsNames;
}

// Modelica functions

QStringList MOomc::getClassNames(QString parentClass)
{
    QString commandRes= evalCommand("getClassNames("+parentClass+")");
    if(commandRes.contains("error",Qt::CaseInsensitive))
        return QStringList();
    else
    {
        commandRes.remove("{");
        commandRes.remove("}");
        return commandRes.split(",",QString::SkipEmptyParts);
    }
}

QString MOomc::getText(QString className)
{
    QString moTxt = evalCommand("list("+className+")");
    moTxt.remove(QRegExp("^([\\s|\\\"]+)"));
    moTxt.remove(QRegExp("([\\s|\\\"]+)$"));
    return moTxt;
}

QString MOomc::getWholeText(bool includeMSL)
{
    QString moTxt;
    if(includeMSL)
    {
        moTxt = evalCommand("list()");
    }
    else
    {
        QStringList classLoaded = getClassNames();
        classLoaded.removeAll("Modelica");

        for(int i=0;i<classLoaded.size();i++)
        {
            moTxt += getText(classLoaded.at(i));
            moTxt +="\n \n";
        }
    }

    moTxt.replace("\\\\","\\");
    moTxt.replace("\\\"","\"");
    moTxt.remove(QRegExp("^([\\s|\\\"]+)"));
    moTxt.remove(QRegExp("([\\s|\\\"]+)$"));

    return moTxt;

}


QStringList MOomc::getPackages(QString parentClass)
{
    QString commandRes= evalCommand("getPackages(" + parentClass +")");

    if(commandRes.contains("error",Qt::CaseInsensitive))
    {
        //InfoSender::instance()->send( Info(ListInfo::OMSGETCOMPERROR,parentClass));
        return QStringList();

    }
    commandRes.remove("{");
    commandRes.remove("}");

    QStringList list = commandRes.split(",");

    if(list.at(0)=="")
    {
        list.clear();
    }
    return list;
}
QStringList MOomc::getRecords(QString parentClass)
{
    QStringList allClasses = getClassNames(parentClass);
    QStringList models;
    QString fullClassName;
    for(int i=0;i<allClasses.size();i++)
    {
        fullClassName = parentClass.isEmpty() ? allClasses.at(i) : parentClass+"."+allClasses.at(i);

        // check if it is a record
        if(isRecord(fullClassName))
            models.push_back(allClasses.at(i));
    }
    return models;
}

QStringList MOomc::getClasses(QString parentClass)
{

    QStringList allClasses = getClassNames(parentClass);
    QStringList models;
    QString fullClassName;
    for(int i=0;i<allClasses.size();i++)
    {
        fullClassName = parentClass.isEmpty() ? allClasses.at(i) : parentClass+"."+allClasses.at(i);

        // check if it is a class
        if(isClass(fullClassName))
            models.push_back(allClasses.at(i));
    }
    return models;
}

QStringList MOomc::getModels(QString parentClass)
{

    QStringList allClasses = getClassNames(parentClass);
    QStringList models;
    QString fullClassName;
    for(int i=0;i<allClasses.size();i++)
    {
        fullClassName = parentClass.isEmpty() ? allClasses.at(i) : parentClass+"."+allClasses.at(i);
        // check if it is a model
        if(isModel(fullClassName))
            models.push_back(allClasses.at(i));
    }
    return models;
}

void MOomc::getContainedComponents(QString parentClass,QStringList & compNames,QStringList & compClasses,bool includeInherited)
{

    compNames.clear();
    compClasses.clear();

    QString commandRes;
    QList<ComponentInfo*> componentInfoList;

    if(!parentClass.isEmpty())
    {
        QString msg;
        msg.sprintf("Reading components of class %s ",parentClass.toLatin1().data());
        InfoSender::instance()->send(Info(msg,ListInfo::OMCNORMAL2));

        commandRes = evalCommand("getComponents(" + parentClass +", useQuotes = true)");
        commandRes.trimmed();

        QStringList list = StringHandler::unparseArrays(commandRes);

        for (int i = 0 ; i < list.size() ; i++) {
          if (list.at(i) == "Error") {
            continue;
          }
          ComponentInfo *pComponentInfo = new ComponentInfo();
          pComponentInfo->parseComponentInfoString(list.at(i));
          componentInfoList.append(pComponentInfo);
        }

        for(int i = 0; i < componentInfoList.size(); i++)
        {
          ComponentInfo *pComponentInfo = componentInfoList.at(i);
          compNames.push_back(pComponentInfo->mName);
          compClasses.push_back(pComponentInfo->mClassName);
        }
    }

    QStringList inhNames, inhClasses;

    if(includeInherited)
    {
        getInheritedComponents(parentClass,inhNames,inhClasses);
        compNames.append(inhNames);
        compClasses.append(inhClasses);
    }
}

QString MOomc::getParameterValue(QString parentClass, QString parameterName)
{
    QString commandRes= evalCommand("getParameterValue(" + parentClass +","+ parameterName+")");
    if(commandRes=="Error")
        return QString();
    else
        return commandRes;
}

/** This function will try to find value of parameter corresponding to its type.
  * Sometimes, a parameter value is a reference to another one. This functions will recursively
  * seek through references until it finds one corresponding to the type specified.
  */
QVariant MOomc::getParameterValue(QString parentClass, QString parameterName,VariableType type, QVariant defaultValue)
{
    QString commandRes= evalCommand("getParameterValue(" + parentClass +","+ parameterName+")");
    if(commandRes=="Error" || commandRes.isEmpty())
        return defaultValue;


    QVariant::Type variantType;
    switch(type)
    {
    case OMREAL:
        variantType = QVariant::Double;
        break;
    case OMBOOLEAN :
        variantType = QVariant::Bool;
        break;
    case OMINTEGER :
        variantType = QVariant::Int;
        break;
    case OMSTRING :
        variantType = QVariant::String;
        break;
    default:
        variantType = QVariant::String;
        break;
    }

    int i=0; // to avoid infinite loop (if for whatever reason, incorrect case is catched by error boolean)
    QVariant result = commandRes;

    bool error = false;

    while(!result.convert(variantType)  && (i<100) &&!error)
    {
        parentClass = commandRes.section(".",0,-2);
        parameterName= commandRes.section(".",-1,-1);
        commandRes = evalCommand("getParameterValue(" + parentClass +","+ parameterName+")");
        error = (commandRes=="Error" || commandRes.isEmpty());
        result = commandRes;
        i++;
    }
    if(result.convert(variantType) && result.convert(variantType))
        return result;
    else
        return defaultValue;
}

QStringList MOomc::getParameterNames(QString parentClass,bool includeInherited)
{
    QStringList parameterNames;
    QString commandRes= evalCommand("getParameterNames(" + parentClass +")");
    if(commandRes=="Error")
        return QStringList(commandRes);

    commandRes.remove("}");
    commandRes.remove("{");
    commandRes.remove(" ");
    parameterNames = commandRes.split(",");

    if(includeInherited)
    {
        QStringList inheritedClass = getInheritedClasses(parentClass);
        for(int i=0;i<inheritedClass.size();i++)
        {
            parameterNames << getParameterNames(inheritedClass.at(i),true);
        }
    }
    return parameterNames;
}

QMap<QString,QString> MOomc::getConnections(const QString & curModel)
{
    QMap<QString,QString> result;
    QStringList connectFields;
    //getting nb connections
    QString res= evalCommand("getConnectionCount("+curModel+")");
    if (res!="Error")
    {
        int nbConnections = res.toInt();

        // for each connection
        for(int iC=0;iC<nbConnections;iC++)
        {
            //get its name
            res= evalCommand("getNthConnection("+curModel+","+QString::number(iC+1)+")");
            if (res!="Error")
            {
                res.remove("{");
                res.remove("}");
                connectFields = res.split(",");

                result.insert(connectFields.at(0),connectFields.at(1));
            }
        }
    }
    return result;
}

QStringList MOomc::getInheritedClasses(QString inheritingClass)
{
    if(inheritingClass.isEmpty())
        return QStringList();

    // Getting number and names of inherited classes
    QString commandText = "getInheritanceCount(" + inheritingClass +")";
    QString commandRes= evalCommand(commandText);

    int nbInheritance = 0;
    QStringList inheritedClasses;

    if ((commandRes!="Error"))
    {
        nbInheritance = commandRes.toInt();

        for(int i=0;i<nbInheritance;i++)
        {
            commandText = "getNthInheritedClass(" + inheritingClass +","+QString::number(i+1)+")";
            commandRes= evalCommand(commandText);
            inheritedClasses << commandRes;
        }
    }
    inheritedClasses.removeAll(inheritingClass); // sometimes, a class appears as inheriting itself
    return inheritedClasses;
}

QString MOomc::getPrimitiveClass(QString className)
{
    if(isPrimitive(className))
        return className;
    else
    {
        QStringList inherited = getInheritedClasses(className);
        if(inherited.size()==1)
        {
            return getPrimitiveClass(inherited.at(0));
        }
        else
            return QString();
    }
}
VariableType MOomc::getPrimitiveDataType(QString className)
{
    QString primitiveClass = getPrimitiveClass(className);

    if(!primitiveClass.compare("Real"))
        return OMREAL;
    if(!primitiveClass.compare("Integer"))
        return OMINTEGER;
    if(!primitiveClass.compare("Boolean"))
        return OMBOOLEAN;
    if(!primitiveClass.compare("String"))
        return OMSTRING;

    // case not found, REAL is default DataType
    return OMREAL;
}
bool MOomc::isPrimitivelyInteger(QString className)
{
    return !getPrimitiveClass(className).compare("Integer");
}
bool MOomc::isPrimitivelyReal(QString className)
{
    return !getPrimitiveClass(className).compare("Real");
}
bool MOomc::isPrimitivelyBoolean(QString className)
{
    return !getPrimitiveClass(className).compare("Boolean");
}

QString MOomc::getComponentClass(QString parameter)
{
    QString parent = parameter.section(".",0,-2);
    QStringList compNames,compClasses;
    getContainedComponents(parent,compNames,compClasses);
    int iParameter = compNames.indexOf(parameter.section(".",-1,-1));
    if(iParameter>-1)
        return compClasses.at(iParameter);
    else
        return QString();
}


QStringList MOomc::getComponentModifierNames(QString componentName)
{
    QStringList modifierNames;
    QString commandText,commandRes;


    QStringList levelNames = componentName.split(".");
    //Checking that component is a first level one
    if(levelNames.size()!=2)
    {
        InfoSender::instance()->send( Info(ListInfo::OMSOBTMODIFIERSFAILED,componentName));
        return QStringList();
    }
    else
    {
        commandText = "getComponentModifierNames(" + levelNames.at(0) +","+levelNames.at(1)+")";
        commandRes = evalCommand(commandText);

        if(commandRes.indexOf("Class getConnectionCount (its type)  not found in scope <global scope>")>-1)
        {
            InfoSender::instance()->send( Info(ListInfo::OMSOBTMODIFIERSFAILED,componentName));
            return QStringList();
        }

        commandRes.remove("{");
        commandRes.remove("}");
        commandRes.remove(" ");
        return(commandRes.split(","));
    }
}


QString MOomc::getFlattenedModel(const QString & modelName)
{
    InfoSender::instance()->sendCurrentTask("Instantiating model "+modelName);
    initCommandLineOptions();
    QString flatcmd = "instantiateModel("+modelName+")";
    QString errorString;
    QString flattened = evalCommand(flatcmd,errorString);

    if(flattened=="\"\"")
        flattened.clear();

    if(!errorString.isEmpty() && !flattened.isEmpty())
        InfoSender::instance()->sendWarning("Instantiating model warned ["+modelName+"]");
    else  if(!errorString.isEmpty() && flattened.isEmpty())
        InfoSender::instance()->sendError("Instantiating model failed ["+modelName+"]");
    else
        InfoSender::instance()->sendNormal("Instantiating model successful ["+modelName+"]");

    return flattened;
}

QString MOomc::getFlattenedModifierValue(const QString & modelName,const QString & componentName,const QString & modifierName)
{
    QString flattened = getFlattenedModel(modelName);
    return getFlattenedModifierValue(modelName,componentName,modifierName,flattened);
}

QStringList MOomc::getFlattenedModifierVectorValue(const QString & modelName,const QString & componentName,const QString & modifierName, int size,QString & flattenedModel)
{
    QStringList result;
    QString curModifier;
    QString curValue;
    for(int i=0;i<size;i++)
    {
        curModifier = modifierName+"\\["+QString::number(i+1)+"\\]";
        curValue = getFlattenedModifierValue(modelName,componentName,curModifier,flattenedModel);
        result.push_back(curValue);
    }
    return result;
}


QString MOomc::getFlattenedModifierUnit(const QString & modelName,const QString & componentName,const QString & modifierName, QString & flattenedModel)
{
    if(flattenedModel.isEmpty())
        flattenedModel = getFlattenedModel(modelName);

    QRegExp exp1(componentName+"."+modifierName+".*unit[\\s|=]*\\\\\\\"(.*)\\\\\\\"");
    exp1.setMinimal(true);

    int i1 = flattenedModel.indexOf(exp1);

    if((i1>-1)&&exp1.capturedTexts().size()==2)
    {
        QString result = exp1.capturedTexts().at(1);
        return result;
    }
    return QString();
}



QString MOomc::getFlattenedModifierValue(const QString & modelName,const QString & componentName,const QString & modifierName, QString & flattenedModel)
{
    if(flattenedModel.isEmpty())
        flattenedModel = getFlattenedModel(modelName);

    QStringList lines = flattenedModel.split("\n");
    if(lines.size()==1)
    {
        lines = flattenedModel.split("\\n");
    }


    // 1st format : Real sterilisateur.Sterilisateur.FactMin = 0.0;
    QRegExp exp1(".*\\b"+componentName+"."+modifierName+"\\b[\\s]*=[\\s]*([\\S|\\\\|\"]+);");

    int i1 =  lines.indexOf(exp1);

    if((i1>-1)&&exp1.capturedTexts().size()==2)
    {
        QString result = exp1.capturedTexts().at(1);
        result = result.remove(QRegExp("[\\\\|\"]*"));
        return result;
    }

    // 2nd format with unit
    QRegExp exp2(".*\\b"+componentName+"."+modifierName+"\\b\\([.*]\\)[\\s]*=[\\s]*(  );");
    int i2 = lines.indexOf(exp2);

    if((i2>-1)&&exp2.capturedTexts().size()==2)
        return exp2.capturedTexts().at(1);

    // 3rd format
    QRegExp exp3(".*\\b"+componentName+"\\b\\(.*\\b"+modifierName+"\\b[\\s]*=[\\s]*([\\d|\\.]*).*;");
    int i3 = lines.indexOf(exp3);

    if((i3>-1)&&exp3.capturedTexts().size()==2)
        return exp3.capturedTexts().at(1);

    // 4th format : a vector
    // e.g.   eIFactMultEquation.groups = {\"group1\", \"group2\"};
    QRegExp exp4(".*\\b"+componentName+"."+modifierName+"\\b[\\s]*=[\\s]\\{(.*)\\};");
    int i4 = lines.indexOf(exp4);

    if((i4>-1)&&exp4.capturedTexts().size()==2)
    {
        return exp4.capturedTexts().at(1);
    }

    return QString();
}

QString MOomc::getComponentModifierValue(QString modelName,QString shortComponentName,QString modifierName)
{
    QStringList modifierNames;
    QString commandText,commandRes;

    //    QStringList levelNames = componentName.split(".");
    //    //Checking that component is a first level one
    //    if(levelNames.size()!=2)
    //    {
    //        InfoSender::instance()->send( Info(ListInfo::OMSOBTMODIFIERSFAILED,componentName));
    //        QString errorString;
    //        return errorString;
    //    }
    //    else
    //    {
    commandText = "getComponentModifierValue(" + modelName +","+shortComponentName+"."+modifierName+")";
    commandRes = evalCommand(commandText);
    if(commandRes.contains("Error",Qt::CaseInsensitive))
    {
        InfoSender::instance()->send( Info(ListInfo::OMSOBTMODIFIERSFAILED,shortComponentName));
        return QString();
    }
    commandRes.remove("=");
    commandRes.remove(" ");
    return commandRes;
    //  }
}

bool MOomc::setComponentModifiers(QString compName,QString model, QStringList modNames,QStringList modValues)
{
    for(int i=0;i<modNames.size();i++)
    {
        QString commandText = "setComponentModifierValue(";
        commandText += model+",";
        commandText += compName.section(".",-1,-1)+"."+modNames.at(i)+","+"$Code(="+modValues.at(i)+"))";

        QString commandRes = evalCommand(commandText);
    }
    return true;
}


bool MOomc::isConnector(QString className)
{
    QString commandText = "isConnector(" + className +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::isModel(QString className)
{
    QString commandText = "isModel(" + className +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::isRecord(QString className)
{
    QString commandText = "isRecord(" + className +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::isClass(QString className)
{
    QString commandText = "isClass(" + className +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::isPackage(QString className)
{
    QString commandText = "isPackage(" + className +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::isPrimitive(QString className)
{
    QString commandText = "isPrimitive(" + className +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::isComponent(QString name)
{
    // #TOCHECK
    QStringList curNames, curClasses;
    QString parentClass = name.section(".",0,-2);
    getContainedComponents(parentClass,curNames,curClasses);
    return (curNames.contains(name));
}

Modelica::ClassRestr MOomc::getClassRestriction(QString className)
{
    if(isPackage(className))
        return Modelica::PACKAGE;
    else
        if(isModel(className))
            return Modelica::MODEL;
        else
            if(isComponent(className))
                return Modelica::COMPONENT;
            else
                return Modelica::OTHER;
}

QString MOomc::getAnnotation(QString compName,QString compClass,QString model)
{

    QString annot;
    /// \todo
    //    QString commandText = "list("+model+")";
    //    QString list= evalCommand(commandText);


    //    QStringList listLines = list.split("\n");
    //    QString compShortName = compName.section(".",-1,-1);

    //    QRegExp regExp(".*"+compClass+"[\\s]*"+ compName.section(".",-1,-1)+"\\(.*\\)[\\s]*;");
    //    int iLine = listLines.indexOf(regExp);

    //    QStringList cap=regExp.capturedTexts();
    //    listLines.at(iLine).contains(regExp);
    //    cap = regExp.capturedTexts();

    //    if(iLine == -1)
    //        return QString();
    //    else
    //    {
    //        QString annot = listLines.at(iLine);
    //        if(!annot.contains("annotation"))
    //            return QString();
    //        else
    //        {
    //            annot.remove(QRegExp(".*annotation\\("));
    //            annot.remove(QRegExp("\\)[\\s]*;"));
    //            return annot;
    //        }
    //    }
    return annot;
}

QString MOomc::getComment(QString modelName, QString compName)
{
    QString commandRes= evalCommand("getComponents(" + modelName +")");

    if(commandRes.indexOf("error",Qt::CaseInsensitive)==0)
        return QString();

    //  {{Modelica.SIunits.Temp_K,TorcH,"ORC hot temperature (K)", "public", false, false, false, false, "parameter", "none", "unspecified",{}},{Modelica.SIunits.Temp_K,TorcC,"ORC cold temperature (K)", "public", false, false, false, false, "parameter", "none", "unspecified",{}}}

    QRegExp rexp(".*\\b"+compName+"\\b,"+"\"(.*)\"");
    rexp.setMinimal(true);
    if(commandRes.contains(rexp)&&rexp.capturedTexts().size()>1)
    {
        QStringList captured = rexp.capturedTexts();
        return captured.at(1);
    }
    else
        return QString();
}

int MOomc::getConnectionNumber(QString className)
{
    QString commandText = "getConnectionCount("+className+")";
    QString commandRes= evalCommand(commandText);

    return commandRes.toInt();
}

bool MOomc::initCommandLineOptions()
{
    QString value  = MOSettings::value("OMCCommandLineOptions").toString();
    QString commandText = "setCommandLineOptions(\""+value+"\");";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::clearCommandLineOptions()
{
    QString commandText = "setCommandLineOptions(\" \");";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::translateModel(QString model)
{
    initCommandLineOptions();
    QString commandText = "translateModel(" + model +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::buildModel(QString model,QString & exeFile,QString & initFile)
{
    initCommandLineOptions();
    QString commandText = "buildModel("+model+")";
    QString commandRes= evalCommand(commandText);
    commandRes.remove(QRegExp("[{|}|\"]"));

    exeFile = commandRes.section(",",0,0);
    bool success = !exeFile.isEmpty();
    if(!success)
    {
        exeFile.clear();
        initFile.clear();
    }
    else
    {
#ifdef WIN32
        exeFile.append(".exe");
#endif
        QDir dir = QFileInfo(exeFile).absoluteDir();
        initFile = dir.absoluteFilePath(commandRes.section(",",1,1));
    }
    return success;
}


bool MOomc::deleteComponent(QString compName)
{
    QString model = compName.section(".",0,0);

    compName = compName.section(".",1,-1);


    QString commandText = "deleteComponent(" + compName +"," + model +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");

}

bool MOomc::deleteClass(QString className)
{
    QString commandText = "deleteClass(" + className+")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

bool MOomc::deleteConnection(const QString & org,const QString & dest, const QString & model)
{
    // getting short names
    QString shortOrg = org;
    shortOrg.remove(QRegExp("^"+model+"."));
    QString shortDest = dest;
    shortDest.remove(QRegExp("^"+model+"."));

    QString commandText = "deleteConnection(" + shortOrg +"," + shortDest +"," + model +")";
    QString commandRes= evalCommand(commandText);

    commandText = "deleteConnection(" + shortDest +"," + shortOrg +"," + model +")";
    commandRes= evalCommand(commandText);

    QMap<QString,QString> existingConns = getConnections(model);

    if((existingConns.value(shortOrg) == shortDest)||(existingConns.value(shortDest) == shortOrg))
    {
        QString msg = "deleting connection didn't work ["+org+","+dest+"]";
        InfoSender::instance()->send(Info(msg,ListInfo::WARNING2));
        return false;
    }
    else
        return true;

}
bool MOomc::deleteConnections(const QStringList &  orgs, const QStringList  &  dests, const QString & model)
{
    bool allOk = false;
    if(orgs.size()==dests.size())
    {
        allOk = true;
        for(int i=0;i<orgs.size();i++)
        {
            allOk = deleteConnection(orgs.at(i),dests.at(i),model) && allOk;
        }
    }
    return allOk;
}

bool MOomc::deleteConnections(const QStringList &  orgs,const QList<QStringList> & dests,const QString & model)
{
    bool allOk = false;
    if(orgs.size()==dests.size())
    {
        allOk = true;
        for(int i=0;i<orgs.size();i++)
        {
            for(int j=0;j<dests.at(i).size();j++)
                allOk = deleteConnection(orgs.at(i),dests.at(i).at(j),model) && allOk;
        }
    }
    return allOk;
}

bool MOomc::addConnection(QString org, QString dest)
{
    QString modelOrg = org.section(".",0,-3);
    QString modelDest = dest.section(".",0,-3);

    if(modelOrg!=modelDest || modelOrg.isEmpty())
        return false;
    else
    {
        QString shortOrg = org.section(".",-2,-1);
        QString shortDest = dest.section(".",-2,-1);

        QString commandText = "addConnection("+shortOrg+","+shortDest+","+modelOrg+")";
        QString commandRes = evalCommand(commandText);

        return ((commandRes=="true") || (commandRes=="Ok"));
    }
}

bool MOomc::addConnections(QStringList orgs, QStringList dests)
{
    if(orgs.size()!=dests.size())
        return false;
    else
    {
        bool all = true;
        for(int i =0;i < orgs.size();i++)
        {
            all = addConnection(orgs.at(i),dests.at(i)) && all;
        }
        return all;
    }
}

bool MOomc::addConnections(QStringList orgs, QList<QStringList> dests)
{
    if(orgs.size()!=dests.size())
        return false;
    else
    {
        bool all = true;
        for(int i =0;i < orgs.size();i++)
        {
            for(int j=0;j<dests.at(i).size();j++)
                all = addConnection(orgs.at(i),dests.at(i).at(j)) && all;
        }
        return all;
    }
}

bool MOomc::inherits(QString childClass, QString parentClass)
{
    QStringList inherited = getInheritedClasses(childClass);
    while(!inherited.isEmpty())
    {
        if(inherited.contains(parentClass))
            return true;
        else
        {
            inherited.append(getInheritedClasses(inherited.at(0)));
            inherited.removeAt(0);
        }
    }
    return false;
}


bool MOomc::addComponent(QString name,QString className, QString modelName,QString annotation)
{
    if(annotation.isEmpty() || (annotation.left(1)!="\"") || (annotation.right(1)!="\""))
        annotation = "\""+annotation+"\"";

    QString commandText = "addComponent("+name.section(".",-1,-1)+","+className+","+modelName+",annotate="+annotation+")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}
bool MOomc::save(QString model)
{
    QString commandText = "save(" + model +")";
    QString commandRes= evalCommand(commandText);
    return (commandRes=="true");
}

//void MOomc::setCommand(QString comm)
//{
//    command=comm;
//}

QString MOomc::evalCommand(QString command)
{
    QString errorString;
    return evalCommand(command,errorString);
}

QString MOomc::evalCommand(QString command,QString &errorString)
{
    if(!evalMutex.tryLock())
    {
        errorString = "Could not launch OMC command";
        return QString();
    }
    else
    {
        errorString.clear();
        QString result;

        QString msg;
        InfoSender::instance()->send( Info(msg.sprintf("OMC: %s",command.toLatin1().data()),ListInfo::OMCNORMAL2));
        nbCalls++;

        if (!mHasInitialized)
        {
            InfoSender::instance()->send(Info(QString("OMC not started. Please start it again (menu OMC->restart)"),ListInfo::WARNING2));
            result = QString();
        }
        else
        {
            // Send command to server
            try
            {
                //mResult = mOMC->sendExpression(command.toLatin1());
                mResult = QString::fromLocal8Bit(mOMC->sendExpression(command.toLocal8Bit()));
                // logOMCMessages(command);
                InfoSender::instance()->send(Info(getResult(),ListInfo::OMCNORMAL2));

                // display errors
                command = "getErrorString()";
                errorString = QString::fromLocal8Bit(mOMC->sendExpression(command.toLocal8Bit()));
                errorString.remove("\"");
                if((!errorString.isEmpty())&&(errorString!="\n"))
                    InfoSender::instance()->send(Info(errorString,ListInfo::OMCWARNING2));

                result = getResult();
            }
            catch(CORBA::Exception&)
            {
                QFile::remove(mObjectRefFile);
                InfoSender::instance()->send(Info(QString("Communication with OMC server has lost ")));
            }
            catch( omniORB::fatalException & ex)
            {
                InfoSender::instance()->debug("Caught omniORB2 fatalException. This is a bug in omniORB");
            }
            catch( CORBA::COMM_FAILURE &ex)
            {
                InfoSender::instance()->debug("Caught CORBA::COMM_FAILURE");
            }
            catch (...)
            {
                InfoSender::instance()->debug("Caught exception");
            }
        }


        evalMutex.unlock();
        return result;
    }



    //if(!delegate_)
    //    startServer();


    //QString msg;
    //InfoSender::instance()->send( Info(msg.sprintf("OMC : %s",_command.toLatin1().data()),ListInfo::OMCNORMAL2));
    //delegate_->evalExpression( _command );
    //QString result = delegate_->result();
    //InfoSender::instance()->send(Info(result,ListInfo::OMCNORMAL2));
    //if(result.compare("NOT RESPONDING",Qt::CaseInsensitive)==0)
    //{
    //    // restart server
    //    delegate_->closeConnection();
    //    delete delegate_;
    //    delegate_ = NULL;
    //    startServer();
    //    delegate_->evalExpression( _command );
    //    result = delegate_->result();
    //}
    //return result;
}
//void MOomc::evalCommand()
//{
//    nbCalls++;
//
//    QString msg;
//    QString error;
//    QString warning;
//
//    // send command to OMC
//    if( isStarted )
//    {
//eval:
//        // 2006-02-02 AF, Added try-catch
//        try
//        {
//            delegate_->evalExpression(command );
//        }
//        catch( exception &e )
//        {
//            exceptionInEval(e);
//            return;
//        }
//
//        // get result
//        msg = delegate_->result();
//        InfoSender::instance()->send(Info(msg,ListInfo::OMCNORMAL2));
//
//        //if(msg.contains("error",Qt::CaseInsensitive))
//        if(true)
//        {
//            // get Error text
//            try
//            {
//                QString getErrorString = "getErrorString()";
//                delegate_->evalExpression(getErrorString);
//            }
//            catch( exception &e )
//            {
//                exceptionInEval(e);
//                return;
//            }
//            msg = delegate_->result();
//            if(msg!="\"\"")
//                InfoSender::instance()->send(Info(msg,ListInfo::OMCERROR2));
//        }
//    }
//    else
//    {
//        if( startServer() )
//        {
//            msg ="No OMC server started - restarted OMC\n" ;
//            InfoSender::instance()->send(Info(msg,ListInfo::WARNING2));
//            goto eval;
//        }
//        else
//        {
//            msg="No OMC server started - unable to restart OMC\n";
//            InfoSender::instance()->send(  Info(msg,ListInfo::ERROR2));
//        }
//    }
//    return;
//}

//void MOomc::exceptionInEval(exception &e)
//{

//    QString msg;
//    // 2006-0-09 AF, try to reconnect to OMC first.
//    try
//    {
//        msg = "Command did not achieve properly :"+QString( e.what() )+"\n";
//        msg = msg + "Trying to reconnect...\n";
//        InfoSender::instance()->send( Info(msg,ListInfo::WARNING2));

//        delegate_->closeConnection();
//        delegate_->reconnect();
//        evalCommand();
//    }
//    catch( exception &e )
//    {
//        // unable to reconnect
//        InfoSender::instance()->send(  Info("Still did not work. Aborting.",ListInfo::ERROR2));

//    }
//}

QStringList MOomc::getDependenciesPaths(QString fileName,bool commentImportPaths)
{
    QStringList importFiles;


    //Openfile and read it
    QFile moFile(fileName);
    if(!moFile.exists())
    {
        return QStringList();
    }
    if(!moFile.open(QIODevice::ReadOnly))
        return QStringList();

    QTextStream ts(&moFile);
    QString text = ts.readAll();
    ts.reset();
    moFile.close();

    QStringList lines = text.split("\n");
    QString tmpDep;

    QRegExp regExp("[\\s]*import.*\".*\"[\\s]*;[\\s]*");

    int index0=0;
    int index=lines.indexOf(regExp,index0);

    while(index>-1)
    {
        tmpDep = lines.at(index);
        tmpDep.remove(QRegExp("[\\s]*import[\\s]*\""));
        tmpDep.remove(QRegExp("\".*"));
        importFiles.push_back(tmpDep);

        if(commentImportPaths)
        {
            tmpDep=lines.at(index);
            tmpDep.insert(0,"//");
            lines.replace(index,tmpDep);
        }

        index0=index+1;
        index=lines.indexOf(regExp,index0);
    }


    if(commentImportPaths && importFiles.size()>0)
    {
        bool openOk = moFile.open(QIODevice::WriteOnly);
        QTextStream ts2(&moFile);

        QString newText = lines.join("\n");
        ts2 << newText;
        moFile.close();
    }

    return importFiles;
}

void MOomc::loadModel(QString filename,bool force,bool &ok,QString & error)
{
    bool doLoad = true;
    if(!force)
    {
        QStringList classLoaded = getClassNames("");
        int i=0;
        while(doLoad && i<classLoaded.size())
        {
            QFileInfo fileOfClassInOMC = getFileOfClass(classLoaded.at(i));

            if(fileOfClassInOMC == QFileInfo(filename))
                doLoad = false;

            i++;
        }
    }

    if(doLoad)
    {
        // store mo file before modifying it
        // (imports will be commented)
        QFile moFile(filename);
        QFileInfo moFileInfo(filename);
        QDir moDir(moFileInfo.absolutePath());
        QString copyPath=filename+"_bkp";
        moDir.remove(copyPath);
        moFile.copy(filename+"_bkp");


        // load dependencies
        QStringList depFiles = getDependenciesPaths(filename,true);
        for(int i=0;i<depFiles.size();i++)
        {
            bool _depOk;
            QString _depError;
            loadModel(depFiles.at(i),false,_depOk,_depError);
        }


        QString result = loadFileWThread(filename);



        // Restore file
        moDir.remove(filename);
        QFile moCopiedFile(copyPath);
        moCopiedFile.copy(filename);

        if(result.contains("true",Qt::CaseInsensitive))
        {
            InfoSender::instance()->send(Info(ListInfo::MODELLOADSUCCESS,filename));
            ok = true;
            error = "";

        }
        else
        {
            ok = false;
            error = result;
            InfoSender::instance()->send(Info(ListInfo::MODELLOADFAIL,filename,error));
        }
    }
    else
    {
        InfoSender::instance()->send( Info(QString("Model file already loaded - ignoring. (" + filename + ")"),ListInfo::NORMAL2));
        ok = true;
        error = "";
    }

    InfoSender::eraseCurrentTask();
}

QStringList MOomc::loadFiles(const QStringList & filePaths)
{
    QStringList result;
    for(int i=0;i<filePaths.size();i++)
    {
        result.push_back(loadFile(filePaths.at(i)));
    }
    return result;
}

QString MOomc::loadFile(const QString & filePath)
{
    InfoSender::sendCurrentTask("Loading model "+filePath);

    QString localFile = filePath;
    localFile = localFile.replace("\\","/");
    QString cmd = QString("loadFile(\"") + localFile + QString("\")");

    QString errorString;
    QString result = evalCommand(cmd,errorString);

    // if file is not in UTF-8, try with another encoding
    if(result.contains("false",Qt::CaseInsensitive) && errorString.contains("The file was not encoded in UTF-8"))
    {
        cmd = QString("loadFile(\"") + localFile + QString("\",encoding=\"ISO-8859-1\")");
        result = evalCommand(cmd);
    }

    emit loadedFile(localFile,result);
    InfoSender::eraseCurrentTask();
    return result;
}

QString MOomc::loadFileWThread(QString filePath)
{
    MOThreads::OMCModelLoader* loader = new MOThreads::OMCModelLoader(filePath,this);
    //loader->start(QThread::HighestPriority);
    loader->run(); // avoid thread separation. Does not work properly yet...
    while (loader->isRunning())
    {
        InfoSender::instance()->sendNormal("refreshing");
        qApp->processEvents();
    }

    QString result = loader->_result;
    delete loader;
    return result;
}

/**
  * if path is empty, load default one
  */
void MOomc::loadStandardLibrary(QString path)
{
    if(path.isEmpty())
        evalCommand("loadModel(Modelica)");
    else
    {
        loadFile(path);
    }
    //evalCommand("loadModel(ModelicaServices)");

    //    //! @todo Remove it once we have removed Media   and  and    and  and   Fluid from MSL.
    //    // just added to remove Fluid   and  and    and  and   Media Library...

    //    evalCommand("deleteClass(Modelica.Media)");
    //    evalCommand("deleteClass(Modelica.Fluid)");
}

void MOomc::clear()
{
    if(isStarted())
        evalCommand("clear()");

    initTempDirectory();
}


void MOomc::exit()
{
    // check if omc is running, if so: ask if it is ok that omc also closes.
    try
    {
        if( mHasInitialized )
        {
            // delegate_->closeConnection();
            // delegate_->reconnect();
            stopServer();
        }
    }
    catch(exception e)
    {}

    //emit emitQuit();
}

bool MOomc::startServer()
{
    try
    {
        // evalMutex.unlock();
        QString msg;
        const char *omhome = getenv("OPENMODELICAHOME");
        QString omcPath;
#ifdef WIN32
        if (!omhome)
        {
            InfoSender::instance()->send(Info("OPEN_MODELICA_HOME_NOT_FOUND"));
            return false;
        }
        omcPath = QString( omhome ) + "/bin/omc.exe";
#else /* unix */
        omcPath = (omhome ? QString(omhome)+"/bin/omc" : QString(CONFIG_DEFAULT_OPENMODELICAHOME) + "/bin/omc");
#endif

        // Check the IOR file created by omc.exe
        QFile objectRefFile;
        QString fileIdentifier;
        fileIdentifier = qApp->sessionId().append(QTime::currentTime().toString(tr("hh:mm:ss:zzz")).remove(":"));

        objectRefFile.setFileName(QString(Utilities::tempDirectory()).append(QDir::separator()).append("openmodelica.objid.").append(this->mName).append(fileIdentifier));

        if (objectRefFile.exists())
            objectRefFile.remove();

        mObjectRefFile = objectRefFile.fileName();


        // Start the omc.exe
        QStringList parameters;
        QString str;
        parameters << QString("+c=").append(mName).append(fileIdentifier) << QString("+d=interactiveCorba") << QString("+corbaObjectReferenceFilePath=").append(Utilities::tempDirectory());
        QProcess *omcProcess = new QProcess();
        QFile omcOutputFile;
#ifdef WIN32 // Win32
        omcOutputFile.setFileName(QString(Utilities::tempDirectory()).append(QDir::separator()).append("openmodelica.omc.output.").append(mName));
#else // UNIX environment
        char *u = getenv("USER");
        QString user = u ? u : "nobody";
        omcOutputFile.setFileName(QString(Utilities::tempDirectory()).append(QDir::separator()).append("openmodelica.").append(( QString(user))).append(".omc.output.").append(mName));
#endif

        InfoSender::instance()->send( Info(str.sprintf("OMC: running command: %s %s",
            omcPath.toLatin1().data(),
        parameters.join(" ").toLatin1().data()),ListInfo::OMCNORMAL2));
        omcProcess->setProcessChannelMode(QProcess::MergedChannels);
        omcProcess->setStandardOutputFile(omcOutputFile.fileName());
        omcProcess->start(omcPath, parameters);
        // wait for the server to start.
        int ticks = 0;
        while (!objectRefFile.exists())
        {
            SleeperThread::msleep(1000);
            ticks++;
            if (ticks > 20)
            {
                msg = "OMC: unable to find OMC server, Object reference file " + mObjectRefFile + " not created.";
                throw std::runtime_error(msg.toStdString());
            }
        }

        // ORB initialization.
        int argc = 2;
        static const char *argv[] = { "-ORBgiopMaxMsgSize", "10485760" };
        CORBA::ORB_var orb = CORBA::ORB_init(argc, (char **)argv);

        objectRefFile.open(QIODevice::ReadOnly);

        char buf[1024];
        objectRefFile.readLine( buf, sizeof(buf) );
        QString uri( (const char*)buf );

        CORBA::Object_var obj = orb->string_to_object(uri.trimmed().toLatin1());

        mOMC = OmcCommunication::_narrow(obj);
        mHasInitialized = true;
    }
    catch(std::exception &e)
    {
        QString msg = e.what();
        InfoSender::instance()->send(Info(msg,ListInfo::ERROR2));
        mHasInitialized = false;
        return false;
    }
    catch (CORBA::Exception&)
    {
        QString msg = "Unable to communicate with OMC";
        InfoSender::instance()->send(Info(msg,ListInfo::ERROR2));
        mHasInitialized = false;
        return false;
    }

    evalCommand("getInstallationDirectoryPath()");
    OMCHelper::OpenModelicaHome = StringHandler::removeFirstLastQuotes(getResult());

    initTempDirectory();

    // set the OpenModelicaLibrary variable.
    evalCommand("getModelicaPath()");
    OMCHelper::OpenModelicaLibrary = StringHandler::removeFirstLastQuotes(getResult());
    return true;
}



void MOomc::initTempDirectory()
{
    // set the temp directory.
    QString tmpPath = Utilities::tempDirectory();
    QString cdResult;
    if (!QDir().exists(tmpPath))
    {
        if (LowTools::mkpath(tmpPath,false))
            cdResult = changeDirectory(tmpPath);
    }
    else
        cdResult = changeDirectory(tmpPath);
}

void MOomc::stopServer()
{
    if(isStarted())
        //  if( delegate_ )
    {
        qDebug("Stoping moomc...");
        QString quit = "quit()";
        evalCommand(quit);
        InfoSender::instance()->send( Info("Quiting OMC...",ListInfo::NORMAL2));
        // evalMutex.lock();
    }
    mHasInitialized = false;
}

bool MOomc::isStarted()
{
    return mHasInitialized;
}

QString MOomc::getResult()
{
    return mResult.trimmed();
}

//OmcCommunicator* MOomc::getCommunicator()
//{
//    //return mCommunicator;
//}

QString MOomc::changeDirectory(QString directory)
{
    QString correctedDir = QDir(directory).absolutePath();
    if (correctedDir.isEmpty())
    {
        evalCommand("cd()");
    }
    else
    {
        correctedDir = correctedDir.replace("\\", "/");
        evalCommand("cd(\"" + correctedDir + "\")");
    }
    return getResult();
}

QString MOomc::getWorkingDirectory()
{
    evalCommand("cd()");
    QString path = getResult();
    path = path.remove("\"");
    return path;
}

QString MOomc::getTempDirectory()
{
    evalCommand("getTempDirectoryPath()");
    QString path = getResult();
    path = path.remove("\"");
    return path;
}

QFileInfo MOomc::getFileOfClass(QString _className)
{
    QString filePath = "";
    QString res = evalCommand("getClassInformation("+_className+")");

    if(res.compare("Error",Qt::CaseInsensitive))
    {
        filePath = res.section(",",2,2);
        filePath.remove("\"");
    }

    return QFileInfo(filePath);
}

QString MOomc::runScript(QString scriptFile)
{
    scriptFile = scriptFile.replace("\\","/");
    QString commandText = "runScript(\"" + scriptFile +"\")";
    QString commandRes= evalCommand(commandText);
    return commandRes;
}

QStringList MOomc::getClassesOfFile(QString _fileName)
{

    QStringList loadedClasses= this->getClassNames();

    QFileInfo fileInfo(_fileName);
    QFileInfo fileInfo2;

    QStringList fileClasses;

    for(int i=0;i<loadedClasses.size();i++)
    {
        fileInfo2 = QFileInfo(getFileOfClass(loadedClasses.at(i)));

        if(fileInfo.absoluteFilePath()==fileInfo2.absoluteFilePath())
            fileClasses.append(loadedClasses.at(i));
    }

    return fileClasses;
}


void MOomc::getInheritedComponents(QString parentClass, QStringList & names, QStringList & classes)
{

    // First inherited classes
    QStringList firstInheritedClasses = getInheritedClasses(parentClass);


    int nbInheritance = firstInheritedClasses.size();
    QStringList curNames,curClasses;
    //call to getContainedComponents which will call this function (recursive algorithm)
    for(int i=0;i<nbInheritance;i++)
    {
        getContainedComponents(firstInheritedClasses.at(i),curNames,curClasses);
        names<<curNames;
        classes<<curClasses;
    }
}


QStringList MOomc::getElementInfos(QString parentClass)
{
    QString res = evalCommand("getElementsInfo("+parentClass+")");
    QStringList list;
    if(res == "Error" || res.size()<=2)
        return QStringList();
    else
    {
        res.remove("{");
        res.remove("}");
        res.remove(")");
        list = res.split("rec(",QString::SkipEmptyParts);

        LowTools::removeWhiteSpaceStrings(list);
    }

    return list;
}

void MOomc::readElementInfos(QString parentClass,QStringList &packagesClasses,QStringList &modelsClasses,QStringList &recordNames,QStringList &compsNames,QStringList &compsClasses)
{
    QString msg;
    msg.sprintf("Reading class infos : %s",parentClass.toLatin1().data());
    InfoSender::instance()->send(Info(msg,ListInfo::OMCNORMAL2));

    packagesClasses.clear();
    modelsClasses.clear();
    compsNames.clear();
    compsClasses.clear();
    recordNames.clear();

    QStringList elementsInfos = getElementInfos(parentClass);
    QString elementInfos;
    QStringList fields;


    QString type;
    QString compClass;
    QString compName;

    for(int iEl=0; iEl<elementsInfos.size();iEl++)
    {
        elementInfos = elementsInfos.at(iEl);
        fields = elementInfos.split(",");

        // get element type
        type = getValueFromElementInfo(elementInfos,"elementtype");

        // Package or Model
        if(type=="classdef")
        {
            //get classrestriction (MODEL or PACKAGE or RECORD)
            QString restr = getValueFromElementInfo(elementInfos,"classrestriction");

            //get classname
            QString className = getValueFromElementInfo(elementInfos,"classname");

            //add it
            if(restr.contains("MODEL"))
            {
                modelsClasses.push_back(className);
                QString msg;
                msg.sprintf("Adding model: %s, Type : %s",className.toLatin1().data(),restr.toLatin1().data());
                InfoSender::instance()->send(Info(msg,ListInfo::OMCNORMAL2));
            }
            if(restr.contains("PACKAGE"))
            {
                packagesClasses.push_back(className);
                QString msg;
                msg.sprintf("Adding package: %s, Type : %s",className.toLatin1().data(),restr.toLatin1().data());
                InfoSender::instance()->send(Info(msg,ListInfo::OMCNORMAL2));
            }
            if(restr.contains("RECORD"))
            {
                recordNames.push_back(className);
                QString msg;
                msg.sprintf("Adding record: %s, Type : %s",className.toLatin1().data(),restr.toLatin1().data());
                InfoSender::instance()->send(Info(msg,ListInfo::OMCNORMAL2));
            }


        }

        // Component
        if(type=="component")
        {
            //get component class
            compClass = getValueFromElementInfo(elementInfos,"typename");

            //get classname
            compName = getValueFromElementInfo(elementInfos,"names");
            compName.remove("{");
            compName.remove("}");
            compName = compName.split(",").at(0);

            compsClasses.push_back(compClass);
            compsNames.push_back(compName);
        }

    }
}

QString MOomc::getValueFromElementInfo(QString elementInfoLine,QString fieldName)
{

    QStringList fields = elementInfoLine.split(",");
    QRegExp regExp(".*"+fieldName+"=.*");

    int iField=fields.indexOf(regExp);
    if(iField>-1)
    {
        try{
            QString value = fields.at(iField).section("=",1,1);

            // removing whitespaces for non-path fields
            if(value.indexOf("\"")==-1)
            {
                value.remove(" ");
            }
            return value;
        }
        catch(std::exception &e)
        {
            return QString();
        }
    }
    return QString();
}
