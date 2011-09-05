// $Id$
/**
@file EIHEN1Result.h
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version

*/
#ifndef _EIHEN1RESULT_H
#define _EIHEN1RESULT_H

#include "Result.h"
#include "EITree.h"
#include "EIConns.h"
#include "MOVector.h"
#include "EIControler.h"
#include "EIHEN.h"

using namespace EI;

class EIHEN1Result : public Result
{
public:
	EIHEN1Result(Project*, Problem*);
        EIHEN1Result(Project* project,ModClassTree*,QDomElement domResult,Problem* problem);
        //EIHEN1Result(const EIHEN1Result &);
	EIHEN1Result(void);
	~EIHEN1Result(void);

        double _totalCost;
        double _totalArea;
        double _HENumber;


        static QString className(){return "EIHEN1Result";};
        virtual QString getClassName(){return EIHEN1Result::className();};

        int problemType(){return Problem::EIHEN1TYPE;};
        QDomElement toXmlData(QDomDocument &);

        EITree* eiTree();
        void setEITree(const EITree&);

        EIConns* eiConns();
        void setEIConns(EIConns*);

        EIHEN* eiHen();


    protected :
        EITree* _eiTree;
        EIConns* _eiConns;
        EIHEN* _eiHEN;

    public :
        // file paths
        QString _logFileName;
        QString _resFileName;
        QString _sensFileName;

};


#endif
