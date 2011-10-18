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

 	@file Problems.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#if !defined(_OMCASES_H)
#define _OMCASES_H

#include <vector>
#include <QtCore/QObject>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QTextStream>
#include <QtCore/QVector>
#include "OMCase.h"
#include "Problem.h"
#include "Result.h"

class OMCases : public QAbstractItemModel
{
public:
        OMCases(QString name="Problems");
        virtual ~OMCases(void);


        QVector<OMCase*> items;

	
	void reset();

	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
        void addCase(OMCase*);
	bool removeRow(int row,const QModelIndex &parent = QModelIndex());
	int findItem(QString);
        bool contains(OMCase* item){return items.contains(item);}

        OMCase* at(int i) const {return items.at(i);}
        int size() const{return items.size();}


private :
        QString name;
};


/**
  * OMCasesCombiner is a class used to display problems and results in only one structure.
  */
class OMCasesCombiner : public QAbstractItemModel
{
    Q_OBJECT
public:
        OMCasesCombiner(OMCases *problems,OMCases *results);
        virtual ~OMCasesCombiner(void);

        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation,
                int role = Qt::DisplayRole) const;
        QModelIndex index(int row, int column,
                const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

        OMCase* item(const QModelIndex &);

public slots :
        void rowsAboutToBeInsertedPb( const QModelIndex & parent, int start, int end ) ;
        void rowsAboutToBeRemovedPb( const QModelIndex & parent, int start, int end );
        void rowsInsertedPb( const QModelIndex & parent, int start, int end );
        void rowsRemovedPb( const QModelIndex & parent, int start, int end );
        void dataChangedPb( const QModelIndex & topLeft, const QModelIndex & bottomRight )    ;
        void rowsAboutToBeInsertedRes( const QModelIndex & parent, int start, int end );
        void rowsAboutToBeRemovedRes( const QModelIndex & parent, int start, int end );
        void rowsInsertedRes( const QModelIndex & parent, int start, int end );
        void rowsRemovedRes( const QModelIndex & parent, int start, int end );
        void dataChangedRes( const QModelIndex & topLeft, const QModelIndex & bottomRight );


private :
        QString _name;
        OMCases* _problems;
        OMCases* _results;
        QModelIndex problemsRootIndex() const;
        QModelIndex resultsRootIndex() const;

        enum node
        {
            PROBLEMROOT,
            RESULTROOT
        };
};


#endif
