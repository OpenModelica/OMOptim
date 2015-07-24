

#include "ModPlusBlackBoxExeCtrl.h"
#include "Project.h"
#include "PlugInterface.h"
#include "ModItemsTree.h"
#include "ModExePlus.h"
#include "LowTools.h"
#include "MOParameter.h"


ModPlusBlackBoxExeCtrl::ModPlusBlackBoxExeCtrl(Project* project,ModelPlus* model):ModPlusExeCtrl(project, model)
{
    _plugInter = loadPlugInterface();
}

ModPlusBlackBoxExeCtrl::~ModPlusBlackBoxExeCtrl(void)
{
}

ModPlusCtrl::Type ModPlusBlackBoxExeCtrl::type() const
{
    return ModPlusCtrl::BLACKBOXEXECUTABLE;
}

ModPlusCtrl *ModPlusBlackBoxExeCtrl::clone()
{
    ModPlusBlackBoxExeCtrl* cloned = new ModPlusBlackBoxExeCtrl(_project, _ModelPlus);
    cloned->_plugInter = _plugInter;


    delete cloned->_parameters;
    cloned->_parameters = _parameters->clone();

    return cloned;
}

QString ModPlusBlackBoxExeCtrl::name()
{
    return "BlackBoxExecutable";
}


PlugInterface* ModPlusBlackBoxExeCtrl::loadPlugInterface() const
{
//    QString pluginPath = _project->pluginPath("OMOptimExeAdapt");
//    QPluginLoader loader(pluginPath);
//    QObject *plugin = loader.instance();
//    PlugInterface * plugInter = qobject_cast<PlugInterface *>(plugin);

//    return plugInter;
    return NULL;
}


bool ModPlusBlackBoxExeCtrl::readInitialVariables(MOVector<Variable> *initVariables,QFileInfoList filesToCopy,  bool forcerecompile, QString initialFile)
{
//    QString initFile;

//    ModItem* item = _ModelPlus->getProject()->modItemsTree()->findItem(_ModelPlus->name());
//    ExeModel* exeMod = dynamic_cast<ExeModel*> (item);
//    if( ! exeMod )
//        return false;
//    else
//    {
//        initFile = exeMod->getInputFilePath();
//        //      if(initFile.isEmpty() || initFile.right(3)!= "xml" || !QFile::exists(initFile))
//        if(initFile.isEmpty() || !QFile::exists(initFile))
//            return false;
//        else if(type == ModPlusCtrl::BLACKBOXEXECUTABLE)
//        {
//            readInitialVarsFromPluginFunc(initFile, _ModelPlus->modelName(), initVariables);
//            return true;
//        }
//        else
//        {
//            //            initVariables->clear();
//            //            getInputVariablesFromXmlFile(initFile, _ModelPlus->modelName(), initVariables);
//            return false;
//        }
//    }
}


bool ModPlusBlackBoxExeCtrl::readInitialVarsFromPluginFunc(QString filename, QString modelName,MOVector<Variable> *initVariables)
{
  bool varsRead = false;
  QDomDocument doc;

  if(_plugInter)
  {
      _plugInter->readVarsFromInitFile(filename, doc);
      varsRead= getInputVariablesFromXmlFile(doc,modelName,initVariables);
  }
  return varsRead;
}

bool ModPlusBlackBoxExeCtrl::setStopTime(double time)
{
    return false;
}


void ModPlusBlackBoxExeCtrl::setInputVarsInInitFileUsingPluginFunc(QString tempInitFile, QString modelName, MOVector<Variable> *variables)
{
    QDomDocument doc;

    if(_plugInter)
    {
        _plugInter->readVarsFromInitFile(tempInitFile, doc);
        setInputVariablesXml(doc, modelName, variables);
        _plugInter->writeInInitFile(tempInitFile, doc);
    }
}


