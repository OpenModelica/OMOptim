// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file SimpleMilpTarget.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "SimpleMilpTarget.h"
#ifdef WIN32
#include <windows.h>
#endif


SimpleMilpTarget::SimpleMilpTarget(EITree* _eiTree,MOOptVector *_variables,QString _modFilePath, QString _dataFilePath)
{
        eiTree = _eiTree ;
	variables = _variables;
	modFilePath =  _modFilePath;
	dataFilePath = _dataFilePath;
}

SimpleMilpTarget::~SimpleMilpTarget(void)
{
}


EITargetResult* SimpleMilpTarget::launch()
{
	//***********************
	// get data
	//***********************
	// get temperature intervals, flows, factors...
	QList<METemperature> Tk;
	QList<EIStream*> eiProcessStreams;
	QList<QList<MEQflow> > Qpk; //.at(iStream).at(iDTk)
	QList<EIStream*> eiUtilityStreams;
	QList<QList<MEQflow> > Quk;//.at(iStream).at(iDTk)
	QMultiMap<EIGroupFact*,EIStream*> factStreamMap; // multimap <unit multiplier, Streams concerned>,
	QMap<EIGroupFact*,EIGroupFact*> factsRelation; // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
	QMap<EIGroupFact*,EIGroup*> factGroupMap;

        EITools::getTkQpkQuk(variables,
                eiTree->rootElement(),Tk,
		eiProcessStreams,Qpk,
		eiUtilityStreams,Quk,
		factStreamMap,
		factsRelation,
                factGroupMap,
                             true);

	// write data
	DataToFile(dataFilePath,Tk,eiProcessStreams,Qpk,eiUtilityStreams,Quk,
		factStreamMap,factsRelation,factGroupMap);

	//data
	QFileInfo dataInfo(dataFilePath);
	QString resFilePath = dataInfo.absolutePath() + QDir::separator() + "MILPResult.txt";
        QString logFilePath = dataInfo.absolutePath() + QDir::separator() + "MILPLog.txt";


        glp_prob* glpProblem = launchGLPK(resFilePath,logFilePath);
	EITargetResult* result;
	if(!glpProblem)
	{
		result = new EITargetResult();
		result->setSuccess(false);
	}
	else
	{
		result = readResult(glpProblem);
		glp_delete_prob(glpProblem);
	}
	return result;
}



