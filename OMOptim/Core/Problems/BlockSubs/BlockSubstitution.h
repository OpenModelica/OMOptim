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

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#endif


class Project;
class ModModelPlus;
class BlockSubstitution

{
public:

    BlockSubstitution(Project* project,QString model, QString orgComponent, QString subComponent,
                      QStringList orgPorts,QList<QStringList> orgConnectedComps,
                      QStringList subPorts,QList<QStringList> subConnectedComps);

    BlockSubstitution(Project*,QString model,QString orgComponent,QString subComponent,bool doAutoConnect, bool &ok);
    BlockSubstitution(Project*,QDomElement domEl,bool &ok);


    ~BlockSubstitution(void);

    void setSubComponent(QString subComponent,bool doAutoConnect);

private :
    bool init(Project*,QString model,QString _orgComponent,QString _subComponent);

public :
    void autoConnect();

public :

    BlockSubstitution* clone() const;
    void copyFrom(BlockSubstitution*);

    QDomElement toXmlData(QDomDocument & doc);

    Project *_project;
    QString _model;

    QString _orgComponent;
    QString _subComponent;

    QStringList _orgPorts; // ports inside original component
    QList<QStringList> _orgConnectedComps; // components connected to original component ports

    QStringList _subPorts; // corresponding ports inside replacing component
    QList<QStringList> _subConnectedComps; // components connected to replacing component ports

};



#endif
