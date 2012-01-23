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

ModPlusOMCtrl::ModPlusOMCtrl(Project* project,ModModelPlus* modModelPlus,MOomc* moomc)
    :ModPlusCtrl(project,modModelPlus,moomc)
{
    _initFileTxt = _modModelPlus->modModelName()+"_init.txt";
    _initFileXml = _modModelPlus->modModelName()+"_init.xml";
    _resFile = _modModelPlus->modModelName()+"_res.csv";
#ifdef WIN32
    _exeFile = _modModelPlus->modModelName()+".exe";
#else
    _exeFile = _modModelPlus->modModelName();
#endif

    _copyAllMoOfFolder = true;

    _parameters = new MOParameters();
    setDefaultParameters();

   connect(_parameters,SIGNAL(modified()),this,SIGNAL(modified()));
}

ModPlusOMCtrl::~ModPlusOMCtrl(void)
{
}

ModPlusCtrl* ModPlusOMCtrl::clone()
{
    ModPlusOMCtrl* cloned = new ModPlusOMCtrl(_project,_modModelPlus,_moomc);

    cloned->_initFileTxt = _initFileTxt;
    cloned->_initFileXml = _initFileXml;
    cloned->_resFile = _resFile;
    cloned->_exeFile = _exeFile;
    cloned->_copyAllMoOfFolder = _copyAllMoOfFolder;
    cloned->_parameters = _parameters->clone();

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


// Parameters
void ModPlusOMCtrl::setDefaultParameters()
{
    _parameters->addItem(new MOParameter(STOPTIME,"stopTime","Stop time",1,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));

    QMap<int,QString> mapSolvers;
    mapSolvers.insert(ModPlusOMCtrl::DASSL,"dassl");
    mapSolvers.insert(ModPlusOMCtrl::EULER,"euler");

    _parameters->addItem( new MOParameterListed((int)SOLVER,"solver","Solver",ModPlusOMCtrl::DASSL,mapSolvers));

    _parameters->addItem(new MOParameter(TOLERANCE,"tolerance","Tolerance",1e-6,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
    _parameters->addItem(new MOParameter(STEPSIZE,"stepSize","Step size",0.002,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
    _parameters->addItem(new MOParameter(STARTTIME,"startTime","Start time",0,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));

    _parameters->addItem(new MOParameter((int)MAXSIMTIME,"MaxSimTime","Maximum time allowed for simulation (-1 : no limit)",-1,MOParameter::INT,-1,std::numeric_limits<int>::max()));
}

bool ModPlusOMCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString resFile)
{
    InfoSender::instance()->send(Info("Reading final variables in "+resFile,ListInfo::NORMAL2));

    if(resFile.isEmpty())
        resFile = _resFile;

    return OpenModelica::getFinalVariablesFromFile(resFile,finalVariables,_modModelPlus->modModelName());
}


bool ModPlusOMCtrl::readInitialVariables(MOVector<Variable> *initVariables,QString initFile)
{


    QString initFileTxt;
    QString initFileXml;

    bool authorizeRecreate=false;
    if(initFile.isEmpty())
    {
        authorizeRecreate=true;

        initFileTxt = _modModelPlus->mmoFolder()+QDir::separator()+_initFileTxt;
        initFileXml = _modModelPlus->mmoFolder()+QDir::separator()+_initFileXml;
    }
    else
    {
        if(initFile.right(3)=="xml")
            initFileXml = initFile;

        if(initFile.right(3)=="txt")
            initFileTxt = initFile;
    }

    InfoSender::instance()->send(Info("Reading initial variables in "+initFile,ListInfo::NORMAL2));

    initVariables->clear();

    if(!QFile::exists(initFileXml)&& !QFile::exists(initFileTxt)&& authorizeRecreate)
    {
        createInitFile();
    }


    if(!QFile::exists(initFileXml)&& !QFile::exists(initFileTxt))
    {
        return false;
    }
    else
    {
        if(QFile::exists(initFileXml))
            OpenModelica::getInputVariablesFromXmlFile(_moomc,initFileXml,_modModelPlus->modModelName(),initVariables);
        else if(QFile::exists(initFileTxt))
            OpenModelica::getInputVariablesFromTxtFile(_moomc,initFileTxt,initVariables,_modModelPlus->modModelName());

        return true;
    }

    return true;
}

bool ModPlusOMCtrl::compile(const QStringList & moDeps)
{

    InfoSender::instance()->send(Info("Compiling model "+_modModelPlus->modModelName(),ListInfo::NORMAL2));

    // compile
    QString logFile = _modModelPlus->mmoFolder()+_modModelPlus->modModelName()+".log";
    bool success = OpenModelica::compile(_moomc,_modModelPlus->modModel()->filePath(),_modModelPlus->modModelName(),_modModelPlus->mmoFolder(),moDeps);

    // Inform
    ListInfo::InfoNum iMsg;
    if(success)
        InfoSender::instance()->send(Info(ListInfo::MODELCOMPILATIONSUCCESS,_modModelPlus->modModelName(),logFile));
    else
        InfoSender::instance()->send(Info("Model "+_modModelPlus->modModelName()+" failed to compile. See OMC log tab for details.",ListInfo::ERROR2));


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
        filePath = _modModelPlus->mmoFolder()+QDir::separator()+filesNeeded.at(i);
        file.setFileName(filePath);
        filesExist = filesExist && file.exists();
    }
    return filesExist;
}


bool ModPlusOMCtrl::simulate(QString tempFolder,MOVector<Variable> * inputVars,MOVector<Variable> * outputVars,QStringList filesToCopy,QStringList moDependencies)
{
    // Info
    InfoSender::instance()->send(Info("Simulating model "+_modModelPlus->modModelName(),ListInfo::NORMAL2));

    // load moDependencies
    _moomc->loadFiles(moDependencies);

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
    allFilesToCopy << mmoDir.filePath(_exeFile);
    allFilesToCopy.append(filesToCopy);
    // choose init file (can take both)
    bool txt=mmoDir.exists(_initFileTxt);
    bool xml=mmoDir.exists(_initFileXml);
    if(txt)
        allFilesToCopy <<  mmoDir.filePath(_initFileTxt);
    if(xml)
        allFilesToCopy << mmoDir.filePath(_initFileXml);

    if(!txt && ! xml)
        InfoSender::instance()->sendError("Unable to find an init file for model "+_modModelPlus->modModelName());

    QDir tempDir = QDir(tempFolder);
    InfoSender::instance()->debug("Start copying in temp directory : "+tempFolder);
    QFileInfo fileToCopyInfo;
    //QFile fileToCopy;
    bool copyOk;
    for(int i=0; i< allFilesToCopy.size();i++)
    {
        //fileToCopy.setFileName(allFilesToCopy.at(i));
        fileToCopyInfo.setFile(allFilesToCopy.at(i));
        tempDir.remove(fileToCopyInfo.fileName());
        copyOk = QFile::copy(allFilesToCopy.at(i),tempDir.filePath(fileToCopyInfo.fileName()));
         //= fileToCopy.copy(tempDir.filePath(fileToCopyInfo.fileName()));
        InfoSender::instance()->debug("Copying in temp directory : "+tempDir.filePath(fileToCopyInfo.fileName())+" : "+QVariant(copyOk).toString());
        if(!copyOk)
            InfoSender::instance()->sendWarning("Unable to copy file in temp directory : "+fileToCopyInfo.fileName()/*+" ("+QFile::errorString()+")"*/);
    }


    // remove previous log files
    QStringList filesToRemove;
    filesToRemove << _resFile;
    for(int i=0;i<filesToRemove.size();i++)
        tempDir.remove(filesToRemove.at(i));

    QString tempInitFileXml = tempDir.absoluteFilePath(_initFileXml);
    QString tempInitFileTxt = tempDir.absoluteFilePath(_initFileTxt);
    QString tempResFile = tempDir.absoluteFilePath(_resFile);
    QString tempExeFile = tempDir.absoluteFilePath(_exeFile);


    QFileInfo initXmlInfo(tempInitFileXml);
    QFileInfo initTxtInfo(tempInitFileTxt);

    // use xml or txt init file ?
    bool useXml = initXmlInfo.exists();

    // Specifying new Variables values in OM input file
    if(useXml)
        OpenModelica::setInputXml(tempInitFileXml,inputVars,_modModelPlus->modModelName(),parameters());
    else
        OpenModelica::setInputVariablesTxt(tempInitFileTxt,inputVars,_modModelPlus->modModelName(),parameters());

    // Launching openmodelica
    int maxNSec=-1;
    int iParam = _parameters->findItem((int)MAXSIMTIME,MOParameter::INDEX);
    if(iParam>-1)
        maxNSec=_parameters->at(iParam)->getFieldValue(MOParameter::VALUE).toInt();

    OpenModelica::start(tempExeFile,maxNSec);

    //getting results
    //Checking if successed
    bool success=QFile::exists(tempResFile);

    if(!success)
        return false;

    bool readOk = readOutputVariables(outputVars,tempResFile);
    return readOk;
}
bool ModPlusOMCtrl::createInitFile(const QStringList & moDeps)
{
    return compile(moDeps);
}
