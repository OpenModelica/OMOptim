// $Id: MOVector.cpp 12125 2012-06-19 16:54:53Z hubert.thieriot $
#include "MOVector.h"

template<class ItemClass>
MOAVector<ItemClass>::MOAVector(bool owner) : QAbstractTableModel()
{
    _owner = owner;
    _editable = true;
}

template<class ItemClass>
MOAVector<ItemClass>::MOAVector(const QList<ItemClass *> list, bool owner, bool makeACopy)
{
    _owner = owner;

    for(int i=0;i<list.size();i++)
        if(makeACopy)
            addItem(list.at(i)->clone());
        else
            addItem(list[i]);

    _editable = true;
}



template<class ItemClass>
MOAVector<ItemClass>::MOAVector(const MOAVector<ItemClass> & copied)
{
    *this = copied;
    _editable = copied._editable;
}

template<class ItemClass>
MOAVector<ItemClass>& MOAVector<ItemClass>::operator=(const MOAVector<ItemClass> &copied)
{
    int iv;
    for(iv=0;iv<copied._items.size();iv++)
    {
        //addItem(new ItemClass(*test_._items.at(iv)));
        addItem(copied._items.at(iv)->clone());
    }

    _owner = true; // indeed, should be true every time !!!
    _editable = copied._editable;

    return *this;
}

//template<class ItemClass>
//MOAVector<ItemClass>::MOAVector(QString savedData, bool owner)
//{

//    _owner = owner;

//    QStringList lineList = savedData.split("\n");
//    QStringList fields;


//    for(int nl=0;nl<lineList.size();nl++)
//    {
//        ///QString test("#");

//        if(!lineList[nl].isEmpty() && (QString(lineList[nl].at(0)).compare(QString("#")))!=0)
//        {
//            _items.push_back(new ItemClass(lineList[nl]));
//        }
//    }
//}





template<class ItemClass>
MOAVector<ItemClass>::~MOAVector()
{
    if(_owner)
        //delete contents
        clear();
}

template<class ItemClass>
void MOAVector<ItemClass>::setEditable(bool editable)
{
    _editable = editable;
}


template<class ItemClass>
void MOAVector<ItemClass>::setEditableFields(QList<int> indexes,bool editable)
{
    for(int i=0;i<_items.size();i++)
        for(int j=0;j<indexes.size();j++)
            _items[i]->setIsEditableField(indexes.at(j),editable);
}

template<class ItemClass>
QStringList MOAVector<ItemClass>::getItemNames()
{
    QStringList _names;
    for(int i=0;i<_items.size();i++)
        _names.push_back(_items.at(i)->name());
    return _names;
}




template<class ItemClass>
int MOAVector<ItemClass>::rowCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return _items.size();
}

template<class ItemClass>
int MOAVector<ItemClass>::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ItemClass::nbFields;
}

template<class ItemClass>
QVariant MOAVector<ItemClass>::headerData(int section, Qt::Orientation orientation, int role) const
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
QVariant MOAVector<ItemClass>::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (!index.isValid())
    {
        return QVariant();
    }

    ItemClass* curItem;
    if(index.row()<_items.size() && index.column()<ItemClass::nbFields)
    {
        curItem = _items.at(index.row());
    }
    else
    {
        return QVariant();
    }

    switch(role)
    {
    case Qt::DisplayRole :
    case Qt::EditRole :
        result = _items.at(index.row())->getFieldValue(index.column(),role);
        return result;
        break;
    case Qt::ToolTipRole :
        return curItem->getStrToolTip();
        break;
    }
    return QVariant();
}

template<class ItemClass>
Qt::ItemFlags MOAVector<ItemClass>::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsDropEnabled ;

    if(!index.isValid())
        return flags;

    bool itemIsEditable =  _editable && !(_items.at(index.row())->isProtectedField(index.column()));

    if(itemIsEditable)
        return Qt::ItemIsEnabled| Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

template<class ItemClass>
bool MOAVector<ItemClass>::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return _items[index.row()]->setFieldValue(index.column(),value);
}

template<class ItemClass>
void MOAVector<ItemClass>::addItem(ItemClass* item)
{
    // Add an item pointer in Vector
    int index=_items.size();
    insertRow(index);//,createIndex(0,0));
    beginInsertRows(QModelIndex(),index,index);
    addInItems(item);
    endInsertRows();
}

template<class ItemClass>
void MOAVector<ItemClass>::insertItem(ItemClass* item,int index)
{
    // Add an item pointer in Vector
    if(index>-1)
    {
        beginInsertRows(QModelIndex(),index,index);
        _items.insert(index,item);
        endInsertRows();
    }
}

