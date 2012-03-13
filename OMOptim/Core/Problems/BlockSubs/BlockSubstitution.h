// $Id$
 /**
     @file BlockSubstitution.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 

  */
#if !defined(_BLOCKSUBSTITUTION_H)
#define _BLOCKSUBSTITUTION_H

#include "ModItemsTree.h"
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#include "ModelicaConnections.h"

class Project;
class ModModelPlus;
class BlockSubstitution
{
public:
    
        BlockSubstitution(Project* project,ModModelPlus* model, QString orgComponent, QString subComponent,
                                                                         QStringList orgPorts,QList<QStringList> orgConnectedComps,
                                                                         QStringList subPorts,QList<QStringList> subConnectedComps);

        BlockSubstitution(Project*,ModModelPlus*,ModelicaConnections*,QString orgComponent,QString subComponent,bool doAutoConnect, bool &ok);
        BlockSubstitution(Project*,ModModelPlus*,ModelicaConnections*,QDomElement domEl );

    
    ~BlockSubstitution(void);

        void setSubComponent(QString subComponent,bool doAutoConnect);

private :
        bool init(Project*,ModModelPlus*,ModelicaConnections*,QString _orgComponent,QString _subComponent);

public :
    void autoConnect();

public :

    BlockSubstitution* clone() const;
    void copyFrom(BlockSubstitution*);

    QDomElement toXmlData(QDomDocument & doc);

        Project *_project;
        ModModelPlus* _model;

        QString _orgComponent;
        QString _subComponent;

        QStringList _orgPorts; // ports inside original component
        QList<QStringList> _orgConnectedComps; // components connected to original component ports
    
        QStringList _subPorts; // corresponding ports inside replacing component
        QList<QStringList> _subConnectedComps; // components connected to replacing component ports
    
};



#endif
