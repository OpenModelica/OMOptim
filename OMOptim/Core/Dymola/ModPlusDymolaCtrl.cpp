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

@file ModPlusDymolaCtrl.cpp
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version


*/


#include "ModPlusDymolaCtrl.h"
#include "reportingHook.h"
#include "Project.h"
#include "ModModelPlus.h"
#include "MOomc.h"
#include "MOParameter.h"
#include "Dymola.h"
#include "LowTools.h"

#include "ModelPlus.h"


ModPlusDymolaCtrl::ModPlusDymolaCtrl(Project* project,ModModelPlus* model,MOomc* moomc)
    :ModPlusCtrl(project,model,moomc)
{
    _dsinFile = "dsin.txt";
    _dsresFile = "dsres.txt";
    _dsfinalFile = "dsfinal.txt";
    _copyAllMoOfFolder = true;
    _modModelPlus = model;

    _parameters = new MOParameters();
    DymolaParameters::setDefaultParameters(_parameters);

    connect(_parameters,SIGNAL(modified()),this,SIGNAL(modified()));
}

ModPlusDymolaCtrl::~ModPlusDymolaCtrl(void)
{
}

ModPlusCtrl* ModPlusDymolaCtrl::clone()
{
    ModPlusDymolaCtrl* cloned = new ModPlusDymolaCtrl(_project,_modModelPlus,_moomc);

    cloned->_dsinFile = _dsinFile;
    cloned->_dsresFile = _dsresFile;
    cloned->_dsfinalFile = _dsfinalFile;
    cloned->_copyAllMoOfFolder = _copyAllMoOfFolder;
    delete cloned->_parameters;
    cloned->_parameters = _parameters->clone();

    return cloned;
}

ModPlusCtrl::Type ModPlusDymolaCtrl::type() const
{
    return ModPlusCtrl::DYMOLA;
}

QString ModPlusDymolaCtrl::name()
{
    return "Dymola";
}


bool ModPlusDymolaCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString path)
{
    QDir folder(path);
    if(folder.path().isEmpty())
        folder = _ModelPlus->mmoFolder();

    int outputFile = _parameters->value(DymolaParameters::str(DymolaParameters::FINALFILE),DymolaParameters::DSFINAL).toInt();
    switch(outputFile)
    {
    case DymolaParameters::DSFINAL :
        return readOutputVariablesDSFINAL(finalVariables,folder.absoluteFilePath(_dsfinalFile));
    case DymolaParameters::DSRES :
        return readOutputVariablesDSRES(finalVariables,folder.absoluteFilePath(_dsresFile));
    }
    return false;
}

bool ModPlusDymolaCtrl::readOutputVariablesDSFINAL(MOVector<Variable> *finalVariables, QString dsfinalFile)
{
    InfoSender::sendCurrentTask("Reading final variables in "+dsfinalFile);

    finalVariables->clear();
    QFileInfo dsfinalInfo = QFileInfo(dsfinalFile);

    if(!dsfinalInfo.exists())
    {
        return false;
    }
    Dymola::getVariablesFromDsFile(dsfinalFile,finalVariables,_ModelPlus->modelName());

    InfoSender::eraseCurrentTask();
    return true;
}

bool ModPlusDymolaCtrl::readOutputVariablesDSRES(MOVector<Variable> *finalVariables, QString dsresFile)
{


    finalVariables->clear();

    if(dsresFile.isEmpty())
    {
        dsresFile = _ModelPlus->mmoFolder().absoluteFilePath(_dsresFile);
    }
    InfoSender::sendCurrentTask("Reading final variables in "+dsresFile);

    QFileInfo dsresInfo = QFileInfo(dsresFile);

    if (dsresInfo.exists())
    {
        Dymola::getFinalVariablesFromDsFile(dsresFile,finalVariables,_ModelPlus->modelName());
        InfoSender::eraseCurrentTask();
        return true;
    }
    else
    {
        InfoSender::eraseCurrentTask();
        return false;
    }
}

