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

ModPlusDymolaCtrl::ModPlusDymolaCtrl(Project* project,ModModelPlus* model,MOomc* moomc,QString mmoFolder,QString moFilePath,QString modModelName)
    :ModPlusCtrl(project,model,moomc,mmoFolder,moFilePath,modModelName)
{
    _dsinFile = "dsin.txt";
    _dsresFile = "dsres.txt";
    _dsfinalFile = "dsfinal.txt";
    _copyAllMoOfFolder = true;
    _outputReadMode = DSFINAL;

    _parameters = new MOParameters();
    setDefaultParameters();
}

ModPlusDymolaCtrl::~ModPlusDymolaCtrl(void)
{
}

ModPlusCtrl::Type ModPlusDymolaCtrl::type()
{
    return ModPlusCtrl::DYMOLA;
}

QString ModPlusDymolaCtrl::name()
{
    return "Dymola";
}

void ModPlusDymolaCtrl::setDefaultParameters()
{
    _parameters->addItem(new MOParameter((int)Dymola::STOPTIME,"stop time","Stop time",1,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
    _parameters->addItem(new MOParameter((int)Dymola::TOLERANCE,"tolerance","Tolerance",1E-4,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
    _parameters->addItem(new MOParameter((int)Dymola::NINTERVAL,"nInterval","nInterval",500,MOParameter::INT,0,std::numeric_limits<int>::max()));


    QMap<int,QString> mapSolvers;
    mapSolvers.insert(Dymola::DASSL,"Dassl");
    mapSolvers.insert(Dymola::EULER,"Euler");

    _parameters->addItem( new MOParameterListed((int)Dymola::SOLVER,"Solver","Solver",Dymola::DASSL,mapSolvers));
    _parameters->addItem(new MOParameter((int)Dymola::MAXSIMTIME,"MaxSimTime","Maximum time allowed for simulation (-1 : no limit)",-1,MOParameter::INT,-1,std::numeric_limits<int>::max()));
}

bool ModPlusDymolaCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString folder)
{
    if(folder.isEmpty())
        folder = _mmoFolder;
    QDir dir(folder);

    switch(_outputReadMode)
    {
    case DSFINAL :
        return readOutputVariablesDSFINAL(finalVariables,dir.absoluteFilePath(_dsfinalFile));
    case DSRES :
        return readOutputVariablesDSRES(finalVariables,dir.absoluteFilePath(_dsresFile));
    }
    return false;
}

bool ModPlusDymolaCtrl::readOutputVariablesDSFINAL(MOVector<Variable> *finalVariables, QString dsfinalFile)
{
    infoSender.send(Info("Reading final variables in "+dsfinalFile,ListInfo::NORMAL2));

    finalVariables->clear();
    QFileInfo dsfinalInfo = QFileInfo(dsfinalFile);

    if(!dsfinalInfo.exists())
    {
        return false;
    }
    Dymola::getVariablesFromDsFile(dsfinalFile,finalVariables,_modModelName);

    return true;
}

bool ModPlusDymolaCtrl::readOutputVariablesDSRES(MOVector<Variable> *finalVariables, QString dsresFile)
{
    infoSender.send(Info("Reading final variables in "+dsresFile,ListInfo::NORMAL2));

    finalVariables->clear();

    if(dsresFile.isEmpty())
    {
        dsresFile = _mmoFolder+QDir::separator()+_dsresFile;
    }

    QFileInfo dsresInfo = QFileInfo(dsresFile);

    if (dsresInfo.exists())
    {
        Dymola::getFinalVariablesFromDsFile(dsresFile,finalVariables,_modModelName);
        return true;
    }
    else
        return false;
}

bool ModPlusDymolaCtrl::readInitialVariables(MOVector<Variable> *initVariables,QString dsinFile)
{


    bool authorizeRecreate=false;
     QString logFile = _mmoFolder+QDir::separator()+ "buildlog.txt";
     if(QFile::exists(logFile))
         QFile::remove(logFile);

    if(dsinFile.isEmpty())
    {
        authorizeRecreate=true;
        dsinFile = _mmoFolder+QDir::separator()+_dsinFile;
    }

    infoSender.send(Info("Reading initial variables in "+dsinFile,ListInfo::NORMAL2));

    initVariables->clear();

    if(!QFile::exists(dsinFile) && authorizeRecreate)
    {
        createDsin();
    }

    if(!QFile::exists(dsinFile))
    {
        QString msg = "Unable to create DsinFile. See <A href=\"file:///"+ logFile +"\">log file</A> for detail.";
              infoSender.send(Info(msg,ListInfo::ERROR2));
        return false;
    }
    else
    {
        Dymola::getVariablesFromDsFile(dsinFile,initVariables,_modModelName);
        infoSender.send(Info(ListInfo::READVARIABLESSUCCESS));
        return true;
    }
}

bool ModPlusDymolaCtrl::compile(const QStringList & moDependencies)
{

    infoSender.send(Info("Compiling model "+_modModelName,ListInfo::NORMAL2));

    //QString logFilePath = _mmoFolder+QDir::separator()+"log.html";
    QString logFilePath = _mmoFolder+QDir::separator()+"buildlog.txt";

    // compile
    bool success = Dymola::firstRun(_project->moFiles(),_modModelName,_mmoFolder,logFilePath,moDependencies);

    // Inform
    ListInfo::InfoNum iMsg;
    if(success)
        iMsg = ListInfo::MODELCOMPILATIONSUCCESS;
    else
        iMsg = ListInfo::MODELCOMPILATIONFAIL;

    infoSender.send(Info(iMsg,_modModelName,logFilePath));

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
        filePath = _mmoFolder+QDir::separator()+filesNeeded.at(i);
        filesExist = filesExist && QFile::exists(filePath);
    }
    return filesExist;
}




bool ModPlusDymolaCtrl::simulate(QString tempFolder,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,QStringList filesTocopy,QStringList moDependencies)
{
    // Info
    infoSender.send(Info("Simulating model "+_modModelName,ListInfo::NORMAL2));

    // eventually compile model
    if(!this->isCompiled())
    {
        bool compileOk = compile();
        if(!compileOk)
            return false;
    }

    // clear outputVars
    outputVars->clear();

    // Create tempDir
    QDir modelTempDir(tempFolder);
    modelTempDir.mkdir(tempFolder);

    /// copy files in temp dir (\todo : optimize with a config.updateTempDir in case of several consecutive launches)
    QStringList allFilesToCopy;
    QDir mmoDir = QDir(_mmoFolder);
    allFilesToCopy << mmoDir.filePath("dsin.txt") << mmoDir.filePath("dymosim.exe");
    allFilesToCopy.append(filesTocopy);

    QDir tempDir = QDir(tempFolder);
    QFileInfo fileToCopyInfo;
    QFile fileToCopy;

    for(int i=0; i< allFilesToCopy.size();i++)
    {
        fileToCopy.setFileName(allFilesToCopy.at(i));
        fileToCopyInfo.setFile(fileToCopy);
        tempDir.remove(fileToCopyInfo.fileName());
        fileToCopy.copy(tempDir.filePath(fileToCopyInfo.fileName()));
    }

    // remove previous dymola log files
    QStringList filesToRemove;
    filesToRemove << "status" << "failure" << "success" << "dslog.txt";
    for(int i=0;i<filesToRemove.size();i++)
        tempDir.remove(filesToRemove.at(i));

    QString tempDsin = tempDir.absoluteFilePath("dsin.txt");
    QString tempDsres = tempDir.absoluteFilePath("dsres.txt");


    // Specifying new Variables values in dymosim input file
    Dymola::setVariablesToDsin(tempDsin,_modModelName,updatedVars,_parameters);

    // Launching Dymosim
    int maxNSec=-1;
    int iParam = _parameters->findItem((int)Dymola::MAXSIMTIME,MOParameter::INDEX);
    if(iParam>-1)
        maxNSec=_parameters->at(iParam)->getFieldValue(MOParameter::VALUE).toInt();
    Dymola::start(tempFolder,_simProcess,maxNSec);

    QString logFile = tempDir.absoluteFilePath("dslog.txt");

    //getting results
    //Checking if successed
    bool success=QFile::exists(tempDir.absoluteFilePath("success"));

    if(!success)
    {
        infoSender.send(Info(ListInfo::ONESIMULATIONFAILED,logFile));
        return false;
    }
    else
    {
        infoSender.send(Info(ListInfo::ONESIMULATIONSUCCESS,logFile));
    }

    bool readOk = readOutputVariables(outputVars,tempFolder);
    return readOk;
}

void ModPlusDymolaCtrl::stopSimulation()
{
    _simProcess.kill();
}

bool ModPlusDymolaCtrl::canBeStoped()
{
    return true;
}

bool ModPlusDymolaCtrl::createDsin()
{
    QDir dir(_mmoFolder);

    // copy dependencies
    QStringList depFileNames = _moomc->getDependenciesPaths(_moFilePath,false);
    for(int i=0;i<depFileNames.size();i++)
    {
        // check if file exists
        QFileInfo depFileInfo(depFileNames.at(i));
        QString depFilePath;
        if(!depFileInfo.exists())
        {
            // check in folder
            depFileInfo = QFileInfo(_moFilePath+QDir::separator()+depFileNames.at(i));
        }

        // copy
        if(depFileInfo.exists())
        {
            QFile depFile(depFileInfo.absoluteFilePath());
            depFile.copy(dir.absoluteFilePath(depFileInfo.fileName()));
        }
    }

    // compile
    bool success = Dymola::createDsin(_project->moFiles(),_modModelName,_mmoFolder);

    // Return
    return success;
}
void ModPlusDymolaCtrl::setMmoFolder(QString mmoFolder)
{
    _mmoFolder = mmoFolder;
}
