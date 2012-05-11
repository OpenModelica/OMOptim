// $Id$
/**
@file ModPlusDymolaCtrl.h
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version

*/
#ifndef _MODPLUSDYMOLACTRL_H
#define _MODPLUSDYMOLACTRL_H

#include <QtCore>
#include <limits>


#include "ModPlusCtrl.h"

class ModModelPlus;
class MOomc;
class Project;
class Variable;

class ModPlusDymolaCtrl :public ModPlusCtrl
{



public:


        ModPlusDymolaCtrl(Project* project,ModModelPlus* model,MOomc* omc);
        ~ModPlusDymolaCtrl(void);
        ModPlusCtrl* clone();

        ModPlusCtrl::Type type() const;
        QString name();

        // Variables functions
        bool readOutputVariables(MOVector<Variable> *,QString path);
        bool readOutputVariablesDSRES(MOVector<Variable> *,QString _dsresFile);
        bool readOutputVariablesDSFINAL(MOVector<Variable> *,QString _dsfinalFile);
        bool readInitialVariables(MOVector<Variable> *,bool forceRecompile,QString _dsinFile="");

        // Parameters
        void setDefaultParameters();

        // Compile function
        bool createDsin(QFileInfoList moDeps);
        bool isCompiled();
        bool compile(const QFileInfoList & moDependencies);

        // Simulate function
        bool simulate(QDir tempDir,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,
                      QFileInfoList filesTocopy,QFileInfoList moDependencies);
        void stopSimulation();
        bool canBeStoped();


private:
        QString _dsinFile;
        QString _dsresFile;
        QString _dsfinalFile;
        QProcess _simProcess;

};



#endif
