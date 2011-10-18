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


ModPlusOMCtrl::ModPlusOMCtrl(Project* project,ModModelPlus* model,MOomc* moomc,QString mmoFolder,QString moFilePath,QString modModelName)
    :ModPlusCtrl(project,model,moomc,mmoFolder,moFilePath,modModelName)
{
    _initFileTxt = _modModelName+"_init.txt";
    _initFileXml = _modModelName+"_init.xml";
    _resFile = _modModelName+"_res.csv";
#ifdef WIN32
    _exeFile = _modModelName+".exe";
#else
    _exeFile = _modModelName;
#endif

    _copyAllMoOfFolder = true;

    _parameters = new MOParameters();
    setDefaultParameters();
}

ModPlusOMCtrl::~ModPlusOMCtrl(void)
{
}

ModPlusCtrl::Type ModPlusOMCtrl::type()
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
    _parameters->addItem(new MOParameter(OpenModelica::STOPVALUE,"stop value","Stop value",1,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
    _parameters->addItem(new MOParameter((int)OpenModelica::MAXSIMTIME,"MaxSimTime","Maximum time allowed for simulation (-1 : no limit)",-1,MOParameter::INT,-1,std::numeric_limits<int>::max()));
}

bool ModPlusOMCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString resFile)
{
    infoSender.send(Info("Reading final variables in "+resFile,ListInfo::NORMAL2));

    if(resFile.isEmpty())
        resFile = _resFile;

    return OpenModelica::getFinalVariablesFromFile(resFile,finalVariables,_modModelName);
}


bool ModPlusOMCtrl::readInitialVariables(MOVector<Variable> *initVariables,QString initFile)
{


    QString initFileTxt;
    QString initFileXml;

    bool authorizeRecreate=false;
    if(initFile.isEmpty())
    {
        authorizeRecreate=true;

        initFileTxt = _mmoFolder+QDir::separator()+_initFileTxt;
        initFileXml = _mmoFolder+QDir::separator()+_initFileXml;
    }
    else
    {
        if(initFile.right(3)=="xml")
            initFileXml = initFile;

        if(initFile.right(3)=="txt")
            initFileTxt = initFile;
    }

    infoSender.send(Info("Reading initial variables in "+initFile,ListInfo::NORMAL2));

    initVariables->clear();

    QFileInfo initFileInfoXml = QFileInfo(initFileXml);
    QFileInfo initFileInfoTxt = QFileInfo(initFileTxt);

    if(!initFileInfoXml.exists()&& !initFileInfoTxt.exists() && authorizeRecreate)
    {
        createInitFile();
    }

    if(!initFileInfoXml.exists()&& !initFileInfoTxt.exists())
    {
        return false;
    }
    else
    {
        if(initFileInfoXml.exists())
            OpenModelica::getInputVariablesFromXmlFile(_moomc,initFileXml,_modModelName,initVariables);
        else if(initFileInfoTxt.exists())
            OpenModelica::getInputVariablesFromTxtFile(_moomc,initFileTxt,initVariables,_modModelName);

        return true;
    }

    return true;
}

bool ModPlusOMCtrl::compile()
{

    infoSender.send(Info("Compiling model "+_modModelName,ListInfo::NORMAL2));

    // compile
    QString logFile = _mmoFolder+_modModelName+".log";
    bool success = OpenModelica::compile(_moomc,_moFilePath,_modModelName,_mmoFolder);

    // Inform
    ListInfo::InfoNum iMsg;
    if(success)
        infoSender.send(Info(ListInfo::MODELCOMPILATIONSUCCESS,_modModelName,logFile));
    else
        infoSender.send(Info("Model "+_modModelName+" failed to compile. See OMC log tab for details.",ListInfo::ERROR2));


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
        filePath = _mmoFolder+QDir::separator()+filesNeeded.at(i);
        file.setFileName(filePath);
        filesExist = filesExist && file.exists();
    }
    return filesExist;
}


bool ModPlusOMCtrl::simulate(QString tempFolder,MOVector<Variable> * inputVars,MOVector<Variable> * outputVars,QStringList filesToCopy)
{
    // Info
    infoSender.send(Info("Simulating model "+_modModelName,ListInfo::NORMAL2));

    // eventually compile model
    if(!isCompiled())
        compile();

    // clear outputVars
    outputVars->clear();

    // Create tempDir
    QDir modelTempDir(tempFolder);
    modelTempDir.mkdir(tempFolder);


    /// copy files in temp dir (\todo : optimize with a config.updateTempDir in case of several consecutive launches)
    QStringList allFilesToCopy;
    QDir mmoDir = QDir(_mmoFolder);
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
        infoSender.sendError("Unable to find an init file for model "+_modModelName);

    QDir tempDir = QDir(tempFolder);
    QFileInfo fileToCopyInfo;
    QFile fileToCopy;
    bool copyOk;
    for(int i=0; i< allFilesToCopy.size();i++)
    {
        fileToCopy.setFileName(allFilesToCopy.at(i));
        fileToCopyInfo.setFile(fileToCopy);
        tempDir.remove(fileToCopyInfo.fileName());
        copyOk = fileToCopy.copy(tempDir.filePath(fileToCopyInfo.fileName()));
        if(!copyOk)
            infoSender.sendWarning("Unable to copy file in temp directory : "+fileToCopy.fileName()+" ("+fileToCopy.errorString()+")");
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
        OpenModelica::setInputXml(tempInitFileXml,inputVars,_modModelName,parameters());
    else
        OpenModelica::setInputVariablesTxt(tempInitFileTxt,inputVars,_modModelName,parameters());

    // Launching openmodelica
    int maxNSec=-1;
    int iParam = _parameters->findItem((int)OpenModelica::MAXSIMTIME,MOParameter::INDEX);
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
bool ModPlusOMCtrl::createInitFile()
{
    return compile();
}

void ModPlusOMCtrl::setMmoFolder(QString mmoFolder)
{
    _mmoFolder = mmoFolder;
}