bool ModPlusBlackBoxExeCtrl::simulate(QDir tempDir, MOVector<Variable> * inputVars, MOVector<Variable> * outputVars, QFileInfoList filesToCopy ,QFileInfoList moDependencies)
{

//    // Info
//    InfoSender::sendCurrentTask("Simulating Executable model "+_ModelPlus->modelName());

//    // clear outputVars
//    outputVars->clear();

//    // Create tempDir
//    if(!tempDir.exists())
//    LowTools::mkpath(tempDir.absolutePath(),false);

//    /// copy files in temp dir (\todo : optimize with a config.updateTempDir in case of several consecutive launches)
//    QFileInfoList allFilesToCopy;
////    QDir mmoDir = QDir(_ModelPlus->mmoFolder());
//    allFilesToCopy << _exeFile;
//    allFilesToCopy.append(filesToCopy);
//    // choose init file (can take both)
//    bool initFile =(_initFile != "");

//    if(initFile)
//        allFilesToCopy << _initFile;
//    else
//        InfoSender::instance()->sendError("Unable to find an init file for model "+_ModelPlus->modelName());

//    InfoSender::instance()->debug("Start copying in temp directory : "+tempDir.absolutePath());

//    QStringList dllFiles;

//    if(_plugInter)
//        dllFiles = _plugInter->dllFileNamesUsedByExeFile();
//    QFileInfoList dllFileInfoList;
//    for(int i = 0 ; i < dllFiles.size(); i++)
//    {
//        if(QFileInfo(dllFiles.at(i)).isAbsolute())
//            dllFileInfoList.append( QFileInfo(dllFiles.at(i)));
//        else
//            dllFileInfoList.append( QFileInfo(_exeFile).absoluteDir().absoluteFilePath(dllFiles.at(i)));
//    }
//    allFilesToCopy.append(dllFileInfoList);

//    QFileInfo fileToCopyInfo;
//    //QFile fileToCopy;
//    bool copyOk;
//    for(int i=0; i< allFilesToCopy.size();i++)
//    {
//        fileToCopyInfo = allFilesToCopy.at(i);
//        tempDir.remove(fileToCopyInfo.fileName());
//        copyOk = QFile::copy(allFilesToCopy.at(i).absoluteFilePath(),tempDir.filePath(fileToCopyInfo.fileName()));
//        InfoSender::instance()->debug("Copying in temp directory : "+tempDir.filePath(fileToCopyInfo.fileName())+" : "+QVariant(copyOk).toString());
//        if(!copyOk)
//            InfoSender::instance()->sendWarning("Unable to copy file in temp directory : "+fileToCopyInfo.fileName()/*+" ("+QFile::errorString()+")"*/);
//    }

//    // remove previous log files
//    QStringList filesToRemove;
//    filesToRemove << resFileNameFromPlugFunc();
//    for(int i=0;i<filesToRemove.size();i++)
//        tempDir.remove(filesToRemove.at(i));

//    QString tempInitFile = tempDir.absoluteFilePath(QFileInfo(_initFile).fileName());
//    QString tempResFile = tempDir.absoluteFilePath(resFileNameFromPlugFunc());
//    QString tempExeFile = tempDir.absoluteFilePath(QFileInfo(_exeFile).fileName());


//    // Specifying new Variables values in input file
//    setInputVarsInInitFileUsingPluginFunc(tempInitFile, _ModelPlus->modelName(),inputVars);

//    // Launching the Executable

//    int maxNSec=-1;
//    int iParam = _parameters->findItem((int)MAXSIMTIME,MOParameter::INDEX);
//    if(iParam>-1)
//        maxNSec=_parameters->at(iParam)->getFieldValue(MOParameter::VALUE).toInt();

//    ModPlusExeCtrl::start(tempExeFile,maxNSec);

//    InfoSender::eraseCurrentTask();

//    //getting results
//    //Checking if successed

//    if(QFile::exists(tempResFile))
//    {
//        bool readOk = getOutputVarsUsingPluginFunc(tempResFile, outputVars,_ModelPlus->modelName());
//        return readOk;
//    }
//     else
//        return false;

}



bool ModPlusBlackBoxExeCtrl::getOutputVarsUsingPluginFunc(QString fileName, MOVector<Variable> * variables, QString modelName)
{
    QDomDocument doc;

//    QString outputFileName = plugInter->outputFileName();
//    QString outputFilePath = QFileInfo(_initFile).absoluteDir().absoluteFilePath(outputFileName);
    if(_plugInter)
        _plugInter->readResultsFromOutputFile(fileName, doc);

    variables->clear();
    readResultsFromXmlDoc(doc, variables, modelName);

}

void ModPlusBlackBoxExeCtrl::readResultsFromXmlDoc(const QDomDocument &doc, MOVector<Variable> * variables, QString modelName)
{
    Variable* var;
    bool ok;

    QDomElement xfmi = doc.firstChildElement("fmiModelDescription");
    QDomElement modelVariables = xfmi.firstChildElement("ModelVariables");
    QDomNodeList scalarVariablesList = modelVariables.elementsByTagName("ScalarVariable");

    for(int i=0; i<scalarVariablesList.size(); i++ )
    {
        var = variableFromFmi(scalarVariablesList.at(i).toElement(), modelName, ok);
        if(ok)
            variables->addItem(var);
    }


}

QString ModPlusBlackBoxExeCtrl::resFileNameFromPlugFunc() const
{
  PlugInterface* plugInter =loadPlugInterface();
  return plugInter->outputFileName();
}
