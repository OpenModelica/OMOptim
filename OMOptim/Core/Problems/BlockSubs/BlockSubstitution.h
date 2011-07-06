// $Id$
 /**
 	@file BlockSubstitution.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_BLOCKSUBSTITUTION_H)
#define _BLOCKSUBSTITUTION_H

#include "ModClassTree.h"
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#include "ModelicaConnections.h"

class Project;
class ModModelPlus;
class BlockSubstitution
{
public:
	
        BlockSubstitution(Project* _project,ModModelPlus* _model, ModClassTree* _modClassTree,QString _orgComponent, QString _subComponent,
									 QStringList _orgPorts,QList<QStringList> _orgConnectedComps,
									 QStringList _subPorts,QList<QStringList> _subConnectedComps);

        BlockSubstitution(Project*,ModModelPlus*,ModelicaConnections*,QString _orgComponent,QString _subComponent,ModClassTree* _modClassTree,bool doAutoConnect, bool &ok);
        BlockSubstitution(Project*,ModModelPlus*,ModelicaConnections*,QDomElement _domEl,ModClassTree* );

	
	~BlockSubstitution(void);

	void setSubComponent(QString _subComponent,bool doAutoConnect);

private :
        bool init(Project*,ModModelPlus*,ModelicaConnections*,QString _orgComponent,QString _subComponent,ModClassTree* _modClassTree);

public :
	void autoConnect();

public :

	BlockSubstitution* clone();
	void copyFrom(BlockSubstitution*);

	QDomElement toXmlData(QDomDocument & doc);

	Project *project;
	ModModelPlus* model;
        ModClassTree* modClassTree;

	QString orgComponent;
	QString subComponent;

	QStringList orgPorts; // ports inside original component
	QList<QStringList> orgConnectedComps; // components connected to original component ports
	
	QStringList subPorts; // corresponding ports inside replacing component
	QList<QStringList> subConnectedComps; // components connected to replacing component ports
	
};



#endif
