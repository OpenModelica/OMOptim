#include "MilpSet.h"


MilpSet::MilpSet(QString name)
{
    _name = name;
}



MilpSet0D::MilpSet0D(QString name)
    :MilpSet(name)
{

}

void MilpSet0D::addItem(QString item)
{
    _items.push_back(item);
}

void MilpSet0D::addItems(QStringList items)
{
    _items.append(items);
}

QString MilpSet0D::toString()
{
    QString result =  "set " + _name + ":=" + _items.join(" ")+";\n";
    return result;
}

void MilpSet0D::setItems(QStringList items)
{
    _items = items;
}

QStringList MilpSet0D::items()
{
    return _items;
}


MilpSet1D::MilpSet1D(QString name)
    :MilpSet(name)
{


}

void MilpSet1D::addItem(QString key,QString item)
{
    _items.insert(key,item);
}

void MilpSet1D::addItems(QString key,QStringList items)
{
    for(int i=0;i<items.size();i++)
    {
        _items.insert(key,items.at(i));
    }
}

QString MilpSet1D::toString()
{
    QString result;
    QString key;
    QStringList values;
    for(int i = 0; i<_items.uniqueKeys().size();i++)
    {
        key = _items.uniqueKeys().at(i);
        values.clear();
        values.append(_items.values(key));
        result += "set " + _name + "[" +key+"] :=" + values.join(" ")+";\n";
    }
    return result;
}

void MilpSet1D::setItems( QMultiMap<QString,QString> items)
{
    _items = items;
}

QMultiMap<QString,QString> MilpSet1D::items()
{
    return _items;
}


MilpSet2D::MilpSet2D(QString name)
    :MilpSet(name)
{
}

void MilpSet2D::addItem(QString indexA,QString indexB,QString item)
{
    MilpKey2D key(indexA,indexB);
    _items.insert(key,item);
}

void MilpSet2D::addItems(QString indexA,QString indexB,QStringList items)
{
    MilpKey2D key(indexA,indexB);

    for(int i=0;i<items.size();i++)
        _items.insert(key,items.at(i));

}

QString MilpSet2D::toString()
{
    QString result;
    MilpKey2D key;
    QStringList values;
    for(int i = 0; i<_items.uniqueKeys().size();i++)
    {
        key = _items.uniqueKeys().at(i);
        values.clear();
        values.append(_items.values(key));
        result += "set " + _name + "[" +key.toString()+"] :=" + values.join(" ")+";\n";
    }
    return result;
}

void MilpSet2D::setItems( QMultiMap<MilpKey2D,QString> items)
{
    _items = items;
}

QMultiMap<MilpKey2D,QString> MilpSet2D::items()
{
    return _items;
}

