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

class EITargetResult : public Result
{
public:
	EITargetResult(Project*, Problem*);
	EITargetResult(void);
	~EITargetResult(void);

	double totalCost;
        EIItem* _rootEI;

	virtual QString getClassName(){return "EITargetResult";};

	int problemType(){return Problem::PROBLEMEI;};
	QDomElement toXMLData(QDomDocument &);

};


#endif
