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

@file ModPlusOMCtrl.cpp
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version


*/
#include "ModPlusOMCtrl.h"
#include "ModModelPlus.h"
#include "ModModel.h"
#include "MOParameter.h"
#include "Project.h"
#include "LowTools.h"
#include "OpenModelicaParameters.h"

ModPlusOMCtrl::ModPlusOMCtrl(Project* project,ModModelPlus* modModelPlus,MOomc* moomc)
    :ModPlusCtrl(project,modModelPlus,moomc)
{

    _initFileTxt = _ModelPlus->modelName()+"_init.txt";
    _initFileXml = _ModelPlus->modelName()+"_init.xml";


#ifdef WIN32
    _exeFile = _ModelPlus->modelName()+".exe";
#else
    _exeFile = _ModelPlus->modelName();
#endif

    _copyAllMoOfFolder = true;
    _modModelPlus = modModelPlus;

    _parameters = new MOParameters();
    OpenModelicaParameters::setDefaultParameters(_parameters);
}

ModPlusOMCtrl::~ModPlusOMCtrl(void)
{
}

ModPlusCtrl* ModPlusOMCtrl::clone()
{
    ModPlusOMCtrl* cloned = new ModPlusOMCtrl(_project,(ModModelPlus*)_ModelPlus,_moomc);

    cloned->_initFileTxt = _initFileTxt;
    cloned->_initFileXml = _initFileXml;
    cloned->_exeFile = _exeFile;
    cloned->_copyAllMoOfFolder = _copyAllMoOfFolder;

    delete cloned->_parameters;
    cloned->_parameters = _parameters->clone();
    cloned->_modModelPlus = _modModelPlus;


    return cloned;
}

ModPlusCtrl::Type ModPlusOMCtrl::type() const
{
    return ModPlusCtrl::OPENMODELICA;
}

QString ModPlusOMCtrl::name()
{
    return "Open Modelica";
}

bool ModPlusOMCtrl::useMat()
{
    return _parameters->value(OpenModelicaParameters::str(OpenModelicaParameters::OUTPUT),true).toBool();
}

QString ModPlusOMCtrl::resFile()
{
    if(useMat())
        return _ModelPlus->modelName()+"_res.mat";
    else
        return _ModelPlus->modelName()+"_res.csv";
}

// Parameters


bool ModPlusOMCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString resFileLocal)
{
    InfoSender::instance()->send(Info("Reading final variables in "+resFileLocal,ListInfo::NORMAL2));

    if(resFileLocal.isEmpty())
        resFileLocal = resFile();

    if(useMat())
        return OpenModelica::getFinalVariablesFromMatFile(resFileLocal,finalVariables,_ModelPlus->modelName());
    else
        return OpenModelica::getFinalVariablesFromFile(resFileLocal,finalVariables,_ModelPlus->modelName());
}


bool ModPlusOMCtrl::readInitialVariables(MOVector<Variable> *initVariables, const QFileInfoList filesToCopy,bool forceRecompile, QString initFile)
{
    QString initFileTxt;
    QString initFileXml;

    bool authorizeRecreate=false;
    if(initFile.isEmpty())
    {
        authorizeRecreate=true;
        initFileTxt = _ModelPlus->mmoFolder().absoluteFilePath(_initFileTxt);
        initFileXml = _ModelPlus->mmoFolder().absoluteFilePath(_initFileXml);
    }
    else
    {
        if(initFile.right(3)=="xml")
            initFileXml = initFile;

        if(initFile.right(3)=="txt")
            initFileTxt = initFile;
    }

    initVariables->clear();

    if((!QFile::exists(initFileXml)&& !QFile::exists(initFileTxt)&& authorizeRecreate)||forceRecompile)
    {
        createInitFile(_modModelPlus->moDependencies(),filesToCopy);
    }

    if(!QFile::exists(initFileXml)&& !QFile::exists(initFileTxt))
    {
        return false;
    }
    else
    {
        if(QFile::exists(initFileXml))
        {
            OpenModelica::getInputVariablesFromXmlFile(_moomc,initFileXml,_ModelPlus->modelName(),initVariables);
            InfoSender::instance()->send(Info("Reading initial variables in "+initFileXml,ListInfo::NORMAL2));
        }
        else if(QFile::exists(initFileTxt))
        {
            OpenModelica::getInputVariablesFromTxtFile(_moomc,initFileTxt,initVariables,_ModelPlus->modelName());
            InfoSender::instance()->send(Info("Reading initial variables in "+initFileTxt,ListInfo::NORMAL2));
        }
        return true;
    }

    return true;
}

bool ModPlusOMCtrl::compile(const QFileInfoList & moDeps, const QFileInfoList filesToCopy)
{
    InfoSender::sendCurrentTask("Open Modelica : Compiling model "+_ModelPlus->modelName());

    // compile

    QString logFile = _ModelPlus->mmoFolder().absolutePath()+_ModelPlus->modelName()+".log";
    bool success = OpenModelica::compile(_moomc,_modModelPlus->moFilePath(),_ModelPlus->modelName(),_ModelPlus->mmoFolder(),
                                         moDeps,QFileInfoList() << _ModelPlus->neededFiles() << filesToCopy,
                                         _ModelPlus->neededFolders());

    // Inform
    if(success)
        InfoSender::instance()->send(Info(ListInfo::MODELCOMPILATIONSUCCESS,_ModelPlus->modelName(),logFile));
    else
        InfoSender::instance()->send(Info("Model "+_ModelPlus->modelName()+" failed to compile. See OMC log tab for details.",ListInfo::ERROR2));

    InfoSender::instance()->eraseCurrentTask();

    return success;
}


