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
    enum OutputReadMode
    {
            DSFINAL,
            DSRES
    };

        ModPlusDymolaCtrl(Project* project,ModModelPlus* model,MOomc* omc);
        ~ModPlusDymolaCtrl(void);
        ModPlusCtrl* clone();

        ModPlusCtrl::Type type() const;
        QString name();

        // Variables functions
        bool readOutputVariables(MOVector<Variable> *,QString folder="");
        bool readOutputVariablesDSRES(MOVector<Variable> *,QString _dsresFile);
        bool readOutputVariablesDSFINAL(MOVector<Variable> *,QString _dsfinalFile);
        bool readInitialVariables(MOVector<Variable> *,bool forceRecompile,QString _dsinFile="");

        // Parameters
        void setDefaultParameters();

        // Compile function
        bool createDsin();
        bool isCompiled();
        bool compile(const QStringList & moDependencies=QStringList());

        // Simulate function
        bool simulate(QString tempDir,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,
                      QStringList filesTocopy=QStringList(),QStringList moDependencies=QStringList());
        void stopSimulation();
        bool canBeStoped();


private:
        QString _dsinFile;
        QString _dsresFile;
        QString _dsfinalFile;
        OutputReadMode _outputReadMode;
        QProcess _simProcess;

};



#endif
