#include "EIModelicaModifier.h"
using namespace EI;


EIModelicaModifier::EIModelicaModifier(ModModelPlus* model, MOomc* moomc)
{
    _model = model;
    _moomc = moomc;
}


void EIModelicaModifier::addHERemover(const QString & hename)
{
    QVariantList args;
    args.push_back(hename);

    addOperation(REMOVEHE,args);
}

void EIModelicaModifier::addHEInserter(const QString &  hename, const QString &  inletH,const QString &  outletH,const QString &  inletC,const QString &  outletC, HEType hetype)
{
    QVariantList args;
    args << hename << inletH << outletH << inletC << outletC << hetype;

    addOperation(ADDHE,args);
}

void  EIModelicaModifier::addOperation(int op,QVariantList args)
{
    _operations.push_back(op);
    _operationArgs.push_back(args);
}


void EIModelicaModifier::addHE(QVariantList args)
{
    QString hename = args.at(0).toString();
    QString inletH = args.at(1).toString();
    QString outletH = args.at(2).toString();
    QString inletC = args.at(3).toString();
    QString outletC = args.at(4).toString();
    HEType heType = (HEType)args.at(5).toInt();

    EIModelicaHE* he = EIModelicaHES::getInstance(heType);

    // create component
    _moomc->addComponent(hename,he->className(),_model->modModelName(),QString());

    // connect
    _moomc->addConnection(inletH,he->inletH());
    _moomc->addConnection(inletC,he->inletC());
    _moomc->addConnection(outletH,he->outletH());
    _moomc->addConnection(outletC,he->outletC());
}

