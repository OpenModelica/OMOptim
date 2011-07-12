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

 	@file ModClassTree.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#ifndef ModClassTree_H
 #define ModClassTree_H

 #include <QAbstractItemModel>
 #include <QModelIndex>
 #include <QVariant>

#include "ModClass.h"
#include "ModReader.h"
#include "MOomc.h"

 class ModClassTree : public QAbstractItemModel
 {
	Q_OBJECT

 public:






     ModClassTree(ModReader* _modReader,MOomc* moomc,QObject *parent = 0);
     virtual ~ModClassTree();
     ModClass* rootElement()const {return _rootElement;}


     // Read and fullfill functions
     void readFromOmc(ModClass*,int depthMax = 1000, int curDepth = 0);	//Read data and children with OMC calls
     void readFromOmcV2(ModClass*,int depthMax = 1000, QString direction ="", int curDepth = 0); 	//Read data and children with OMC calls
     void readFromOmcV3(ModClass*,int depthMax = 1000, QString direction ="", int curDepth = 0);	//Read data and children with OMC calls
     void readFromOmcV4(ModClass*,int depthMax = 1000, QString direction ="", int curDepth = 0);	//Read data and children with OMC calls, using OMEdit version


     // add functions
     bool addModClass(ModClass* parent,QString className,QString filePath="");
     bool addChild(ModClass* parent,ModClass* child);

     // Find functions
     bool isInDescendants(QString fullName,ModClass* parent=NULL);
     ModClass* findInDescendants(QString fullName,ModClass* parent=NULL);
     QList<ModClass*> findCompOfClassInDescendants(QString _className,ModClass* parent=NULL);

     ModModel* modelOf(ModClass* item);
     ModModel* modelOf(QString itemName);

     void childrenInfos(ModClass* parent,QStringList &packagesClasses,QStringList &modelsClasses,QStringList &compsNames,QStringList &compsClasses);

     //*****************************
     //Ports
     //*****************************
     QList<ModClass*> getPorts(ModClass* parent);
     QStringList getPorts(ModClass* parent,Modelica::NameFormat format);

     // remove functions
     void clear();



     // QAbstractItemModel functions
     QVariant data(const QModelIndex &index, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
     QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
     QModelIndex indexOf(ModClass*,int column=0);
     QModelIndex parent(const QModelIndex &index) const;
     bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
     int rowCount(const QModelIndex &parent = QModelIndex()) const;
	 int columnCount(const QModelIndex &parent = QModelIndex()) const;
	 ModClass* findItem(QString);


	 QStringList mimeTypes() const;
	 QMimeData* mimeData(const QModelIndexList &indexes) const;
     QIcon getModelicaNodeIcon(ModClass* modClass);
     bool canFetchMore ( const QModelIndex & parent ) const;
     void fetchMore ( const QModelIndex & parent );


 private:
         ModClass* _rootElement;
         ModReader* _modReader;
         bool _enabled;
         MOomc* _moomc;

         bool _showOnlyComponents;

	 public slots:
		 void allDataChanged();
		 void allDataCleared();
                 void onRootDeleted();
 };

 #endif
