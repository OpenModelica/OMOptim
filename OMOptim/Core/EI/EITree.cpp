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

        @file EITree.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#include "EITree.h"
#include "Project.h"

EITree::EITree(Project* project,bool showFields,bool editable)
    :_showFields(showFields),_editable(editable)
{
    _project = project;
    _rootElement = new EIItem(NULL,QString()); // _rootElement name should be empty for search functions
    _enabled = true;

}

EITree::EITree(const EITree &tree)
{
    _showFields = tree._showFields;
    _editable = tree._editable;
    _enabled = tree._enabled;
    _project = tree._project;

    _rootElement = tree._rootElement->clone();
}

EITree & EITree::operator=(const EITree &tree)
{
    _showFields = tree._showFields;
    _editable = tree._editable;
    _enabled = tree._enabled;
    _project = tree._project;

    _rootElement = tree._rootElement->clone();
}

EITree::EITree(Project* project,QDomElement & domEl)
 {
     _rootElement = new EIItem(domEl);
     _enabled = true;
    _project = project;
 }

EITree::~EITree(void)
{
    clear();
    delete _rootElement;
}

void EITree::clear()
{
    beginResetModel();
    _rootElement->clear();
    endResetModel();
}


bool EITree::addChild(EIItem* parent, EIItem* child, int i)
{
    QModelIndex parentIndex = indexOf(parent);

    int index;
    if(i>=0)
        index=i;
    else
        index = parent->childCount()+i+1;


    beginInsertRows(parentIndex,index,index);
    bool ok = parent->addChild(child,index);
    endInsertRows();
    emit dataChanged(indexOf(parent),indexOf(parent));
    return ok;
}

void EITree::addEmptyGroup(EIItem* parent)
{
    EIGroup* newGroup = new EIGroup();
    bool ok = addChild(parent,newGroup);
    if(!ok)
        delete newGroup;
}

void EITree::addEmptyStream(EIItem* parent)
{
    EIStream* newStream = new EIStream(parent);
    bool ok = addChild(parent,newStream);
    if(!ok)
        delete newStream;
}


int EITree::columnCount(const QModelIndex &parent) const
{
    return EIStream::nbFields;
}

QVariant EITree::data(const QModelIndex &index, int role) const
{
    if(_enabled)
    {
	QVariant result;

	if (!index.isValid())
            return QVariant();

	if(index.column()<0 || index.column()>=columnCount())
            return QVariant();

	EIItem *item = static_cast<EIItem*>(index.internalPointer());
	if(!item)
            return 0;
	
	if((item->getEIType()==EI::GROUP)
            &&(index.column()>0)
            &&(role==Qt::DisplayRole))
            return QVariant();

        if(role==Qt::TextColorRole)
        {
            if(item->getEIType()==EI::MODELCONTAINER)
                return QVariant();
            if(EIReader::isInFactVariable(item))
                return utilityTextColor();
            else
                return processTextColor();
        }


        switch(role)
	{
        case Qt::DisplayRole :
        case Qt::EditRole :
            result = item->getFieldValue(index.column(),role);
            return result;
            break;
        case Qt::ToolTipRole :
            return item->getStrToolTip();
            break;
	}
	if(isCheckable(index)&&(role==Qt::CheckStateRole))
	{
            if(item->isChecked())
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
        return QVariant();
    }
    else
        return QVariant();
}

bool EITree::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(_enabled)
    {
        if (!index.isValid())
            return false;

        if(index.column()<0 || index.column()>=columnCount())
            return false;

        int checkState;
        EIItem *item = static_cast<EIItem*>(index.internalPointer());
        if(item)
        {
            switch(role)
            {
            case Qt::CheckStateRole :
                checkState = value.toInt();
                if((checkState == Qt::Unchecked) || (checkState == Qt::PartiallyChecked))
                    item->setChecked(false);
                else
                    item->setChecked(true);

                dataChanged(index,index);
                return true;
                break;
            case Qt::EditRole :
                item->setFieldValue(index.column(),value);
                dataChanged(index,index);
                return true;
                break;
            default:
                return false;
            }
        }

        dataChanged(QModelIndex(),QModelIndex());
        return true;
    }
    else
        return false;
}

