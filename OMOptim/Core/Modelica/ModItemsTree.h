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

  @file ModItemsTree.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#ifndef ModItemsTree_H
#define ModItemsTree_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "ExeModel.h"
#include "ModItem.h"
#include "ModLoader.h"
#include "MOomc.h"



/**
  * \brief ModItemsTree is a container of Modelica models, packages, records...
  * ModItemsTree is a container of Modelica models, packages, records...
  * It represents items loaded in OMC. ModItemsTree contains its own reading functions allowing
  * it to fullfil itself communicating with OMC through _moomc.
  *
  * It inherits QAbstractItemModel in order to display contents easily in GUI (using a QTreeView).
  *
  * \todo Better split functions between ModItemsTree and ModLoader
  */

class ModItemsTree : public QAbstractItemModel
{
    Q_OBJECT

public:

    ModItemsTree(ModLoader* modLoader,MOomc* moomc,QObject *parent = 0);
    virtual ~ModItemsTree();
    ModItem* rootElement()const {return _rootElement;}



    bool addChild(ModItem* parent,ModItem* child);


    // Find functions
    bool isInDescendants(QString fullName,ModItem* parent=NULL);
    ModItem* findInDescendants(QString fullName,ModItem* parent=NULL);
    QList<ModItem*> findInDescendantsByClass(QString _className,ModItem* parent=NULL);
    QList<ModItem*> findInDescendantsByInheritedClass(QString _className,ModItem* parent=NULL);
    QList<ModItem*> findCompOfClassInDescendants(QString _className,ModItem* parent=NULL);

    ModModel* modelOf(ModItem* item);
    ModItem* modelOf(QString itemName);

    void childrenInfos(ModItem* parent,QStringList &packagesClasses,QStringList &modelsClasses,QStringList &recordsClasses,QStringList &compsNames,QStringList &compsClasses);

    //*****************************
    //Ports
    //*****************************
    QList<ModItem*> getPorts(ModItem* parent);
    QStringList getPorts(ModItem* parent,ModItem::NameFormat format);

    // remove functions
    void clear();


    // QAbstractItemModel functions
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex indexOf(ModItem*,int column=0);
    QModelIndex parent(const QModelIndex &index) const;
    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    ModItem* findItem(QString);
    QIcon getModelicaNodeIcon(ModItem* ModItem) const;
    bool canFetchMore ( const QModelIndex & parent ) const;
    void fetchMore ( const QModelIndex & parent );
    QMimeData* mimeData(const QModelIndexList &indexes) const;

    // drag and drop functions
 //   QStringList mimeTypes() const;
 //   QMimeData* mimeData(const QModelIndexList &indexes) const;
 //   Qt::DropActions supportedDropActions() const;

    void setShowComponent(bool);
    void emitDataChanged();
    void readFromOMCWThread(ModItem*,int depthMax = 1000,  QString direction ="", int curDepth = 0);    //Read data and children with OMC call
    void addExeModel(ModItem* parent,QString name, QString inputFile, QString exeFile);

public slots :
    // Read and fullfill functions
    void readFromOmc(ModItem*,int depthMax,  QString direction, int curDepth);    //Read data and children with OMC calls
    void readFromOmcV2(ModItem*,int depthMax = 1000, QString direction ="", int curDepth = 0);     //Read data and children with OMC calls
    void readFromOmcV3(ModItem*,int depthMax = 1000, QString direction ="", int curDepth = 0);    //Read data and children with OMC calls


private:
    ModItem* _rootElement;
    ModLoader* _modLoader;
    bool _enabled;
    MOomc* _moomc;

    bool _showComponents;

};

class ModItemsLoader : public QThread
{
    Q_OBJECT
public:
    ModItemsLoader(ModItemsTree* modItemsTree,ModItem* parent,int depthMax = 1000,  QString direction ="", int curDepth = 0);

    ModItemsTree* _modItemsTree;
    ModItem *_parent;
    int _depthMax;
    QString _direction;
    int _curDepth;
    static int num;


    void run();
signals:
    void readFromOmc(ModItem*,int depthMax ,  QString direction, int curDepth );    //Read data and children with OMC calls
};


#endif
