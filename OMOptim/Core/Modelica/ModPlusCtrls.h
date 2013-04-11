#ifndef MODPLUSCTRLS_H
#define MODPLUSCTRLS_H

#include "ModPlusCtrl.h"
#include "ModelPlus.h"

class ModPlusCtrls :public QObject, public QMap<ModPlusCtrl::Type,ModPlusCtrl*>
{
    Q_OBJECT

public :
    ModPlusCtrls(Project* project,ModelPlus* ModPlus,QObject* parent);
    ModPlusCtrls(Project* project,ModelPlus* ModPlus,const QDomElement &,QObject* parent);
    virtual ~ModPlusCtrls();

    static QString className(){return "Controlers";};

    ModPlusCtrls* clone();

    QDomElement toXmlData(QDomDocument & doc);


    ModPlusCtrl* currentCtrl() const;
    ModPlusCtrl::Type currentCtrlType() const;
    void setCurrentCtrlType(ModPlusCtrl::Type);
    void setFromOtherCtrls(const ModPlusCtrls &);

    static QList<ModPlusCtrl*> getCompatibleCtrls(Project* project, ModelPlus* modelPlus);

signals :
    void modified();


private :
    Project *_project;
    ModelPlus* _modelPlus;
    ModPlusCtrl::Type _ctrlType;
    void insertCtrl(ModPlusCtrl::Type,ModPlusCtrl*);

    static ModPlusCtrl* getNewCtrl(ModPlusCtrl::Type type,Project* project,ModelPlus* ModPlus);


};

#endif // MODPLUSCTRLS_H
