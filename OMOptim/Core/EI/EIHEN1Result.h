// $Id$
/**
@file EIHEN1Result.h
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version 0.9 

*/
#ifndef _EIHEN1RESULT_H
#define _EIHEN1RESULT_H

#include "Result.h"
#include "EITree.h"
#include "EIConns.h"
#include "MOVector.h"

class EIHEN1Result : public Result
{
public:
	EIHEN1Result(Project*, Problem*);
        EIHEN1Result(const EIHEN1Result &);
	EIHEN1Result(void);
	~EIHEN1Result(void);

        double _totalCost;
        double _totalArea;
        double _HENumber;


	virtual QString getClassName(){return "EIHEN1Result";};

        int problemType(){return Problem::EIHEN1;};
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
