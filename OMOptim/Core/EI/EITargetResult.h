// $Id$
/**
@file EITargetResult.h
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version

*/
#ifndef _EITARGETRESULT_H
#define _EITARGETRESULT_H

#include "Result.h"
#include "EITree.h"
#include "EIConns.h"
#include "MOVector.h"
#include "EIControler.h"

using namespace EI;

class EITarget;


class EITargetResult : public Result
{
public:
        EITargetResult(Project*, ModClassTree*, Problem*);
        EITargetResult(Project* project,ModClassTree*,QDomElement domResult,Problem* problem);
        EITargetResult(Project*);
	~EITargetResult(void);

        double _totalCost;


        static QString className(){return "EITargetResult";};
        virtual QString getClassName(){return EITargetResult::className();};

        int problemType(){return Problem::EITARGETTYPE;};
        QDomElement toXmlData(QDomDocument &);

        EITree* eiTree();
        void setEITree(const EITree&);

        EIConns* eiConns();
        void setEIConns(EIConns*);

    protected :
        EITree* _eiTree;
        EIConns* _eiConns;

    public :
        // file paths
        QString _logFileName;
        QString _resFileName;
        QString _sensFileName;

};


#endif
