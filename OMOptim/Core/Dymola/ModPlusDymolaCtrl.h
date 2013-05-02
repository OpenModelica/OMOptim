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
#include "DymolaParameters.h"




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
  bool readOutputVariablesDSRES(MOVector<Variable> *,QString dsresFile);
  bool readOutputVariablesDSFINAL(MOVector<Variable> *,QString dsfinalFile);
  bool readInitialVariables(MOVector<Variable> *, const QFileInfoList filesToCopy,bool forceRecompile, QString dsinFile="");

  // compatible models
  virtual QList<ModelPlus::ModelType> compatibleModels();


  // Compile function
  bool createDsin(QFileInfoList moDeps,QFileInfoList filesToCopy);
  bool isCompiled();
  bool uncompile();
  bool compile(const QFileInfoList & moDependencies, const QFileInfoList filesToCopy);

  // Simulate function
  bool simulate(QDir tempDir,MOVector<Variable> * updatedVars,MOVector<Variable> * outputVars,
                QFileInfoList filesTocopy,QFileInfoList moDependencies);
  void stopSimulation();
  bool canBeStoped();
  bool setStopTime(double time);



private:
  QString _dsinFile;
  QString _dsresFile;
  QString _dsfinalFile;
  QProcess _simProcess;
  ModModelPlus *_modModelPlus; /// cast of _ModelPlus

};



#endif