Qt::ItemFlags EITree::flags(const QModelIndex &index) const
{
    Qt::ItemFlags _flags = Qt::ItemIsDropEnabled ;
    if(!_enabled || !index.isValid())
        return _flags;



    EIItem* item = static_cast<EIItem*>(index.internalPointer());

    if(item==_rootElement)
        return  Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    bool ok;
    bool drop;
    switch(item->getEIType())
    {
    case EI::STREAM :
        ok=true;
        drop = false;
        break;
    case EI::GROUP :
    case EI::MODELCONTAINER :
        ok=(index.column()==0);
        drop = true;
        break;
    case EI::GENERIC:
        ok = false;
        drop = false;
    }


    if(ok)
    {
        _flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

        if(drop)
              _flags = _flags | Qt::ItemIsDropEnabled;

        if(_editable)
            _flags = _flags | Qt::ItemIsEditable;

        if(index.column()==0)
            _flags = _flags | Qt::ItemIsUserCheckable ;
    }
    return _flags;
}

Qt::DropActions EITree::supportedDropActions() const{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList EITree::mimeTypes () const
{
    QStringList types;
    types << "text/plain";
    return types;
}
QMimeData* EITree::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    // select only first column indexes (since selection is made by row)
    QModelIndexList rowIndexes;
    for(int i=0;i<indexes.size();i++)
    {
        if(indexes.at(i).column()==0)
            rowIndexes.push_back(indexes.at(i));
    }

    QString mimeText;
    for(int i=0;i<rowIndexes.size();i++)
    {
        EIItem* item = (EIItem*)rowIndexes.at(i).internalPointer();
        mimeText.push_back("EI::"+item->name(EI::FULL)+"\n");
    }
    mimeData->setText(mimeText);

    return mimeData;
}

bool EITree::dropMimeData(const QMimeData *data,
                          Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasText())
        return false;

    QString text = data->text();
    QStringList lines = text.split("\n");
    QString line;
    bool somethingDone = false;

    for(int i=0;i<lines.size();i++)
    {
        line = lines.at(i);


        if(line.indexOf("MODELICA::")==0)
        {
            // look for model
            ModModel* model = dynamic_cast<ModModel*>(_project->modClassTree()->findItem(line.remove(QRegExp("^MODELICA::"))));
            if(model)
            {
                // prepare long time loading
                QApplication::setOverrideCursor(Qt::BusyCursor);
                // load
                loadModel(model);
                somethingDone = true;
                // restore cursor
                QApplication::restoreOverrideCursor();
            }
        }

        if(line.indexOf("EI::")==0)
        {
            // look for item
            QString eiName = line.remove(QRegExp("^EI::"));
            EIItem* item = dynamic_cast<EIItem*>(this->findItem(eiName));

            if(item)
            {
                if(!parent.isValid())
                    return false;

                EIItem* parentItem = ((EIItem*)parent.internalPointer());


                addChild(parentItem,item->clone(),row);
                removeItem(item);
                somethingDone = true;
            }
        }
    }

    return somethingDone;
}