bool ModPlusDymolaCtrl::readInitialVariables(MOVector<Variable> *initVariables, const QFileInfoList filesToCopy,bool forceRecompile,QString dsinFile)
{
    bool authorizeRecreate=false;
    QString logFile = _ModelPlus->mmoFolder().absoluteFilePath("buildlog.txt");
    if(QFile::exists(logFile))
        LowTools::removeFile(logFile);

    if(dsinFile.isEmpty())
    {
        authorizeRecreate=true;
        dsinFile = _ModelPlus->mmoFolder().absoluteFilePath(_dsinFile);
    }
    InfoSender::sendCurrentTask("Reading initial variables in "+dsinFile);

    initVariables->clear();

    if((!QFile::exists(dsinFile) && authorizeRecreate)||forceRecompile)
    {
        //********************
        // update dependencies
        //********************
        QFileInfoList moDeps = _modModelPlus->moDependencies();
        // 1- add project loaded mo files
        QFileInfoList projectMoFiles = _project->moFiles();
        for(int i=0;i<projectMoFiles.size();i++)
            if(!moDeps.contains(projectMoFiles.at(i)))
                moDeps.push_back(projectMoFiles.at(i));

        // 2 - remove modelica library from moDependencies since Dymola
        // automatically loads it
        moDeps.removeAll(_project->moomc()->getFileOfClass("Modelica"));

        createDsin(moDeps,filesToCopy);
    }
    InfoSender::eraseCurrentTask();

    if(!QFile::exists(dsinFile))
    {
        QString msg = "Unable to create DsinFile. See <A href=\"file:///"+ logFile +"\">log file</A> for detail.";
        InfoSender::instance()->send(Info(msg,ListInfo::ERROR2));
        return false;
    }
    else
    {
        Dymola::getVariablesFromDsFile(dsinFile,initVariables,_ModelPlus->modelName());
        InfoSender::instance()->send(Info(ListInfo::READVARIABLESSUCCESS));
        return true;
    }
}

bool ModPlusDymolaCtrl::compile(const QFileInfoList & moDependencies, const QFileInfoList filesToCopy)
{
    InfoSender::sendCurrentTask("Dymola : Compiling model "+_ModelPlus->modelName());

    //QString logFilePath = _mmoFolder+QDir::separator()+"log.html";
    QString logFilePath = _ModelPlus->mmoFolder().absoluteFilePath("buildlog.txt");

    //********************
    // update dependencies
    //********************
    QFileInfoList moDeps = moDependencies;
    // 1- add project loaded mo files
    QFileInfoList projectMoFiles = _project->moFiles();
    for(int i=0;i<projectMoFiles.size();i++)
        if(!moDeps.contains(projectMoFiles.at(i)))
            moDeps.push_back(projectMoFiles.at(i));

    // 2 - remove modelica library from moDependencies since Dymola
    // automatically loads it
    moDeps.removeAll(_project->moomc()->getFileOfClass("Modelica"));

    // compile
    bool success = Dymola::compile(_modModelPlus->moFilePath(),_modModelPlus->modelName(),
                                    _ModelPlus->mmoFolder(),logFilePath,moDeps,
                                   QFileInfoList() << _modModelPlus->neededFiles() << filesToCopy);

    // Inform
    ListInfo::InfoNum iMsg;
    if(success)
        iMsg = ListInfo::MODELCOMPILATIONSUCCESS;
    else
        iMsg = ListInfo::MODELCOMPILATIONFAIL;


    // Infos
    InfoSender::instance()->send(Info(iMsg,_ModelPlus->modelName(),logFilePath));

    InfoSender::instance()->eraseCurrentTask();

    return success;
}



bool ModPlusDymolaCtrl::isCompiled()
{
    bool filesExist = true;
    QStringList filesNeeded;
    filesNeeded << "dsin.txt" << "dymosim.exe";

    QString filePath;
    for(int i=0;i<filesNeeded.size();i++)
    {
        filesExist = filesExist && _ModelPlus->mmoFolder().exists(filesNeeded.at(i));
    }
    return filesExist;
}

/** Uncompile model: remove compiled version.
  * Returns true if it has been successful or if there were no compiled version.
  */
bool ModPlusDymolaCtrl::uncompile()
{
    // Info
    InfoSender::instance()->sendNormal("Removing compiled model : "+name());

    // first remove initfile
    QFileInfo initFile(_ModelPlus->mmoFolder(),"dsin.txt");
    QFile::remove(initFile.absoluteFilePath());

    // remove exeFile
    QFileInfo exeFile(_ModelPlus->mmoFolder(),"dymosim.exe");
    if(!exeFile.exists())
        return true;

    return QFile::remove(exeFile.absoluteFilePath());
}

