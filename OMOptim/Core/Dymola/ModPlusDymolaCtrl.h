// $Id$
/**
@file ModPlusDymolaCtrl.h
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version 0.9 

*/
#ifndef _MODPLUSDYMOLACTRL_H
#define _MODPLUSDYMOLACTRL_H

#include "ModPlusCtrl.h"
#include "Dymola.h"

class ModPlusDymolaCtrl :public ModPlusCtrl
{

	enum OutputReadMode
	{
		DSFINAL,
		DSRES
	};

public:
	ModPlusDymolaCtrl(ModModelPlus* _model,MOomc* _oms,QString _mmoFolder,QString _moFilePath,QString modModelName);
	~ModPlusDymolaCtrl(void);

	void setMmoFolder(QString);
	ModPlusCtrl::Type type();

	// Variables functions
	bool readOutputVariables(MOVector<Variable> *,QString folder="");
	bool readOutputVariablesDSRES(MOVector<Variable> *,QString _dsresFile);
	bool readOutputVariablesDSFINAL(MOVector<Variable> *,QString _dsfinalFile);
	bool readInitialVariables(MOVector<Variable> *,QString _dsinFile="");

	// Parameters
	void setDefaultParameters();
	
	// Compile function
	bool createDsin();
	bool isCompiled();
	bool compile();
	
	// Simulate function
	bool simulate(QString tempDir,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars);


private:
	QString _dsinFile;
	QString _dsresFile;
	QString _dsfinalFile;
	OutputReadMode _outputReadMode;

};



#endif