template<class ItemClass>
bool MOAVector<ItemClass>::removeRow(int index,const QModelIndex &parent)
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
bool MOAVector<ItemClass>::removeRows(int index, int count, const QModelIndex &parent)
{

    if(index<0)
    {
        qDebug(QString("!! Tried to remove item at index "+QString::number(index)).toLatin1().data());
        return false;
    }

    if(count == 0)
    {
        qDebug(QString("!! Tried to remove 0 item in MOVector").toLatin1().data());
        return false;
    }

    if(_items.size()>=index+count)
    {
        beginRemoveRows(QModelIndex(),index,index+count-1);
        for(int i=0;i<count;i++)
        {
            if(_owner)
                delete _items.at(index);
            this->removeFromItems(index);
        }
        endRemoveRows();
      // emit layoutChanged();
        return true;
    }
    else
    {
        return false;
    }
}


template<class ItemClass>
bool MOAVector<ItemClass>::removeRows(QList<int> indexes)
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
void MOAVector<ItemClass>::removeRow(QString itemName)
{
    ItemClass* item = findItem(itemName);
    int index = indexOf(item);
    if (index!=-1)
    {
        removeRow(index);
    }
}



template<class ItemClass>
ItemClass* MOAVector<ItemClass>::findItem(QString itemName) const
{
    int i=0;
    int nbItems=_items.size();
    QString itemName2;

    while(i<nbItems)
    {
        itemName2=_items.at(i)->name();

        if(itemName.compare(itemName2)==0)
            return _items.at(i);

        i++;
    }

    return NULL;
}

template<class ItemClass>
ItemClass* MOAVector<ItemClass>::findItem(QVariant itemFieldValue, int iField) const
{
    int i = indexOf(itemFieldValue,iField);
    if(i==-1)
    {
        return NULL;
    }
    else
    {
        return this->at(i);
    }
}

template<class ItemClass>
int MOAVector<ItemClass>::indexOf(QVariant itemFieldValue, int iField) const
{
    bool found = false;
    int i=0;
    int nbItems=_items.size();
    QVariant curFieldValue;

    while((!found)&&(i<nbItems))
    {
        curFieldValue=_items.at(i)->getFieldValue(iField);
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
int MOAVector<ItemClass>::indexOf(ItemClass *item)
{
    return _items.indexOf(item);
}

template<class ItemClass>
bool MOAVector<ItemClass>::isEmpty() const
{
    return _items.isEmpty();
}


template<class ItemClass>
bool MOAVector<ItemClass>::contains(ItemClass* item)
{
    return _items.contains(item);
}

template<class ItemClass>
bool MOAVector<ItemClass>::alreadyIn(QString itemName)
{
    ItemClass* i = findItem(itemName);
    if (i)
        return true;
    else
        return false;
}


/** replace in this vector values which are in overVector
*     Illustration example : this = [A=2,B=3,C=4,D=5] and overVector = [C=99]
*    then this will be [A=2,B=3,C=99,D=5];
*     !! values are copied, not referenced in (*this) vector
*/

template<class ItemClass>
void MOAVector<ItemClass>::replaceIn(MOAVector<ItemClass> *overVector)
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
                delete _items.at(iv);
            _items.erase(_items.begin()+iv,_items.begin()+iv+1);
            _items.insert(_items.begin()+iv,overVector->at(iov));
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
void MOAVector<ItemClass>::replaceAt(int index, ItemClass *newItem)
{
    this->removeRow(index);
    this->insertItem(newItem,index);
}


template<class ItemClass>
void MOAVector<ItemClass>::addItems(MOAVector<ItemClass> * newItems,bool makeACopy)
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
        //                    delete _items.at(iv);
        //                _items.erase(_items.begin()+iv,_items.begin()+iv+1);
        //                _items.insert(_items.begin()+iv,newItems->at(iov));
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
void MOAVector<ItemClass>::cloneFromOtherVector(const MOAVector *vector_)
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
MOAVector<ItemClass>* MOAVector<ItemClass>::clone() const
{
    MOAVector<ItemClass>* newVector = new MOAVector<ItemClass>(_owner);

    int i;
    ItemClass* newItem;
    for(i=0;i<_items.size();i++)
    {
        newItem =_items.at(i)->clone();
        newVector->addItem(newItem);
    }
    return newVector;
}

template<class ItemClass>
void MOAVector<ItemClass>::clear()
{
    this->beginResetModel();
    if(_items.size()>0)
    {
        beginRemoveRows(QModelIndex(),0,_items.size()-1);
        removeRows(0,_items.size());
        endRemoveRows();
    }
    this->endResetModel();
}




template<class ItemClass>
QString MOAVector<ItemClass>::toSavingString()
{
    QString saveString;

    // print field names
    saveString += "\n#";
    for(int iF=0; iF<ItemClass::nbFields; iF++)
        saveString += ItemClass::sFieldName(iF,Qt::UserRole) + "\t ";

    saveString += "\n";
    // print item values
    for(int i=0; i<_items.size();i++)
    {
        saveString += _items.at(i)->toSavingString();
        saveString += "\n";
    }
    return saveString;
}

template<class ItemClass>
QDomElement MOAVector<ItemClass>::toXmlData(QDomDocument & doc,QString listTitle)
{
    // Root element
    QDomElement cList = doc.createElement(listTitle);

    for(int i=0;i<_items.size();i++)
    {
        QDomElement cItem = _items.at(i)->toXmlData(doc);
        cList.appendChild(cItem);
    }
    return cList;
}



template<class ItemClass>
QModelIndex MOAVector<ItemClass>::index(int row, int column, const QModelIndex &parent)const
{
    if(column>=this->columnCount(QModelIndex()))
        return QModelIndex();

    if((row>-1)&&(row < _items.size()))
        return createIndex(row,column,_items.at(row));
    else
        return QModelIndex();
}

template<class ItemClass>
int MOAVector<ItemClass>::size() const
{
    return _items.size();
}

template<class ItemClass>
ItemClass* MOAVector<ItemClass>::at(int i) const
{
    return _items.at(i);
}

template<class ItemClass>
void MOAVector<ItemClass>::addInItems(ItemClass * item)
{
    _items.push_back(item);
}

template<class ItemClass>
void MOAVector<ItemClass>::removeFromItems(int index)
{
   _items.removeAt(index);
}

template<class ItemClass>
QStringList MOAVector<ItemClass>::mimeTypes () const
{
    QStringList types;
    types << "text/plain";
    types << "text/XML";
    return types;
}

template<class ItemClass>
QMimeData* MOAVector<ItemClass>::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();

    // create xml and plain text data
    QString csv;
    QDomDocument doc;
    MOAVector<ItemClass> dragVector(false);
    for(int i=0;i<indexes.size();i++)
    {
        ItemClass* item = (ItemClass*)indexes.at(i).internalPointer();
        if(!dragVector.contains(item))
            dragVector.addItem(item);
    }

    for(int i=0;i<dragVector._items.size();i++)
    {
        csv.push_back(dragVector._items.at(i)->toCSV());
        csv.push_back("\n");
    }

    doc.appendChild(dragVector.toXmlData(doc,"list"));
    mimeData->setData("text/XML","XML::"+doc.toString().toAscii());
    mimeData->setText(csv);

    return mimeData;
}

template<class ItemClass>
Qt::DropActions MOAVector<ItemClass>::supportedDropActions() const{
    return  Qt::MoveAction | Qt::CopyAction;
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

        ItemClass* item;
        if(!nameNode.isNull())
        {
            item = this->findItem(name);
        }
        if(item)
            item->update(domItem);
        else
            this->addItem(new ItemClass(domItem));

        n = n.nextSibling();
    }
}


