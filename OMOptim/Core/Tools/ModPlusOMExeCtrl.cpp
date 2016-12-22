

#include "ModPlusOMCtrl.h"
#include "ModPlusOMExeCtrl.h"
#include "OpenModelica.h"
#include "LowTools.h"
#include "Project.h"
#include "ModItemsTree.h"
#include "ExeModel.h"
#include "OpenModelicaParameters.h"
#include "Utilities.h"

ModPlusOMExeCtrl::ModPlusOMExeCtrl(Project* project,ModelPlus* model)
    :ModPlusExeCtrl(project, model)
{
    _parameters = new MOParameters();
    OpenModelicaParameters::setDefaultParameters(_parameters);
}


ModPlusOMExeCtrl::~ModPlusOMExeCtrl(void)
{
}

ModPlusCtrl *ModPlusOMExeCtrl::clone()
{
    ModPlusOMExeCtrl* cloned = new ModPlusOMExeCtrl(_project,_ModelPlus);


    delete cloned->_parameters;
    cloned->_parameters = _parameters->clone();

    return cloned;
}


QString ModPlusOMExeCtrl::name()
{
    return "OMExecutable";
}


bool ModPlusOMExeCtrl::useMat()
{
    return _parameters->value(OpenModelicaParameters::str(OpenModelicaParameters::OUTPUT),true).toBool();
}


QString ModPlusOMExeCtrl::resFile()
{
    QString model_name = _ModelPlus->modelName();
    model_name.truncate(model_name.size() -4);
    if(useMat())
        return model_name +"_res.mat";
    else
        return model_name +"_res.csv";
}


QString ModPlusOMExeCtrl::resMatFile()
{
    QString resFile = exeFile().fileName();
    resFile.truncate(resFile.size() -4);
    return resFile +"_res.mat";
}


QString ModPlusOMExeCtrl::resCsvFile()
{
    QString resFile = exeFile().fileName();
    resFile.truncate(resFile.size() -4);
    return resFile +"_res.csv";
}



ModPlusCtrl::Type ModPlusOMExeCtrl::type() const
{
    return ModPlusCtrl::OMEXECUTABLE;
}

bool ModPlusOMExeCtrl::readInitialVariables(MOVector<Variable> *initVariables, QFileInfoList filesToCopy, bool forcerecompile, QString initialFile)
{
    QString initFile;

    ModItem* item = _ModelPlus->getProject()->modItemsTree()->findItem(_ModelPlus->name());
    ExeModel* exeMod = dynamic_cast<ExeModel*> (item);
    if( ! exeMod )
        return false;
    else
    {
        initFile = exeMod->inputFile().absoluteFilePath();
        if(initFile.isEmpty() || !QFile::exists(initFile))
            return false;
        else
        {
            initVariables->clear();
            getInputVariablesFromXmlFile(initFile, _ModelPlus->modelName(), initVariables);
            return true;
        }
    }
}



bool ModPlusOMExeCtrl::simulate(QDir tempFolder, MOVector<Variable> *inputVars, MOVector<Variable> *outputVars, QFileInfoList filesToCopy, QFileInfoList moDependencies)
{

    // Info
    InfoSender::sendCurrentTask("Simulating Executable model "+_ModelPlus->modelName());

    // clear outputVars
    outputVars->clear();

    // Create tempDir
    LowTools::mkpath(tempFolder.absolutePath(),false);

    /// copy files in temp dir (\todo : optimize with a config.updateTempDir in case of several consecutive launches)
    QFileInfoList allFilesToCopy;
    allFilesToCopy << exeFile();
    allFilesToCopy << initFile();
    allFilesToCopy.append(filesToCopy);

    InfoSender::instance()->debug("Start copying in temp directory : "+tempFolder.absolutePath());
    //QFile fileToCopy;
    bool copyOk = LowTools::copyFilesInFolder(allFilesToCopy,tempFolder);
    if(!copyOk)
        return false;

    // remove previous log files
    QStringList filesToRemove;
    filesToRemove << resMatFile() << resCsvFile();
    for(int i=0;i<filesToRemove.size();i++)
        tempFolder.remove(filesToRemove.at(i));

    QString tempInitFile = tempFolder.absoluteFilePath(QFileInfo(initFile()).fileName());
    QString tempMatResFile = tempFolder.absoluteFilePath(resMatFile());
    QString tempCsvResFile = tempFolder.absoluteFilePath(resCsvFile());
    QString tempExeFile = tempFolder.absoluteFilePath(QFileInfo(exeFile()).fileName());

    // Specifying new Variables values in OM input file
    OpenModelica::setInputXml(tempInitFile,inputVars,_ModelPlus->modelName(),parameters());


    // Launching openmodelica
    int maxNSec=_parameters->value(OpenModelicaParameters::str(OpenModelicaParameters::MAXSIMTIME),-1).toInt();

    ModPlusOMExeCtrl::start(tempExeFile,maxNSec);

    InfoSender::eraseCurrentTask();

    //getting results
    //Checking if successed

    if(QFile::exists(tempMatResFile))
    {
        bool readOk = readOutputVariables(outputVars,tempMatResFile);
        return readOk;
    }
    else if(QFile::exists(tempCsvResFile))
    {
        bool readOk = readOutputVariables(outputVars,tempCsvResFile);
        return readOk;
    }
     else
        return false;

}


bool ModPlusOMExeCtrl::readOutputVariables(MOVector<Variable> *finalVariables,QString resFileLocal)
{
    InfoSender::instance()->send(Info("Reading final variables in "+resFileLocal,ListInfo::NORMAL2));

//    if(resFileLocal.isEmpty())
//        resFileLocal = resFile();

    if(resMatFile()== QFileInfo(resFileLocal).fileName())
        return OpenModelica::getFinalVariablesFromMatFile(resFileLocal,finalVariables,_ModelPlus->modelName());
    else
        return OpenModelica::getFinalVariablesFromFile(resFileLocal,finalVariables,_ModelPlus->modelName());
}

bool ModPlusOMExeCtrl::setStopTime(double time)
{
    _parameters->setValue(OpenModelicaParameters::str(OpenModelicaParameters::STOPTIME),time);
    return true;
}
bool ModPlusOMExeCtrl::start(QString exeFile,int maxnsec)
{

    QFileInfo exeFileInfo(exeFile);
    QString exeDir = exeFileInfo.absolutePath();

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
        QString msg("CreateProcess failed (%d).");
        InfoSender::instance()->debug(msg);
        return false;
    }
    QString output(simProcess.readAllStandardOutput());
    InfoSender::instance()->send(Info(output,ListInfo::OMCNORMAL2));
    return ok;
}