void SimpleMilpTarget::DataToFile(QString dataFilePath, QList<METemperature> &Tk,
	QList<EIStream*> &eiProcessStreams,
	QList<QList<MEQflow> > &Qpk, //.at(iStream).at(iDTk)
	QList<EIStream*> &eiUtilityStreams,
	QList<QList<MEQflow> > &Quk,//.at(iStream).at(iDTk)
	QMultiMap<EIGroupFact*,EIStream*> &factStreamMap, // multimap <unit multiplier, Streams concerned>,
	QMap<EIGroupFact*,EIGroupFact*> &factsRelation, // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
	QMap<EIGroupFact*,EIGroup*> &factGroupMap)
{
	// Create Data File
	QFile file(dataFilePath);
	if(file.exists())
	{
		file.remove();
	}
	file.open(QIODevice::WriteOnly);
	
	// Create text
	QString dataText;
	dataText.append("data;");
	
	//*************
	// sets
	//*************
	QList<double> doubleSet;
	// STk
	for(int i=0;i<Tk.size();i++)
	{
		doubleSet.push_back(Tk.at(i).value(METemperature::K));
	}
	dataText += GlpkTools::listToSet("STk",doubleSet);
	
	// SProcStreams
	QStringList stringSet;
	for(int i=0;i<eiProcessStreams.size();i++)
		stringSet.push_back(eiProcessStreams.at(i)->name());
	dataText += GlpkTools::listToSet("SProcStreams",stringSet);

	// SUtStreams
	stringSet.clear();
	for(int i=0;i<eiUtilityStreams.size();i++)
		stringSet.push_back(eiUtilityStreams.at(i)->name());
	dataText += GlpkTools::listToSet("SUtStreams",stringSet);

	// SUtGroups
	stringSet.clear();
	for(int i=0;i<factGroupMap.values().size();i++)
		stringSet.push_back(factGroupMap.values().at(i)->name());
	dataText += GlpkTools::listToSet("SUtGroups",stringSet);

	// SUtStrGroups
	EIGroupFact* curGroupFact;
	EIGroup* curGroup;
	EIStream* curStream;
	QString setUtStrGrText = "set SUtStrGroups := ";
	for(int iFact=0;iFact<factStreamMap.uniqueKeys().size();iFact++)
	{
		curGroupFact = factStreamMap.uniqueKeys().at(iFact);
		curGroup = factGroupMap.value(curGroupFact);

		for(int iS=0;iS<factStreamMap.values(curGroupFact).size();iS++)
		{
			curStream = factStreamMap.values(curGroupFact).at(iS);
			setUtStrGrText+="("+curGroup->name()+","+curStream->name()+") ";
		}		
	}
	setUtStrGrText += "; \n";
	dataText += setUtStrGrText;


	//*************
	// param
	//*************
	dataText += "param nbTemperatures := "+QString::number(Tk.size())+"; \n";
	dataText += "param nbUtGroups := "+QString::number(factGroupMap.values().size())+"; \n";


	//DQpk
	dataText += "param DQpk := \n";
	double curValue;
	for(int iS=0;iS<Qpk.size();iS++)
	{
		for(int iK=0;iK<Qpk.at(iS).size();iK++)
		{
			curValue = Qpk.at(iS).at(iK).value(MEQflow::KW);
			if(curValue!=0)
				dataText += "["+eiProcessStreams.at(iS)->name()
				+"," +QString::number(iK+1)
				+"] " + QString::number(-curValue) + "\n"; 
		}
	}
	dataText +=";";

	//DQuk
	dataText += "param DQuk := \n";
	for(int iS=0;iS<Quk.size();iS++)
	{
		for(int iK=0;iK<Quk.at(iS).size();iK++)
		{
			curValue = Quk.at(iS).at(iK).value(MEQflow::KW);
			if(curValue!=0)
				dataText += "["+eiUtilityStreams.at(iS)->name()
				+"," +QString::number(iK+1)
				+"] " + QString::number(-curValue) + "\n"; 
		}
	}
	dataText +=";";

	//fmin, fmax, cost
	QString strFmax = "param fmax := ";
	QString strFmin = "param fmin := ";
	QString strCostFix = "param costFix := ";
	QString strCostMult = "param costMult := ";

	for(int iFact=0;iFact<factGroupMap.uniqueKeys().size();iFact++)
	{
		curGroupFact = factGroupMap.uniqueKeys().at(iFact);
		curGroup = factGroupMap.value(curGroupFact);

		strFmax += "["+curGroup->name()+"] " 
			+ QString::number(curGroupFact->max)+"\n"; 
		strFmin += "["+curGroup->name()+"] " 
			+ QString::number(curGroupFact->min)+"\n"; 
		strCostFix += "["+curGroup->name()+"] " 
			+ curGroup->getFieldValue(EIGroup::COSTFIX).toString()+"\n"; 
		strCostMult += "["+curGroup->name()+"] " 
			+ curGroup->getFieldValue(EIGroup::COSTMULT).toString()+"\n"; 
	}

	strFmax += "; \n";
	strFmin += "; \n";
	strCostFix += "; \n";
	strCostMult += "; \n";


	dataText+= strFmax;
	dataText+= strFmin;
	dataText+= strCostFix;
	dataText+= strCostMult;



	dataText +="end;";




	QTextStream ts( &file );
	ts << dataText;
	file.close();
}

