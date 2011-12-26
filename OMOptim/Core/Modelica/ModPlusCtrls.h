#ifndef MODPLUSCTRLS_H
#define MODPLUSCTRLS_H

#include "ModPlusCtrl.h"


class ModPlusCtrls :public QObject, public QMap<ModPlusCtrl::Type,ModPlusCtrl*>
{
    Q_OBJECT

public :
    ModPlusCtrls(Project* project,ModModelPlus* modModelPlus);
    ModPlusCtrls(Project* project,ModModelPlus* modModelPlus,const QDomElement &);
    virtual ~ModPlusCtrls();

    static QString className(){return "Controlers";};

    ModPlusCtrls* clone();

    QDomElement toXmlData(QDomDocument & doc);


    ModPlusCtrl* currentCtrl() const;
    ModPlusCtrl::Type currentCtrlType() const;
    void setCurrentCtrlType(ModPlusCtrl::Type);

    void setFromOtherCtrls(const ModPlusCtrls &);

signals :
    void modified();


private :
    Project *_project;
    ModModelPlus* _modModelPlus;
    ModPlusCtrl::Type _ctrlType;
    void insertCtrl(ModPlusCtrl::Type,ModPlusCtrl*);

    static ModPlusCtrl* getNewCtrl(ModPlusCtrl::Type type,Project* project,ModModelPlus* modModelPlus);

};

#endif // MODPLUSCTRLS_H
