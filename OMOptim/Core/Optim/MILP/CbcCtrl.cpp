#include "CbcCtrl.h"


CbcCtrl::CbcCtrl(QDir folder,QString mpsFileName,QString solFileName,MOParameters *parameters)
    :_process("Cbc",true)
{
    _folder = folder;
    _mpsFileName = mpsFileName;
    _solFileName = solFileName;
    _parameters = parameters;

}

bool CbcCtrl::run(QString &msg)
{
    _process.setWorkingDirectory(_folder.absolutePath());

    QString optPreprocessor="";
    QString optIntegerTolerance="";
    QString optPrimalTolerance="";

    // preprocessor
    EIHEN1Parameters::CbcPreprocessor iPreprocessor = (EIHEN1Parameters::CbcPreprocessor)_parameters->value(EIHEN1Parameters::CBCPREPROCESS).toInt();

    int iParam = _parameters->findItem(EIHEN1Parameters::CBCPREPROCESS,MOParameter::INDEX);
    if(iParam>-1)
    {
        MOParameterListed* preprocess = dynamic_cast<MOParameterListed*>(_parameters->at(iParam));
        optPreprocessor= " -preprocess "+preprocess->strValue();
    }

    iParam = _parameters->findItem(EIHEN1Parameters::PRIMALTOLERANCE,MOParameter::INDEX);
    if(iParam>-1)
    {
        MOParameter* primalTParam = dynamic_cast<MOParameter*>(_parameters->at(iParam));
        optPrimalTolerance= " -primalTolerance "+QString::number(primalTParam->value().toDouble());
    }


    iParam = _parameters->findItem(EIHEN1Parameters::INTEGERTOLERANCE,MOParameter::INDEX);
    if(iParam>-1)
    {
        MOParameter* integerTParam = dynamic_cast<MOParameter*>(_parameters->at(iParam));
        optIntegerTolerance= " -integerTolerance "+QString::number(integerTParam->value().toDouble());
    }


    QStringList arguments;
    arguments << "-c";
    QString cmd = "cbc -import "+ _mpsFileName +optPreprocessor+optIntegerTolerance+optPrimalTolerance+" -solve -solution " + _solFileName;
    arguments << cmd;
    _process.start("sh",arguments);
    _process.waitForFinished(-1);

    return success(msg);
}

void CbcCtrl::stop()
{
    _process.kill();
}

bool CbcCtrl::success(QString &msg)
{
    QFile file(_folder.absoluteFilePath(_solFileName));
    bool openOk = file.open(QIODevice::ReadOnly);
    if(!openOk)
        return false;

    QTextStream* in = new QTextStream(&file);
    QString txt = in->readLine();

    // read if successfull
    bool isOptimal = (txt.indexOf("Optimal")==0);
    if(!isOptimal)
       msg = txt;
    return isOptimal;
}
