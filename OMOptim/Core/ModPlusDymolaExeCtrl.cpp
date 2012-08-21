#include "ModPlusDymolaExeCtrl.h"
#include "Dymola.h"
#include "LowTools.h"
#include "DymolaParameters.h"



ModPlusDymolaExeCtrl::ModPlusDymolaExeCtrl(Project *project, ModelPlus *model): ModPlusExeCtrl(project,model)
{
    _dsresFile = "dsres.txt";
    _dsfinalFile = "dsfinal.txt";


    _parameters = new MOParameters();
    DymolaParameters::setDefaultParameters(_parameters);

    connect(_parameters,SIGNAL(modified()),this,SIGNAL(modified()));
}

ModPlusDymolaExeCtrl::~ModPlusDymolaExeCtrl()
{
}

ModPlusCtrl *ModPlusDymolaExeCtrl::clone()
{
    ModPlusDymolaExeCtrl* cloned = new ModPlusDymolaExeCtrl(_project,_ModelPlus);

    cloned->_dsfinalFile = _dsfinalFile;
    cloned->_dsresFile = _dsresFile;
    cloned->_initFile = _initFile;
    cloned->_exeFile = _exeFile;

    delete cloned->_parameters;
    cloned->_parameters = _parameters->clone();

    return cloned;
}

QString ModPlusDymolaExeCtrl::name()
{
    return "DymolaExecutable";
}

ModPlusCtrl::Type ModPlusDymolaExeCtrl::type() const
{
    return ModPlusCtrl::DYMOLAEXECUTABLE;
}




bool ModPlusDymolaExeCtrl::simulate(QDir tempDir, MOVector<Variable> *updatedVars, MOVector<Variable> *outputVars, QFileInfoList filesToCopy, QFileInfoList moDependencies)
{
    // Info
    InfoSender::sendCurrentTask("Dymola : Simulating model "+_ModelPlus->modelName());


    // clear outputVars
    outputVars->clear();

    // Create tempDir
    LowTools::mkdir(tempDir.absolutePath(),true);

    /// copy files in temp dir (\todo : optimize with a config.updateTempDir in case of several consecutive launches)
    QFileInfoList allFilesToCopy;
//    QDir mmoDir = QDir(_ModelPlus->mmoFolder());
    allFilesToCopy << _exeFile;
    allFilesToCopy.append(filesToCopy);
    // choose init file (can take both)
    bool initFile =(_initFile != "");

    if(initFile)
        allFilesToCopy << _initFile;
    else
        InfoSender::instance()->sendError("Unable to find an init file for model "+_ModelPlus->modelName());

    InfoSender::instance()->debug("Start copying in temp directory : "+tempDir.absolutePath());
    QFileInfo fileToCopyInfo;
    //QFile fileToCopy;
    bool copyOk;
    for(int i=0; i< allFilesToCopy.size();i++)
    {
        //fileToCopy.setFileName(allFilesToCopy.at(i));
        fileToCopyInfo = allFilesToCopy.at(i);
        tempDir.remove(fileToCopyInfo.fileName());
        copyOk = QFile::copy(allFilesToCopy.at(i).absoluteFilePath(),tempDir.filePath(fileToCopyInfo.fileName()));
        //= fileToCopy.copy(tempDir.filePath(fileToCopyInfo.fileName()));
        InfoSender::instance()->debug("Copying in temp directory : "+tempDir.filePath(fileToCopyInfo.fileName())+" : "+QVariant(copyOk).toString());
        if(!copyOk)
            InfoSender::instance()->sendWarning("Unable to copy file in temp directory : "+fileToCopyInfo.fileName()/*+" ("+QFile::errorString()+")"*/);
    }


    // remove previous dymola log files
    QStringList filesToRemove;
    filesToRemove << "status" << "failure" << "success" << "dslog.txt";
    for(int i=0;i<filesToRemove.size();i++)
        tempDir.remove(filesToRemove.at(i));

    QString tempInit = tempDir.absoluteFilePath(QFileInfo(_initFile).fileName());
    QString tempDsres = tempDir.absoluteFilePath("dsres.txt");


    // Specifying new Variables values in dymosim input file
    Dymola::setVariablesToDsin(tempInit,_ModelPlus->modelName(),updatedVars,_parameters);

    // Launching Dymosim
    int maxNSec=_parameters->value(DymolaParameters::str(DymolaParameters::MAXSIMTIME),-1).toInt();
    Dymola::start(tempDir,_simProcess,maxNSec);

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



bool ModPlusDymolaExeCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString path)
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

bool ModPlusDymolaExeCtrl::readOutputVariablesDSFINAL(MOVector<Variable> *finalVariables, QString dsfinalFile)
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

bool ModPlusDymolaExeCtrl::readOutputVariablesDSRES(MOVector<Variable> *finalVariables, QString dsresFile)
{


    finalVariables->clear();

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



bool ModPlusDymolaExeCtrl::readInitialVariables(MOVector<Variable> *initVariables,bool forceRecompile, QString dsinFile)
{


    bool authorizeRecreate=false;
    QString logFile = _ModelPlus->mmoFolder().absoluteFilePath("buildlog.txt");
    if(QFile::exists(logFile))
        QFile::remove(logFile);

    InfoSender::sendCurrentTask("Reading initial variables in "+dsinFile);

    initVariables->clear();

    InfoSender::eraseCurrentTask();

    if(!QFile::exists(_initFile))
    {
        QString msg = "Unable to find the initial file";
        InfoSender::instance()->send(Info(msg,ListInfo::ERROR2));
        return false;
    }
    else
    {
        Dymola::getVariablesFromDsFile(_initFile,initVariables,_ModelPlus->modelName());
        InfoSender::instance()->send(Info(ListInfo::READVARIABLESSUCCESS));
        return true;
    }
}