glp_prob* SimpleMilpTarget::launchGLPK(QString resFilePath,QString logFilePath)
{

	//delete result file
	QFile resFile(resFilePath);
	if(resFile.exists())
		resFile.remove();


	glp_prob *glpProblem;
	glp_tran *tran;
	int ret;
	glpProblem = glp_create_prob();
	tran = glp_mpl_alloc_wksp();
	ret = glp_mpl_read_model(tran, modFilePath.toLatin1().data(), 1);
	if (ret != 0)
	{ 
		infoSender.send(Info(ListInfo::MILPERRORMODEL));
		return NULL;
	}
	ret = glp_mpl_read_data(tran, dataFilePath.toLatin1().data());
	if (ret != 0)
	{ 
		infoSender.send(Info(ListInfo::MILPERRORDATA));
		return NULL;
	}
	ret = glp_mpl_generate(tran, NULL);
	if (ret != 0)
	{
		infoSender.send(Info(ListInfo::MILPERRORDATA));
		return NULL;
	}
	
	glp_mpl_build_prob(tran, glpProblem);
	glp_simplex(glpProblem, NULL);
	glp_intopt(glpProblem, NULL);
	ret = glp_mpl_postsolve(tran, glpProblem, GLP_MIP);
	if (ret != 0)
		fprintf(stderr, "Error on postsolving model\n");
	
	glp_mpl_free_wksp(tran);
	glp_print_sol(glpProblem, resFilePath.toLatin1().data());




	// get results
	int nbRows = glp_get_num_rows(glpProblem);
	int nbCols = glp_get_num_cols(glpProblem);

	QString resText;
	QString rowName;
	QString colName;
	double value;
	for(int iRow = 1;iRow<=nbRows; iRow++)
	{
		rowName = QString(glp_get_row_name(glpProblem, iRow));
		value = glp_mip_row_val(glpProblem, iRow);
		resText += "Row[" + QString::number(iRow)+"] : ";
		resText += rowName + " : ";
		resText += QString::number(value) +"\n";
	}
	for(int iCol = 1;iCol<=nbCols; iCol++)
	{
		colName = QString(glp_get_col_name(glpProblem, iCol));
		value = glp_mip_col_val(glpProblem, iCol);
		resText += "Col[" + QString::number(iCol)+"] : ";
		resText += colName + " : ";
		resText += QString::number(value) +"\n";
	}

	QFile resFile3(resFilePath+"3");
	if(resFile3.exists())
	{
		resFile3.remove();
	}
	resFile3.open(QIODevice::WriteOnly);
	QTextStream ts( &resFile3 );
	ts << resText;
	resFile3.close();

	
	return glpProblem;
}







