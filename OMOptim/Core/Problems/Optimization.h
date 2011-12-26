// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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
 	@version 

  */
#if !defined(_OPTIMIZATION_H)
#define _OPTIMIZATION_H


#include "Problem.h"
#include "Result.h"

#include "MOVector.h"
#include "OptObjective.h"
#include "OptimResult.h"
#include "ProblemConfig.h"

#include "BlockSubs/BlockSubstitution.h"
#include "BlockSubs/BlockSubstitutions.h"
#include "OptObjectives.h"


class EABase;

class Optimization : public Problem
{
	Q_OBJECT



protected :


        //Model
	ModModelPlus* _modModelPlus;
        ScannedVariables *_scannedVariables;
        OptVariables *_optimizedVariables;
        OptObjectives *_objectives;
	BlockSubstitutions *_blockSubstitutions;

        // Simulation controlers
        ModPlusCtrls* _ctrls;

	bool _useScan;

        // Algorithm information
        QList<OptimAlgo*> _algos;
        int _iCurAlgo;

public:
	//Optimization(void);
        Optimization(Project*,ModModelPlus* modModelPlus);
	Optimization(const Optimization &);
        Optimization(QDomElement domProblem,Project* project,bool &ok);
        virtual Problem* clone() const;
        virtual ~Optimization(void);

        static QString className(){return "Optimization";};
        virtual QString getClassName(){return Optimization::className();};
        virtual bool hasQuickEndOption(){return true;}

	//Get functions
        ModModelPlus* modModelPlus() const;
        ScannedVariables* scannedVariables()const{return _scannedVariables;};
        OptVariables *optimizedVariables()const{return _optimizedVariables;};
        OptObjectives *objectives()const{return _objectives;};
        BlockSubstitutions *blockSubstitutions()const{return _blockSubstitutions;};

	
        //overwrited functions (also virtual since other problems might inherit Optimization)
        virtual bool checkBeforeComp(QString & error);
        virtual Result* launch(ProblemConfig _config);
        virtual void store(QString destFolder, QString tempDir);
        virtual QDomElement toXmlData(QDomDocument & doc);
	
	//specific functions
	void createSubExecs(QList<ModModelPlus*> & _subMod, QList<BlockSubstitutions*> & _subBlocks);

        //algo functions
        int getiCurAlgo();
        OptimAlgo* getCurAlgo() const;
        QStringList getAlgoNames();
        void setiCurAlgo(int);

        // controlers
        ModPlusCtrl* ctrl() const;
        ModPlusCtrls* ctrls() const;
        ModPlusCtrl::Type  ctrlType() const;
        void setCtrlType(ModPlusCtrl::Type);


	//block substitution
	void setBlockSubstitutions(BlockSubstitutions*);

	int nbScans();

        // recompute points
        virtual void recomputePoints(OptimResult*, std::vector<int>,bool forceRecompute = false);
	
public slots :
        void onQuickEndAsked();

	
};


#endif
