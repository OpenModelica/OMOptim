// $Id$
/**
@file EITargetResult.h
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version 0.9 

*/
#ifndef _EITARGETRESULT_H
#define _EITARGETRESULT_H

#include "Result.h"
#include "EITree.h"
#include "EIConns.h"
#include "MOVector.h"

class EITargetResult : public Result
{
public:
	EITargetResult(Project*, Problem*);
	EITargetResult(void);
	~EITargetResult(void);

	double totalCost;


	virtual QString getClassName(){return "EITargetResult";};

        int problemType(){return Problem::EIPROBLEM;};
        QDomElement toXmlData(QDomDocument &);

        EITree* eiTree();
        void setEITree(EITree*);

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
