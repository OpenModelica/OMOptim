// $Id: MOVector.cpp 12125 2012-06-19 16:54:53Z hubert.thieriot $
#include "MOVector.h"

template<class ItemClass>
MOVector<ItemClass>::MOVector(bool owner) : QAbstractTableModel()
{
    _owner = owner;
}



template<class ItemClass>
MOVector<ItemClass>::MOVector(const MOVector & copied)
{
    *this = copied;
}

template<class ItemClass>
MOVector<ItemClass>& MOVector<ItemClass>::operator=(const MOVector<ItemClass> &copied)
{
    int iv;
    for(iv=0;iv<copied.items.size();iv++)
    {
        //addItem(new ItemClass(*test_.items.at(iv)));
        addItem(copied.items.at(iv)->clone());
    }

    _owner = true; // indeed, should be true every time !!!

    return *this;
}

template<class ItemClass>
MOVector<ItemClass>::MOVector(QString savedData, bool owner)
{

    _owner = owner;

    QStringList lineList = savedData.split("\n");
    QStringList fields;


    for(int nl=0;nl<lineList.size();nl++)
    {
        ///QString test("#");

        if(!lineList[nl].isEmpty() && (QString(lineList[nl].at(0)).compare(QString("#")))!=0)
        {
            items.push_back(new ItemClass(lineList[nl]));
        }
    }
}


template<class ItemClass>
MOVector<ItemClass>::MOVector(QDomElement & domList, bool owner)
{
    _owner = owner;
    setItems(domList);
    connect(this,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SIGNAL(modified()));
}



template<class ItemClass>
MOVector<ItemClass>::~MOVector() 
{
    if(_owner)
        //delete contents
        clear();
}

template<class ItemClass>
void MOVector<ItemClass>::update(const QDomElement & domList)
{
    QDomNode n = domList.firstChild();
    while( !n.isNull() )
    {
        QDomElement domItem = n.toElement();
        QDomNamedNodeMap attributes = domItem.attributes();
        QDomNode nameNode = attributes.namedItem("Name");
        QString name = nameNode.toAttr().value().replace(XMLTools::space()," ");

        int index=-1;
        if(!nameNode.isNull())
        {
            index = findItem(name);
        }
        if(index>-1)
            items.at(index)->update(domItem);
        else
            addItem(new ItemClass(domItem));

        n = n.nextSibling();
    }
}

template<class ItemClass>
QStringList MOVector<ItemClass>::getItemNames()
{
    QStringList _names;
    for(int i=0;i<items.size();i++)
        _names.push_back(items.at(i)->name());
    return _names;
}


template<class ItemClass>
void MOVector<ItemClass>::setItems(QDomElement & domList)
{
    clear();

    QDomNode n = domList.firstChild();
    while( !n.isNull() )
    {
        QDomElement domItem = n.toElement();
        ItemClass* newItem;
        if( !domItem.isNull() )
        {
            newItem = new ItemClass(domItem);
            addItem(newItem);
        }
        n = n.nextSibling();
    }
}


template<class ItemClass>
int MOVector<ItemClass>::rowCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return items.size();
}

template<class ItemClass>
int MOVector<ItemClass>::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ItemClass::nbFields;
}

template<class ItemClass>
QVariant MOVector<ItemClass>::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            return ItemClass::sFieldName(section,role);
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

template<class ItemClass>
QVariant MOVector<ItemClass>::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid()) return QVariant();

    ItemClass* curItem;
    if(index.row()<items.size() && index.column()<ItemClass::nbFields)
    {
        curItem = items.at(index.row());
    }
    else
        return QVariant();
    switch(role)
    {
    case Qt::DisplayRole :
    case Qt::EditRole :
        result = items.at(index.row())->getFieldValue(index.column(),role);
        return result;
        break;
    case Qt::ToolTipRole :
        return curItem->getStrToolTip();
        break;
    }
    return QVariant();
}

template<class ItemClass>
Qt::ItemFlags MOVector<ItemClass>::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsDropEnabled ;

    if(!index.isValid())
        return flags;

    if(items.at(index.row())->isEditableField(index.column()))
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

template<class ItemClass>
bool MOVector<ItemClass>::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return items[index.row()]->setFieldValue(index.column(),value);
}

template<class ItemClass>
void MOVector<ItemClass>::addItem(ItemClass* item_)
{
    // Add an item pointer in Vector
    int index=items.size();
    insertRow(index);//,createIndex(0,0));
    beginInsertRows(QModelIndex(),index,index);
    items.push_back(item_);
    endInsertRows();
}

