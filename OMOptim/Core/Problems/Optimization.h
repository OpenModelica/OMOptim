// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR 
 * THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE. 
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file Optimization.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_OPTIMIZATION_H)
#define _OPTIMIZATION_H


#include "Problem.h"
#include "Result.h"

#include "MOVector.h"
#include "OptObjective.h"
#include "OptimResult.h"
#include "ProblemConfig.h"
#include "EIProblem.h"

#include "BlockSubstitution.h"
#include "BlockSubstitutions.h"


class EABase;

class Optimization : public Problem
{
	Q_OBJECT

protected :
	ModModelPlus* _modModelPlus;
	MOVector<ScannedVariable> *_scannedVariables;
	MOVector<OptVariable> *_optimizedVariables;
	MOVector<OptObjective> *_objectives;
	BlockSubstitutions *_blockSubstitutions;

	// MER problem
	bool _useEI;
        EIProblem* _eiProblem;
	bool _useScan;


public:
	//Optimization(void);
	Optimization(Project*,ModClass* _rootModelClass,ModReader*,ModPlusCtrl*,ModModelPlus* _mainModelPlus);
	Optimization(const Optimization &);
	~Optimization(void);
	virtual QString getClassName(){return "Optimization";};

	//Get functions
	ModModelPlus* modModelPlus();
	MOVector<ScannedVariable> *scannedVariables(){return _scannedVariables;};
	MOVector<OptVariable> *optimizedVariables(){return _optimizedVariables;};
	MOVector<OptObjective> *objectives(){return _objectives;};
	BlockSubstitutions *blockSubstitutions(){return _blockSubstitutions;};
	bool useEI(){return _useEI;};
	EIProblem* eiProblem(){return _eiProblem;};
	
	//overwrited functions
	bool checkBeforeComp(QString & error);
	void launch(ProblemConfig _config);
	void store(QString destFolder, QString tempDir);
	QDomElement toXmlData(QDomDocument & doc);
	
	//specific functions
	void createSubExecs(QList<ModModelPlus*> & _subMod, QList<BlockSubstitutions*> & _subBlocks);

	//block substitution
	void setBlockSubstitutions(BlockSubstitutions*);

	int nbScans();

	//void setEAConfigs(QVector<EAConfig*>);
	//EAConfig* getEAConfig(int iEA);
	//QVector<EAConfig*> getEAConfigs();
	
protected:
	//QVector<EAConfig*> eaConfigs;
	
	public slots:
	//	void onEAConfigChanged();

signals:
	//	void EAConfigChanged();
	

};


#endif