template<class ItemClass>
void MOVector<ItemClass>::setItems(QDomElement & domList)
{
    this->clear();

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
MOVector<ItemClass>::MOVector(bool owner):MOAVector<ItemClass>(owner)
{
}

template<class ItemClass>
MOVector<ItemClass>::MOVector(const QList<ItemClass *> list, bool owner, bool makeACopy)
:MOAVector<ItemClass>(list,owner,makeACopy)
{
}

template<class ItemClass>
MOVector<ItemClass>::MOVector(const MOAVector<ItemClass> & b)
:MOAVector<ItemClass>(b)
{
}

template<class ItemClass>
MOVector<ItemClass>::MOVector(QDomElement & domList, bool owner)
    :MOAVector<ItemClass>(owner)
{
    setItems(domList);
    connect(this,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SIGNAL(modified()));
}


template<class ItemClass>
bool MOVector<ItemClass>::dropMimeData(const QMimeData *data,
                                       Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasText())
        return false;

    QString text = data->data("text/XML");

    bool somethingDone = false;

    if(text.indexOf("XML::")==0)
    {
        somethingDone = true;

        // look for item
        QString xmlContent = text.remove(QRegExp("^XML::"));
        qDebug(QString("Drop :"+xmlContent).toLatin1().data());
        QDomDocument doc;
        doc.setContent(xmlContent);

        // create _items from xml
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

template<class ItemClass>
MOVector<ItemClass>* MOVector<ItemClass>::clone() const
{
    MOVector<ItemClass>* newVector = new MOVector<ItemClass>(this->_owner);

    int i;
    ItemClass* newItem;
    for(i=0;i<this->_items.size();i++)
    {
        newItem =this->_items.at(i)->clone();
        newVector->addItem(newItem);
    }
    return newVector;
}