template<class ItemClass>
void MOVector<ItemClass>::insertItem(ItemClass* item,int index)
{
    // Add an item pointer in Vector
    qDebug(QString("Insert item at range :" + QString::number(index)).toLatin1().data());
    if(index>-1)
    {
        beginInsertRows(QModelIndex(),index,index);
        items.insert(index,item);
        endInsertRows();
    }
}

template<class ItemClass>
bool MOVector<ItemClass>::removeRow(int index,const QModelIndex &parent)
{
    if(index<0)
    {
        qDebug(QString("!! Tried to remove item at index "+QString::number(index)).toLatin1().data());
        return false;
    }

    return removeRows(index,1,parent);
}


/**
  * This is the function that will be in fine called everytime a removeRow function is called.
  * It is the only one which contains beginRemoveRows and endRemoveRows calls, as data deleting.
  */
template<class ItemClass>
bool MOVector<ItemClass>::removeRows(int index, int count, const QModelIndex &parent)
{

    if(index<0)
    {
       qDebug(QString("!! Tried to remove item at index "+QString::number(index)).toLatin1().data());
        return false;
    }

    if(items.size()>=index+count)
    {
        beginRemoveRows(QModelIndex(),index,index+count-1);
        for(int i=0;i<count;i++)
        {
            if(_owner)
                delete items.at(index);
            items.erase(items.begin()+index);
        }
        endRemoveRows();
        return true;
    }
    else
    {
        return false;
    }
}


template<class ItemClass>
bool MOVector<ItemClass>::removeRows(QList<int> indexes)
{
    if(indexes.size())
    {
        qSort(indexes.begin(),indexes.end());

        // start by the end in order not to be affected
        for(int i=indexes.size()-1;i>=0;i--)
        {
            removeRow(indexes.at(i));
        }
        return true;
    }
    return false;
}


template<class ItemClass>
void MOVector<ItemClass>::removeRow(QString itemName)
{
    int index = findItem(itemName);
    if (index!=-1)
    {
        removeRow(index);
    }
}



template<class ItemClass>
int MOVector<ItemClass>::findItem(QString itemName, Qt::CaseSensitivity caseSens) const
{
    bool found = false;
    int i=0;
    int nbItems=items.size();
    QString itemName2;

    while((!found)&&(i<nbItems))
    {
        itemName2=items.at(i)->name();
        found=(itemName.compare(itemName2,caseSens)==0);
        i++;
    }
    if(!found)
    {
        return -1;
    }
    else
    {
        return i-1;
    }
}

template<class ItemClass>
int MOVector<ItemClass>::findItem(QVariant itemFieldValue, int iField) const
{
    bool found = false;
    int i=0;
    int nbItems=items.size();
    QVariant curFieldValue;

    while((!found)&&(i<nbItems))
    {
        curFieldValue=items.at(i)->getFieldValue(iField);
        found=(itemFieldValue == curFieldValue);
        i++;
    }
    if(!found)
    {
        return -1;
    }
    else
    {
        return i-1;
    }
}

template<class ItemClass>
bool MOVector<ItemClass>::contains(ItemClass* item)
{
    return items.contains(item);
}

template<class ItemClass>
bool MOVector<ItemClass>::alreadyIn(QString itemName)
{
    int i = findItem(itemName);
    if (i==-1)
        return false;
    else
        return true;
}


/** replace in this vector values which are in overVector
*     Illustration example : this = [A=2,B=3,C=4,D=5] and overVector = [C=99]
*    then this will be [A=2,B=3,C=99,D=5];
*     !! values are copied, not referenced in (*this) vector
*/

template<class ItemClass>
void MOVector<ItemClass>::replaceIn(MOVector<ItemClass> *overVector)
{
    int iv,iov;
    QString curName;

    for(iov=0;iov<overVector->size();iov++)
    {
        curName = overVector->at(iov)->name();
        iv=findItem(curName);

        if(iv!=-1)
        {
            if(_owner)
                delete items.at(iv);
            items.erase(items.begin()+iv,items.begin()+iv+1);
            items.insert(items.begin()+iv,overVector->at(iov));
        }
        else
        {
            QString msg;
            msg.sprintf("Variable %s not found in vector. Could not replace.",curName.utf16());
            InfoSender::instance()->debug(msg);
        }
    }
}

