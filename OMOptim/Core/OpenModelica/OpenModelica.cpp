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
#include "ModPlusOMCtrl.h"
#include "VariableType.h"
#include "LowTools.h"
#include "OpenModelicaParameters.h"
#include "omc_config.h"
#include "Utilities.h"

// for .mat reading
extern "C" {
#include "read_matlab4.h"
#include "read_matlab4.c"
}

OpenModelica::OpenModelica()
{

}

OpenModelica::~OpenModelica(void)
{
}

bool OpenModelica::compile(MOomc *_omc,QFileInfo moFile,QString modelToConsider,QDir storeFolder,const QFileInfoList & moDeps, QFileInfoList neededFiles, QFileInfoList neededFolders)
{
    // check if model already loaded
    QFileInfo loadedMoFile = _omc->getFileOfClass(modelToConsider);

    bool loadOk;
    QString loadError;

    // load moDependencies if not already loaded
    // forceLoad = false
    for(int i=0;i<moDeps.size();i++)
        _omc->loadModel(moDeps.at(i).absoluteFilePath(),false,loadOk,loadError);

    // if not already loaded, reload
    if(loadedMoFile != moFile)
    {
        _omc->loadModel(moFile.absoluteFilePath(),true,loadOk,loadError);
    }

    // Create working dir
    QString tmpPath = _omc->getWorkingDirectory();

    //    if(workDir.exists())
    //        LowTools::removeDirContents(tmpPath);
    //    else
    //    {
    //        QDir dir;
    //        dir.mkpath(tmpPath);
    //    }


    if(!QDir(tmpPath).exists())
        QDir().mkpath(tmpPath);
    QDir workDir(tmpPath);

    // Copy file and folder
    LowTools::copyFilesInFolder(neededFiles,workDir.absolutePath());

    _omc->changeDirectory(tmpPath);

    QString exeFile;
    QString initFile;
    bool success =  _omc->buildModel(modelToConsider,exeFile,initFile);

    QFileInfoList filesToCopy;
    filesToCopy << QFileInfo(exeFile) << QFileInfo(initFile);

    LowTools::copyFilesInFolder(filesToCopy,storeFolder);
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
    QRegExp rx("^(\\S+)\\s*//(\\S+)\\s*$");
    QRegExp rxDefault("^(\\S+)\\s*//[\\w||\\s]+//(\\S+)\\s*$");
    QStringList fields;
    while (!line.isEmpty()){
        if(rx.indexIn(line)>-1)
        {
            fields = rx.capturedTexts();
            newVariable = new Variable();
            newVariable->setName(rx.cap(2));
            newVariable->setModel(_modelName);
            newVariable->setValue(rx.cap(1).toDouble());
            variables->addItem(newVariable);
            //get datatype
            newVariable->setDataType(_omc->getPrimitiveDataType(newVariable->name()));
        }
        else if(rxDefault.indexIn(line)>-1)
        {
            fields = rxDefault.capturedTexts();
            newVariable = new Variable();
            newVariable->setName(rxDefault.cap(2));
            newVariable->setModel(_modelName);
            newVariable->setValue(rxDefault.cap(1).toDouble());
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
        // InfoSender::instance()->send( Info(ListInfo::PROBLEMFILENOTEXISTS,filePath));
        return false;
    }
    else if( !doc.setContent(&file,&error) )
    {
        file.close();
        // InfoSender::instance()->send( Info(ListInfo::PROBLEMFILECORRUPTED,error,filePath));
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
    bool allok = true;
    for(int i=0;i<listScalarVars.size();i++)
    {
        Variable* newVar = variableFromFmi(listScalarVars.at(i).toElement(),modModelName,ok);
        if(ok)
            variables->addItem(newVar);
        allok = allok && ok;
    }

    return allok;
}

Variable* OpenModelica::variableFromFmi(const QDomElement & el,QString modelName,  bool & ok)
{
    ok = true;
    QString name = el.attribute("name");
    if(name.isEmpty())
    {
        ok = false;
        return NULL;
    }

    Variable* newVar = new Variable();
    newVar->setName(name);
    newVar->setModel(modelName);
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

    //look for causality
    newVar->setCausality(varCausality(el));

    if(!ok)
    {
        delete newVar;
        return NULL;
    }
    else
        return newVar;
}

VariableCausality OpenModelica::varCausality(const QDomElement & el)
{
    QString variability = el.attribute("variability");
    QString causality = el.attribute("causality");

    if(!variability.compare("parameter") || !causality.compare("input") )
        return OMINPUT;
    else
        return OMOUTPUT;
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


/**
  * Uses OpenModelica functions to read from .mat files : faster than csv (both writing and reading)
  * @note : don't forget to free reader (omc_free_matlab4_reader()) in order to let files writable
  */
bool OpenModelica::getFinalVariablesFromMatFile(QString fileName, MOVector<Variable> * variables,QString _modelName)
{

    ModelicaMatReader reader;
    ModelicaMatVariable_t *var;
    const char* msg;
    double value;
    bool varOk;
    int status ;
    Variable* newVar;

    //Read in mat file
    if(0 != (msg = omc_new_matlab4_reader(fileName.toStdString().c_str(), &reader)))
    {
        InfoSender::instance()->sendError("Unable to read .mat file: "+QString(msg));

#ifdef WIN32 // don't know why sigabrt on linux. Should look for.
        delete[] msg;
#endif
        return false;
    }

   // delete[] msg;
    //Read in timevector
    double stopTime =  omc_matlab4_stopTime(&reader);

    if (reader.nvar < 1)
    {
        InfoSender::instance()->sendError("Unable to read .mat file : no variable inside");
        omc_free_matlab4_reader(&reader);
        return false;
    }

    // read in all values
    for (int i = 0; i < reader.nall; i++)
    {
        newVar = new Variable();
        QString name(reader.allInfo[i].name);
        newVar->setName(name);
        newVar->setModel(_modelName);


        // read the variable final value
        var = omc_matlab4_find_var(&reader, reader.allInfo[i].name);


        if (!var->isParam)
        {
            status = omc_matlab4_val(&value,&reader,var,stopTime);
        }
        // if variable is a parameter then take at first step
        else
        {
            status = omc_matlab4_val(&value,&reader,var,0.0);
        }


        varOk = (status==0);
        if(!varOk && reader.nrows>0)
        {
            double *vals = omc_matlab4_read_vals(&reader,var->index);
            value = vals[reader.nrows-1];
            varOk = true;
        }
        newVar->setValue(value);


        if(varOk)
            variables->addItem(newVar);
        else
        {
            InfoSender::instance()->debug("Couldn't find value of variable in .mat file :" + newVar->name());
            delete newVar;
        }
    }


    omc_free_matlab4_reader(&reader);
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
        newVar->setName(varNames.at(i));
        newVar->setModel(_modelName);
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
            varName = curVar->name(Variable::SHORT);
            //varName = varName.remove(modModelName+".");
            rxLine.setPattern(sciNumRx()+"\\s*(//[\\w*|\\s*]*//|//)\\s*"+varName);
            index = rxLine.indexIn(allText);

            if(index>-1)
            {
                fields = rxLine.capturedTexts();
                newLine = curVar->getFieldValue(Variable::VALUE).toString() +"\t";
                newLine += fields.at(2)+varName;
                allText = allText.replace(rxLine.cap(0),newLine);
            }
            else
            {
                InfoSender::instance()->send(Info("Warning : unable to set variable value (not found in init file):"+varName,ListInfo::ERROR2));
            }
        }

        // Parameters to write in init file
        /// @deprecated Now OM uses xml input.
        if(parameters)
        {
            QList<OpenModelicaParameters::Parameters> initParameters; // parameters to specify in init file
            initParameters << OpenModelicaParameters::STOPTIME;

            QStringList paramIndicators;
            paramIndicators << "stop value";


            QVariant paramValue;
            QString paramName;
            MOParameter * curParam;
            int iP;
            for(int i=0;i<initParameters.size();i++)
            {
                curParam = parameters->findItem(OpenModelicaParameters::str(initParameters.at(i)));
                if(curParam)
                {
                    paramName = paramIndicators.at(i);
                    paramValue = curParam->getFieldValue(MOParameter::VALUE);
                    rxLine.setPattern(sciNumRx()+"\\s*(//[\\w*|\\s*]*//|//)\\s*"+paramName);
                    index = rxLine.indexIn(allText);

                    if(index>-1)
                    {
                        fields = rxLine.capturedTexts();
                        newLine = paramValue.toString() +"\t";
                        newLine += fields.at(2)+paramName;
                        allText = allText.replace(rxLine.cap(0),newLine);
                    }
                    else
                    {
                        InfoSender::instance()->send(Info("Warning : unable to set parameter value (not found in init file):"+paramName,ListInfo::ERROR2));
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
        InfoSender::instance()->debug("Model input file (xml) load failed");
        return false;
    }
    else if( !doc.setContent(&file,&error) )
    {
        InfoSender::instance()->sendError("Model input file (xml) is corrupted ! ["+error+"]");
        file.close();
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

    ok = file.open(QIODevice::WriteOnly);
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
        localVarName = variables->at(i)->name(Variable::SHORT);
        //localVarName = localVarName.remove(modelName+".");

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
                newType.setAttribute("start",variables->at(i)->value().toString());
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
    QDomElement xfmi = doc.firstChildElement("fmiModelDescription");
    QDomElement oldxfmi = xfmi;

    QDomElement xExp = xfmi.firstChildElement("DefaultExperiment");
    QDomElement oldxExp = xExp;


    double starttime = parameters->value(OpenModelicaParameters::str(OpenModelicaParameters::STARTTIME),0).toDouble();
    double stoptime = parameters->value(OpenModelicaParameters::str(OpenModelicaParameters::STOPTIME),1).toDouble();
    int nbIntervals = parameters->value(OpenModelicaParameters::str(OpenModelicaParameters::NBINTERVALS),2).toInt();
    double stepSize = (stoptime-starttime)/nbIntervals;


    MOParameter * curParam;
    MOParameterListed* curParamListed;
    for(int i=0;i<parameters->size();i++)
    {
        curParam = parameters->at(i);

        if(!curParam->name().contains(" ")) // remove old parameters, temporary
        {
            if(curParam->name()==OpenModelicaParameters::str(OpenModelicaParameters::SOLVER))
            {
                curParamListed = dynamic_cast<MOParameterListed*>(curParam);
                xExp.setAttribute(curParamListed->name(),curParamListed->strValue());
            }
            else if(curParam->name()==OpenModelicaParameters::str(OpenModelicaParameters::OUTPUT))
            {
                curParamListed = dynamic_cast<MOParameterListed*>(curParam);
                xExp.setAttribute(curParamListed->name(),curParamListed->strValue());
            }
            else if (curParam->name()==OpenModelicaParameters::str(OpenModelicaParameters::NBINTERVALS))
            {
                xExp.setAttribute("stepSize",QString::number(stepSize));
            }
            else
            {
                xExp.setAttribute(curParam->name(),curParam->value().toString());
            }
        }
    }

    // update xfmi with new vars
    xfmi.replaceChild(xExp,oldxExp);
    doc.replaceChild(xfmi,oldxfmi);
}


bool OpenModelica::start(QString exeFile,QString &errMsg,int maxnsec)
{

    QFileInfo exeFileInfo(exeFile);
    QString exeDir = exeFileInfo.absolutePath();
    if(!QFile::exists(exeFile))
    {
        errMsg = "Cannot find model executable file : " + exeFile;
        return false;
    }

    QProcess simProcess;
    simProcess.setWorkingDirectory(exeDir);


#ifdef WIN32
    QString appPath = "\""+exeFile+"\"";
    // add OM path in PATH
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString mingw = Utilities::getMinGWBinPath();
    QString om = Utilities::getOMBinPath();
    env.insert("PATH", env.value("PATH") + ";"+om+";"+mingw);

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
        errMsg = "Simulation process failed or time limit reached";
        simProcess.close();
        return false;
    }

    QString output(simProcess.readAllStandardOutput());
    InfoSender::instance()->send(Info(output,ListInfo::OMCNORMAL2));
    return ok;
}

QString OpenModelica::sciNumRx()
{
    QString rx = "([+-]?[0-9]*\\.?[0-9]+|[+-]?[0-9]+\\.?[0-9]*[eE][+-]?[0-9]+)";
    return rx;
}


QString OpenModelica::home()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString omHome = env.value("OPENMODELICAHOME");
    return omHome;
}

/**
  * Tries to find library path corresponding to version set as parameter.
  * If not found return newest library path.
  */
QString OpenModelica::getLibraryPath(QString version)
{
    const char *omlibrary = getenv("OPENMODELICALIBRARY");
    QString omLibraryFolder(omlibrary);

    QDir omLibDir(omlibrary);
    QStringList omLibDirs = omLibDir.entryList(QDir::AllDirs| QDir::NoDotAndDotDot);
    omLibDirs = omLibDirs.filter(QRegExp("^Modelica .*"));
    if(omLibDirs.isEmpty())
        return QString();
    else
    {
        int iVersion = omLibDirs.indexOf("Modelica "+version);
        if(iVersion>-1)
            return omLibDir.absoluteFilePath(omLibDirs.at(iVersion)+QDir::separator()+"package.mo");

        else
        {
            omLibDirs.sort();
            return omLibDir.absoluteFilePath(omLibDirs.last()+QDir::separator()+"package.mo");
        }
    }
}
