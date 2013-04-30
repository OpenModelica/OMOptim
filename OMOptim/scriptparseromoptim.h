#ifndef SCRIPTPARSEROMOPTIM_H
#define SCRIPTPARSEROMOPTIM_H

#include "scriptparser.h"

class Project;

class ScriptParserOMOptim : public ScriptParserOMOptimBasis
{
public:
    ScriptParserOMOptim(Project*);


protected:
  virtual bool launchFunction(QString function, QStringList args, bool & foundFunction);
    Project* _project;
};

#endif // SCRIPTPARSEROMOPTIM_H