template<class ItemClass>
void MOVector<ItemClass>::addItems(MOVector<ItemClass> * newItems,bool makeACopy)
{
    int iov;
    QString curName;

    for(iov=0;iov<newItems->size();iov++)
    {
        curName = newItems->at(iov)->name();
        //        if(replaceIfPresent)
        //        {
        //            iv=findItem(curName);

        //            if(iv!=-1)
        //            {
        //                if(_owner)
        //                    delete items.at(iv);
        //                items.erase(items.begin()+iv,items.begin()+iv+1);
        //                items.insert(items.begin()+iv,newItems->at(iov));
        //            }
        //            else
        //            {
        //                 addItem(newItems->at(iov));
        //            }
        //        }
        //        else
        //        {
        if(makeACopy)
            addItem(newItems->at(iov)->clone());
        else
            addItem(newItems->at(iov));
        //        }
    }


}


template<class ItemClass>
void MOVector<ItemClass>::cloneFromOtherVector(const MOVector *vector_)
{
    clear();
    int i;
    ItemClass* newItem;
    for(i=0;i<vector_->size();i++)
    {
        newItem = vector_->at(i)->clone();
        addItem(newItem);
    }
}

template<class ItemClass>
MOVector<ItemClass>* MOVector<ItemClass>::clone() const
{
    MOVector<ItemClass>* newVector = new MOVector<ItemClass>(_owner);

    int i;
    ItemClass* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem =items.at(i)->clone();
        newVector->addItem(newItem);
    }


    return newVector;
}

template<class ItemClass>
void MOVector<ItemClass>::clear()
{
    this->beginResetModel();
    if(items.size()>0)
    {
        beginRemoveRows(QModelIndex(),0,items.size()-1);
        removeRows(0,items.size());
        endRemoveRows();
    }
    this->endResetModel();
}




template<class ItemClass>
QString MOVector<ItemClass>::toSavingString()
{
    QString saveString;

    // print field names
    saveString += "\n#";
    for(int iF=0; iF<ItemClass::nbFields; iF++)
        saveString += ItemClass::sFieldName(iF,Qt::UserRole) + "\t ";

    saveString += "\n";
    // print item values
    for(int i=0; i<items.size();i++)
    {
        saveString += items.at(i)->toSavingString();
        saveString += "\n";
    }
    return saveString;
}

template<class ItemClass>
QDomElement MOVector<ItemClass>::toXmlData(QDomDocument & doc,QString listTitle)
{
    // Root element
    QDomElement cList = doc.createElement(listTitle);

    for(int i=0;i<items.size();i++)
    {
        QDomElement cItem = items.at(i)->toXmlData(doc);
        cList.appendChild(cItem);
    }
    return cList;
}



template<class ItemClass>
QModelIndex MOVector<ItemClass>::index(int row, int column, const QModelIndex &parent)const
{
    if((row>-1)&&(row < items.size()))
        return createIndex(row,column,items.at(row));
    else
        return createIndex(row,column);
}

template<class ItemClass>
int MOVector<ItemClass>::size() const
{
    return items.size();
}

template<class ItemClass>
ItemClass* MOVector<ItemClass>::at(int i) const
{
    return items.at(i);
}

template<class ItemClass>
QStringList MOVector<ItemClass>::mimeTypes () const
{
    QStringList types;
    types << "text/plain";
    return types;
}

template<class ItemClass>
QMimeData* MOVector<ItemClass>::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();

    // create xml data
    QDomDocument doc;
    MOVector<ItemClass> dragVector(false);
    for(int i=0;i<indexes.size();i++)
    {
        ItemClass* item = (ItemClass*)indexes.at(i).internalPointer();
        if(!dragVector.contains(item))
            dragVector.addItem(item);
    }

    doc.appendChild(dragVector.toXmlData(doc,"list"));
    mimeData->setText("XML::"+doc.toString());

    return mimeData;
}

template<class ItemClass>
Qt::DropActions MOVector<ItemClass>::supportedDropActions() const{
    return  Qt::MoveAction | Qt::CopyAction;
}

template<class ItemClass>
bool MOVector<ItemClass>::dropMimeData(const QMimeData *data,
                                       Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasText())
        return false;

    QString text = data->text();

    bool somethingDone = false;

    if(text.indexOf("XML::")==0)
    {
        somethingDone = true;

        // look for item
        QString xmlContent = text.remove(QRegExp("^XML::"));
        qDebug(QString("Drop :"+xmlContent).toLatin1().data());
        QDomDocument doc;
        doc.setContent(xmlContent);

        // create items from xml
        QDomElement el = doc.firstChildElement("list");
        MOVector<ItemClass> dropedVector(el,false);
        qDebug(QString("droped vector size :"+QString::number(dropedVector.size())).toLatin1().data());
        for(int i=0;i<dropedVector.size();i++)
        {
            insertItem(dropedVector.at(i),row);
        }

    }

    return somethingDone;
}
