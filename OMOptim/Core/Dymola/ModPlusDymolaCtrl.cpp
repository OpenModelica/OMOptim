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

ModPlusDymolaCtrl::ModPlusDymolaCtrl(Project* project,ModModelPlus* model,MOomc* moomc)
    :ModPlusCtrl(project,model,moomc)
{
    _dsinFile = "dsin.txt";
    _dsresFile = "dsres.txt";
    _dsfinalFile = "dsfinal.txt";
    _copyAllMoOfFolder = true;

    _parameters = new MOParameters();
    setDefaultParameters();

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

    QMap<int,QString> finalFiles;
    finalFiles.insert(DSFINAL,"dsfinal");
    finalFiles.insert(DSRES,"dsres");
    _parameters->addItem( new MOParameterListed((int)Dymola::FINALFILE,"Final file considered","File where variables are read (longer if dsres but all variables are within)",DSFINAL,finalFiles));
}

bool ModPlusDymolaCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString folder)
{
    if(folder.isEmpty())
        folder = _modModelPlus->mmoFolder();
    QDir dir(folder);

    int outputFile = _parameters->value(Dymola::FINALFILE,DSFINAL).toInt();
    switch(outputFile)
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
    InfoSender::instance()->send(Info("Reading final variables in "+dsfinalFile,ListInfo::NORMAL2));

    finalVariables->clear();
    QFileInfo dsfinalInfo = QFileInfo(dsfinalFile);

    if(!dsfinalInfo.exists())
    {
        return false;
    }
    Dymola::getVariablesFromDsFile(dsfinalFile,finalVariables,_modModelPlus->modModelName());

    return true;
}

bool ModPlusDymolaCtrl::readOutputVariablesDSRES(MOVector<Variable> *finalVariables, QString dsresFile)
{
    InfoSender::instance()->send(Info("Reading final variables in "+dsresFile,ListInfo::NORMAL2));

    finalVariables->clear();

    if(dsresFile.isEmpty())
    {
        dsresFile = _modModelPlus->mmoFolder()+QDir::separator()+_dsresFile;
    }

    QFileInfo dsresInfo = QFileInfo(dsresFile);

    if (dsresInfo.exists())
    {
        Dymola::getFinalVariablesFromDsFile(dsresFile,finalVariables,_modModelPlus->modModelName());
        return true;
    }
    else
        return false;
}

bool ModPlusDymolaCtrl::readInitialVariables(MOVector<Variable> *initVariables,QString dsinFile)
{


    bool authorizeRecreate=false;
     QString logFile = _modModelPlus->mmoFolder()+QDir::separator()+ "buildlog.txt";
     if(QFile::exists(logFile))
         QFile::remove(logFile);

    if(dsinFile.isEmpty())
    {
        authorizeRecreate=true;
        dsinFile = _modModelPlus->mmoFolder()+QDir::separator()+_dsinFile;
    }

    InfoSender::instance()->send(Info("Reading initial variables in "+dsinFile,ListInfo::NORMAL2));

    initVariables->clear();

    if(!QFile::exists(dsinFile) && authorizeRecreate)
    {
        createDsin();
    }

    if(!QFile::exists(dsinFile))
    {
        QString msg = "Unable to create DsinFile. See <A href=\"file:///"+ logFile +"\">log file</A> for detail.";
              InfoSender::instance()->send(Info(msg,ListInfo::ERROR2));
        return false;
    }
    else
    {
        Dymola::getVariablesFromDsFile(dsinFile,initVariables,_modModelPlus->modModelName());
        InfoSender::instance()->send(Info(ListInfo::READVARIABLESSUCCESS));
        return true;
    }
}

bool ModPlusDymolaCtrl::compile(const QStringList & moDependencies)
{

    InfoSender::instance()->send(Info("Compiling model "+_modModelPlus->modModelName(),ListInfo::NORMAL2));

    //QString logFilePath = _mmoFolder+QDir::separator()+"log.html";
    QString logFilePath = _modModelPlus->mmoFolder()+QDir::separator()+"buildlog.txt";

    // compile
    bool success = Dymola::firstRun(_project->moFiles(),_modModelPlus->modModelName(),_modModelPlus->mmoFolder(),logFilePath,moDependencies);

    // Inform
    ListInfo::InfoNum iMsg;
    if(success)
        iMsg = ListInfo::MODELCOMPILATIONSUCCESS;
    else
        iMsg = ListInfo::MODELCOMPILATIONFAIL;

    InfoSender::instance()->send(Info(iMsg,_modModelPlus->modModelName(),logFilePath));

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
        filePath = _modModelPlus->mmoFolder()+QDir::separator()+filesNeeded.at(i);
        filesExist = filesExist && QFile::exists(filePath);
    }
    return filesExist;
}




bool ModPlusDymolaCtrl::simulate(QString tempFolder,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,QStringList filesTocopy,QStringList moDependencies)
{
    // Info
    InfoSender::instance()->send(Info("Simulating model "+_modModelPlus->modModelName(),ListInfo::NORMAL2));

    // clear outputVars
    outputVars->clear();

    bool compileOk = isCompiled();
    // eventually compile model
    if(!compileOk)
        compileOk = compile();

    if(!compileOk)
        return false; // compilation failed, useless to pursue



    // Create tempDir
    QDir modelTempDir(tempFolder);
    modelTempDir.mkdir(tempFolder);

    /// copy files in temp dir (\todo : optimize with a config.updateTempDir in case of several consecutive launches)
    QStringList allFilesToCopy;
    QDir mmoDir = QDir(_modModelPlus->mmoFolder());
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
    Dymola::setVariablesToDsin(tempDsin,_modModelPlus->modModelName(),updatedVars,_parameters);

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
        InfoSender::instance()->send(Info(ListInfo::ONESIMULATIONFAILED,logFile));
        return false;
    }
    else
    {
        InfoSender::instance()->send(Info(ListInfo::ONESIMULATIONSUCCESS,logFile));
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
    QDir dir(_modModelPlus->mmoFolder());

    // copy dependencies
    QString moFilePath = _modModelPlus->modModel()->filePath();
    QStringList depFileNames = _moomc->getDependenciesPaths(moFilePath,false);
    for(int i=0;i<depFileNames.size();i++)
    {
        // check if file exists
        QFileInfo depFileInfo(depFileNames.at(i));
        QString depFilePath;
        if(!depFileInfo.exists())
        {
            // check in folder
            depFileInfo = QFileInfo(_modModelPlus->modModel()->filePath()+QDir::separator()+depFileNames.at(i));
        }

        // copy
        if(depFileInfo.exists())
        {
            QFile depFile(depFileInfo.absoluteFilePath());
            depFile.copy(dir.absoluteFilePath(depFileInfo.fileName()));
        }
    }

    // compile
    bool success = Dymola::createDsin(_project->moFiles(),_modModelPlus->modModelName(),_modModelPlus->mmoFolder());

    // Return
    return success;
}