EITargetResult* SimpleMilpTarget::readResult(glp_prob * glpProblem)
{

	// clone rootEI
	EITargetResult* result = new EITargetResult();
        result->setEITree(*eiTree);

	// read if successfull
	int status = glp_get_status(glpProblem);
	switch(status)
	{
	case GLP_OPT :
		result->setSuccess(true);
		break;
	default:
		result->setSuccess(false);
	}



	// read all VFacMul
	QMap<QString,double> mapGroupFacMul; //<groupName,value>
	QString colName;
	QRegExp regExp("VFactUt[.]*");
	

	QString groupName,prefix,valueStr;
	double value;

	int nbCols = glp_get_num_cols(glpProblem);
	for(int iCol = 1;iCol<=nbCols; iCol++)
	{
		colName = QString(glp_get_col_name(glpProblem, iCol));
		if (regExp.indexIn(colName) != -1)
		{
			groupName = colName;
			groupName.remove("VFactUt");
			groupName.remove("[");
			groupName.remove("\'");
			groupName.remove("]");
			groupName.remove(QRegExp("^\\."));
			value = glp_mip_col_val(glpProblem, iCol);
			mapGroupFacMul.insert(groupName,value);
		}
	}

	EIItem* curGroup;

	for(int i=0;i<mapGroupFacMul.keys().size();i++)
	{
		groupName = mapGroupFacMul.keys().at(i);
                curGroup = eiTree->findItem(groupName);
		if(curGroup)
		{
                    value = mapGroupFacMul.value(groupName);
                        dynamic_cast<EIGroup*>(curGroup)->getFact()->value=value;
                        if(value==0)
                            dynamic_cast<EIGroup*>(curGroup)->setChecked(false);
		}
	}


	// read TotalCost
	return result;
}
//
//void SimpleMilpTarget::RootEIToTargetMilp(EIItem* rootEI,MOOptVector *variables)
//{
//
//	// get temperature intervals, flows, factors...
//	QList<METemperature> Tk;
//	QList<EIStream*> eiProcessStreams;
//	QList<QList<MEQflow> > Qpk; //.at(iStream).at(iDTk)
//	QList<EIStream*> eiUtilityStreams;
//	QList<QList<MEQflow> > Quk;//.at(iStream).at(iDTk)
//	QMultiMap<EIGroupFact*,EIStream*> factStreamMap; // multimap <unit multiplier, Streams concerned>,
//	QMap<EIGroupFact*,EIGroupFact*> factsRelation; // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
//	QMap<EIGroupFact*,EIGroup*> factGroupMap;
//
//	EITools::getTkQpkQuk(variables,
//		rootEI,Tk,
//		eiProcessStreams,Qpk,
//		eiUtilityStreams,Quk,
//		factStreamMap,
//		factsRelation,
//		factGroupMap);
//
//	QList<EIGroupFact*> facts = factStreamMap.uniqueKeys();
//
//	//*****************************************************
//	// sets
//	//*****************************************************
//	int nbFacts = facts.size();
//	MP_set SDTk(Tk.size()-1); // temperatures' intervals
//	MP_set STk(Tk.size()); // temperatures
//	MP_set SProcStreams(eiProcessStreams.size());
//	MP_set SUtStreams(eiUtilityStreams.size());
//	MP_set SUtilities(nbFacts);
//	
//	//*****************************************************
//	// Data
//	//*****************************************************
//	
//	//Declaration
//	MP_data DQpk(SProcStreams,SDTk);
//	MP_data DQuk(SUtStreams,SDTk);
//	MP_data FactOfStr(SUtStreams);
//	MP_data zero;
//	MP_data Identity(SUtilities);
//	MP_data fmin(SUtilities);
//	MP_data fmax(SUtilities);
//	MP_data costFix(SUtilities);
//	MP_data costMult(SUtilities);
//	
//
//	//Filling
//	zero()=0;
//
//	// fill DQpk and DQuk (kW)
//	for(int iDT=0;iDT<Tk.size()-1;iDT++)
//	{
//		for(int iUS=0;iUS<eiUtilityStreams.size();iUS++)
//		{
//			DQuk(iUS,iDT)=Quk.at(iUS).at(iDT).value(MEQflow::KW);
//		}
//		for(int iPS=0;iPS<eiProcessStreams.size();iPS++)
//		{
//			DQpk(iPS,iDT)=Qpk.at(iPS).at(iDT).value(MEQflow::KW);
//		}
//	}
//
//	// fill FactOfStr(SUtStreams)
//	EIStream* curStream;
//	EIGroupFact* curGroupFact;
//	int iFact;
//	for(int iUS=0;iUS<eiUtilityStreams.size();iUS++)
//	{
//		curStream = eiUtilityStreams.at(iUS);
//		curGroupFact = factStreamMap.key(curStream,NULL);
//		if(curGroupFact)
//		{
//			iFact = facts.indexOf(curGroupFact);
//			FactOfStr(iUS)=iFact;
//		}
//	}
//
//	// fill Identity, fmin, fmax, costFix, costMult (SUtilities)
//	EIGroup* curGroup;
//	for(int i=0;i<nbFacts;i++)
//	{
//		Identity(i)=i;
//		curGroupFact = facts.at(i);
//		curGroup = factGroupMap.value(facts.at(i),NULL);
//		if(curGroupFact)
//		{
//			assert(curGroupFact==curGroup->getFact());
//
//			fmin(i) = curGroupFact->min;
//			fmax(i) = curGroupFact->max;
//			costFix(i) = curGroup->getFieldValue(EIGroup::COSTFIX).toDouble();
//			costMult(i) = curGroup->getFieldValue(EIGroup::COSTMULT).toDouble();
//		}
//	}
//
//
//
//	//*****************************************************
//	// Variables
//	//*****************************************************
//	MP_variable VFactUt(SUtilities); //utility multiplication factors
//	MP_variable VRk(STk);
//
//
//	//*****************************************************
//	// Expressions
//	//*****************************************************
//	
//	
//	//*****************************************************
//	// Constraints
//	//*****************************************************
//	MP_index ik;
//	MP_index is;
//	MP_index iu;
//	MP_constraint
//		CDQk(SDTk),
//		CRk(SDTk),
//		CR1,
//		CRkn;
//
//	// temperature interval heat equation
//
//	CDQk(ik) =
//		// sum fw * sum(qwk)
//		sum(SUtilities(iu),VFactUt(iu)*sum(SUtStreams(is),DQuk(is,ik).such_that(Identity(iu) == FactOfStr(is))))
//		// sum Qik
//		+sum(SProcStreams,DQpk(SProcStreams,ik))
//		// + Rk+1 - Rk
//		+ VRk(ik+1)-VRk(ik)
//		//==0
//		==zero(0);
//
//	// no heat propagation from cold to hot temperature zone
//	CRk(SDTk) = VRk(SDTk) >= zero(0);
//
//	// all needs provided
//	CR1() = VRk(0) == zero(0);
//	CRkn() = VRk(SDTk+1) == zero(0);
//
//	// multiplication factors
//	for(int iFact=0;iFact<nbFacts;iFact++)
//	{
//		VFactUt.lowerLimit(iFact) = facts.at(iFact)->min;
//		VFactUt.upperLimit(iFact) = facts.at(iFact)->max;
//	}
//
//
//	// relation between multiplication factors
//	//(when utility groups are included in utility groups)
//	EIGroupFact* childFact;
//	EIGroupFact* parentFact;
//	MP_constraint facRel(SUtilities);
//	for(int iFact=0;iFact<factsRelation.keys().size();iFact++)
//	{
//		childFact = factsRelation.keys().at(iFact);
//		parentFact = factsRelation.values().at(iFact);
//		
//		int iChildFac = facts.indexOf(childFact);
//		int iParentFac = facts.indexOf(parentFact);
//		if((iChildFac>-1)&&(iParentFac>-1))
//		{
//			facRel(iChildFac)= VFactUt(iParentFac)*facts.at(iFact)->max - VFactUt(iChildFac) >=zero(0);
//			facRel(iChildFac)= VFactUt(iParentFac)*facts.at(iFact)->min - VFactUt(iChildFac) <=zero(0);
//		}
//	}
//
//	//*****************************************************
//	// Objective
//	//*****************************************************
//	MP_variable totalCost;
//	MP_constraint totalCostDef;
//	totalCostDef() = totalCost() == sum(SUtilities(iu),costFix(iu)+costMult(iu)*VFactUt(iu));
//
//	//*****************************************************
//	// Model
//	//*****************************************************
//	FlopMessenger flopMessenger;
//	MP_model M(new OsiCbcSolverInterface,&flopMessenger);
//	M.minimize(totalCost());
//	totalCost.display("totalCost");
//	VFactUt.display("VFactUt");
//	double _facMul = VFactUt.level(0);
//	double _totalCost = totalCost.level(0); 
//	M.Solver->writeLp("MILPProblem","txt");
//
//	//*****************************************************
//	// Display
//	//*****************************************************
//	MP_model::MP_status status = M.getStatus();
//}
//
//
//

	