bool ModPlusOMCtrl::isCompiled()
{
    bool filesExist = true;
    QStringList filesNeeded;
    filesNeeded << _exeFile;

    QString filePath;
    QFile file;

    for(int i=0;i<filesNeeded.size();i++)
    {
        filePath = _ModelPlus->mmoFolder().absoluteFilePath(filesNeeded.at(i));
        file.setFileName(filePath);
        filesExist = filesExist && file.exists();
    }
    return filesExist;
}

/** Uncompile model: remove compiled version.
  * Returns true if it has been successful or if there were no compiled version.
  */
bool ModPlusOMCtrl::uncompile()
{
    // info
    InfoSender::instance()->sendNormal("Removing compiled model : "+name());

    // first remove initfile
    QFileInfo initFile(_ModelPlus->mmoFolder(),_initFileXml);
    LowTools::removeFile(initFile.absoluteFilePath());

    // remove exeFile
    QFileInfo exeFile(_ModelPlus->mmoFolder(),_exeFile);
    if(!exeFile.exists())
        return true;

    return LowTools::removeFile(exeFile.absoluteFilePath());
}


bool ModPlusOMCtrl::simulate(QDir tempFolder,MOVector<Variable> * inputVars,MOVector<Variable> * outputVars,QFileInfoList filesToCopy,QFileInfoList moDependencies)
{
    // Info
    InfoSender::sendCurrentTask("Open Modelica : Simulating model "+_ModelPlus->modelName());

    // clear outputVars
    outputVars->clear();

    bool compileOk = isCompiled();
    // eventually compile model
    if(!compileOk)
        compileOk = compile(_modModelPlus->moDependencies(),filesToCopy);

    if(!compileOk)
        return false; // compilation failed, useless to pursue

    // Create tempDir
    LowTools::mkpath(tempFolder.absolutePath(),false);

    /// copy files in temp dir (\todo : optimize with a config.updateTempDir in case of several consecutive launches)
    QFileInfoList allFilesToCopy;
    QDir mmoDir = QDir(_ModelPlus->mmoFolder());
    allFilesToCopy << mmoDir.filePath(_exeFile);
    allFilesToCopy.append(filesToCopy);
    bool xml=mmoDir.exists(_initFileXml);
    allFilesToCopy << mmoDir.filePath(_initFileXml);

    InfoSender::instance()->debug("Start copying in temp directory : "+tempFolder.absolutePath());

    //QFile fileToCopy;
    bool copyOk = LowTools::copyFilesInFolder(allFilesToCopy,tempFolder);
    if(!copyOk)
        return false;


    // remove previous log / result files
    QStringList filesToRemove;
    filesToRemove << resFile();
    for(int i=0;i<filesToRemove.size();i++)
        tempFolder.remove(filesToRemove.at(i));

    QString tempInitFileXml = tempFolder.absoluteFilePath(_initFileXml);
    QString tempResFile = tempFolder.absoluteFilePath(resFile());
    QString tempExeFile = tempFolder.absoluteFilePath(_exeFile);

    // Specifying new Variables values in OM input file
    bool setInputFileOk = OpenModelica::setInputXml(tempInitFileXml,inputVars,_ModelPlus->modelName(),parameters());
    if(!setInputFileOk)
    {
        InfoSender::instance()->sendWarning("Simulation failed : failed to set input file");
        return false;
    }

    // Launching openmodelica
    int maxNSec=_parameters->value(OpenModelicaParameters::str(OpenModelicaParameters::MAXSIMTIME),-1).toInt();

    QString startErrMsg;
    bool startOk = OpenModelica::start(tempExeFile,startErrMsg,maxNSec);
    if(!startOk)
    {
        InfoSender::instance()->sendWarning("Simulation failed : "+startErrMsg);
        return false;
    }

    InfoSender::eraseCurrentTask();

    //getting results
    //Checking if successed
    bool success=QFile::exists(tempResFile);

    if(!success)
        return false;

    bool readOk = readOutputVariables(outputVars,tempResFile);
    return readOk;
}

bool ModPlusOMCtrl::setStopTime(double time)
{
    _parameters->setValue(OpenModelicaParameters::str(OpenModelicaParameters::STOPTIME),time);
    return true;
}

bool ModPlusOMCtrl::createInitFile(const QFileInfoList & moDeps,const QFileInfoList filesToCopy)
{
    return compile(moDeps,filesToCopy);
}


QList<ModelPlus::ModelType> ModPlusOMCtrl::compatibleModels()
{
    QList<ModelPlus::ModelType> result;
    result << ModelPlus::MODELICA;
    return result;
}
