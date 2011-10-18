#ifndef EIMODELICAMODIFIER_H
#define EIMODELICAMODIFIER_H


#include <QtCore/QString>
#include "MOomc.h"
#include "ModModelPlus.h"
#include "EIModelicaHE.h"

namespace EI
{


class EIModelicaModifier
{

public:
    EIModelicaModifier(ModModelPlus*, MOomc*);

    void addHERemover(const QString & hename);
    void addHEInserter(const QString &  hename, const QString &  inletH,const QString &  outletH,const QString &  inletC,const QString &  outletC,HEType);

    void apply();

private :
    void removeHE(QVariantList);
    void addHE(QVariantList);
    void addOperation(int op,QVariantList args);

    ModModelPlus* _model;
    MOomc*  _moomc;

    QList<int> _operations;
    QList<QVariantList> _operationArgs;

    enum operations
    {
        ADDHE,
        REMOVEHE
    };
};



}
#endif // EIMODELICAMODIFIER_H