bool ModPlusDymolaCtrl::simulate(QDir tempDir,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,QFileInfoList filesToCopy,QFileInfoList moDependencies)
{
    // Info
    InfoSender::sendCurrentTask("Dymola : Simulating model "+_ModelPlus->modelName());


    // clear outputVars
    outputVars->clear();

    bool compileOk = isCompiled();
    // eventually compile model
    if(!compileOk)
        compileOk = compile(moDependencies,filesToCopy);

    if(!compileOk)
        return false; // compilation failed, useless to pursue


    // Create tempDir
    bool createTmpDirOk = LowTools::mkpath(tempDir.absolutePath(),true);
    if(!createTmpDirOk)
    {
        InfoSender::instance()->sendWarning("Failed to create/clear temp directory : "+tempDir.absolutePath());
        return false;
    }

    /// copy files in temp dir (\todo : optimize with a config.updateTempDir in case of several consecutive launches)
    QFileInfoList allFilesToCopy;
    QDir mmoDir = QDir(_ModelPlus->mmoFolder());
    allFilesToCopy << mmoDir.filePath("dsin.txt") << mmoDir.filePath("dymosim.exe");
    allFilesToCopy.append(filesToCopy);

    bool copyOk = LowTools::copyFilesInFolder(allFilesToCopy,tempDir);
    if(!copyOk)
        return false;

    // remove previous dymola log files
    QStringList filesToRemove;
    filesToRemove << "status" << "failure" << "success" << "dslog.txt";
    for(int i=0;i<filesToRemove.size();i++)
        tempDir.remove(filesToRemove.at(i));

    QString tempDsin = tempDir.absoluteFilePath("dsin.txt");

    // Specifying new Variables values in dymosim input file
    QString errMsg;
    bool setDsinOk = Dymola::setVariablesToDsin(tempDsin,_ModelPlus->modelName(),updatedVars,_parameters,errMsg);
    if(!setDsinOk)
    {
        InfoSender::instance()->sendWarning("Simulation failed : "+errMsg);
        return false;
    }
    // Launching Dymosim
    int maxNSec=_parameters->value(DymolaParameters::str(DymolaParameters::MAXSIMTIME),-1).toInt();
    QString startErrMsg;
    bool startOk = Dymola::start(tempDir,_simProcess,startErrMsg,maxNSec);

    if(!startOk)
    {
        InfoSender::instance()->sendWarning("Simulation failed : "+startErrMsg);
        return false;
    }

    QString logFile = tempDir.absoluteFilePath("dslog.txt");

    //getting results
    //Checking if successed
    bool success=QFile::exists(tempDir.absoluteFilePath("success"));

    if(!success)
    {
        InfoSender::instance()->send(Info(ListInfo::ONESIMULATIONFAILED,logFile));
        return false;
    }
    else
    {
        InfoSender::instance()->send(Info(ListInfo::ONESIMULATIONSUCCESS,logFile));
    }

    bool readOk = readOutputVariables(outputVars,tempDir.absolutePath());
    InfoSender::eraseCurrentTask();
    return readOk;

}

bool ModPlusDymolaCtrl::setStopTime(double time)
{
    _parameters->setValue(DymolaParameters::str(DymolaParameters::STOPTIME),time);
    return true;
}

void ModPlusDymolaCtrl::stopSimulation()
{
    _simProcess.kill();
}

bool ModPlusDymolaCtrl::canBeStoped()
{
    return true;
}

bool ModPlusDymolaCtrl::createDsin(QFileInfoList moDeps,QFileInfoList filesToCopy)
{

    QDir dir(_ModelPlus->mmoFolder());


    // copy dependencies

    QFileInfo moFilePath = _modModelPlus->moFilePath();

    QStringList depFileNames = _moomc->getDependenciesPaths(moFilePath.absoluteFilePath(),false);
    for(int i=0;i<depFileNames.size();i++)
    {
        // check if file exists
        QFileInfo depFileInfo(depFileNames.at(i));
        QString depFilePath;
        if(!depFileInfo.exists())
        {
            // check in folder
            depFileInfo = QFileInfo(_ModelPlus->mmoFolder().absolutePath()+QDir::separator()+depFileNames.at(i));
        }

        // copy
        if(depFileInfo.exists())
        {
            QFile depFile(depFileInfo.absoluteFilePath());
            depFile.copy(dir.absoluteFilePath(depFileInfo.fileName()));
        }
    }

    // compile
    bool success = Dymola::createDsin(_modModelPlus->moFilePath(),_ModelPlus->modelName(),_ModelPlus->mmoFolder(),
                                      moDeps,QFileInfoList() << _ModelPlus->neededFiles() << filesToCopy);


    // Return
    return success;
}



QList<ModelPlus::ModelType> ModPlusDymolaCtrl::compatibleModels()
{
    QList<ModelPlus::ModelType> result;
    result << ModelPlus::MODELICA;
    return result;
}