QVariant EITree::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if((orientation==Qt::Horizontal)&&(_showFields))
    {
        if (role == Qt::DisplayRole)
        {
            return EIStream::sFieldName(section,role);
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex EITree::indexOf(EIItem* item,int column) const
{
    if(item==_rootElement)
        return createIndex(0,column,item);
    else
        return createIndex(item->indexInParent(),column,item);
}

QModelIndex EITree::index(int row, int column, const QModelIndex &parent)
        const
{
    if(_enabled)
    {
        if(!hasIndex(row,column,parent))
            return QModelIndex();

        EIItem *parentComponent;

        if (!parent.isValid())
            return indexOf(_rootElement,column);
        else
            parentComponent = static_cast<EIItem*>(parent.internalPointer());

        // looking in children
        int nbChildren = parentComponent->childCount();

        if((row<0) || (row>= nbChildren))
            return QModelIndex();

        EIItem* childElement = parentComponent->child(row);
        return createIndex(row, column, childElement);
    }
    else
        return QModelIndex();
}

QModelIndex EITree::parent(const QModelIndex &index) const
{
    if(_enabled)
    {
        if (!index.isValid())
            return QModelIndex();

        EIItem *childElement = static_cast<EIItem*>(index.internalPointer());

        if(childElement==_rootElement)
            return QModelIndex();

        EIItem *parentElement  = childElement->parent();
        return indexOf(parentElement);

    }
    else
        return QModelIndex();
}

int EITree::rowCount(const QModelIndex &parent) const
{
    if(_enabled)
    {
	EIItem *parentElement;

	if (!parent.isValid())
            return 1; // row corresponding to root element
	else
        {
            parentElement =static_cast<EIItem*>(parent.internalPointer());
            return parentElement->childCount();
        }
    }
    else
        return 0;
}

EIItem* EITree::findItem(QString _fullName)const
{
    return EIReader::findInDescendants(_rootElement,_fullName);
}

EIItem* EITree::findItem(EI::Type eiType,QVariant itemFieldValue, int iField)const
{
    return EIReader::findInDescendants(_rootElement,eiType,itemFieldValue,iField);
}




bool EITree::isCheckable(const QModelIndex _index) const
{
    return _index.column()==0;
}

bool EITree::removeItem(EIItem* item)
{
    EIItem* parent = item->parent();
    return removeChild(parent,item->indexInParent());
}

bool EITree::removeItem(const QModelIndex &index)
{
    if(!index.isValid())
        return false;

    EIItem* eiItem = static_cast<EIItem*>(index.internalPointer());
    EIItem* eiParent = eiItem->parent();

    return removeChild(eiParent,eiItem->indexInParent());
}

bool EITree::removeChild(EIItem* parent, int iChild)
{
    //create index
    QModelIndex parentIndex = indexOf(parent);
    return removeRow(iChild,parentIndex);
}


bool EITree::removeChildren(EIItem* parent)
{
    //create index
    QModelIndex parentIndex = indexOf(parent);
    int nbChildren = rowCount(parentIndex);
    return removeRows(0,nbChildren,parentIndex);
}

void EITree::removeUnchecked()
{
    beginResetModel();
    _rootElement->removeUncheckedDescendants();
    endResetModel();
}

 void EITree::uncheckUtilities()
 {
     QList<EIItem*> groups = EIReader::getItems(_rootElement,true,EI::GROUP);
     EIGroup* curGroup;
     for(int i=0;i<groups.size();i++)
     {
         curGroup = dynamic_cast<EIGroup*>(groups.at(i));
         if(curGroup && curGroup->isFactVariable())
             curGroup->setChecked(false);
     }
 }

bool EITree::removeRows ( int row, int count, const QModelIndex & parent )
{
    bool ok = true;
    if(!parent.isValid())
        return false;

    EIItem* parentItem = static_cast<EIItem*>(parent.internalPointer());
    int nbRemoved=0;

    beginRemoveRows(parent,row,row+count-1);
    while(nbRemoved<count)
    {
        ok = ok && parentItem->removeChild(row);
        nbRemoved++;
    }
    endRemoveRows();
    return ok;
}

EIItem* EITree::rootElement() const
{
    return _rootElement;
}




bool EITree::isValid(QString &msg,MOOptVector* variables,const QModelIndex index,bool recursive)
{
    EIItem* parent;
    bool ok = true;
    if(index.isValid())
        parent=static_cast<EIItem*>(index.internalPointer());
    else
        parent = _rootElement;

    ok = parent->isValid(variables,msg) && ok; // force verification even if already got an error (instead of ok && ...)

    if(recursive)
    {
        for(int iC=0;iC<parent->childCount();iC++)
            ok = parent->isValid(variables,msg) && ok;
    }
    return ok;
}

QDomElement EITree::toXmlData(QDomDocument & doc)
{
    return _rootElement->toXmlData(doc);
}




void EITree::loadModel(ModModel* loadedModel)
{
    bool eraseExisting=true;
    // be sure to read before loading
    _project->modClassTree()->readFromOmcV2(loadedModel);

    // extract ei information
    EIItem* modelRootEI = EIModelicaExtractor::extractFromModClass(loadedModel,_project->modClassTree(),_project->moomc());

    bool unloadOk=true;
    if(eraseExisting)
        unloadModel(loadedModel,unloadOk);

    if(unloadOk)
        this->addChild(rootElement(),modelRootEI);
}

void EITree::unloadModel(ModModel* unloadedModel, bool &ok)
{
    EIItem*  eiItem = this->findItem(EI::MODELCONTAINER,unloadedModel->name(),EIModelContainer::MODEL);

    ok= true;
    if(eiItem)
    {
        ok = this->removeItem(eiItem);
    }
}
