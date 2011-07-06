#include "MilpParam.h"


MilpParam::MilpParam(QString name)
{
    _name = name;
}



MilpParam0D::MilpParam0D(QString name)
    :MilpParam(name)
{

}

void MilpParam0D::addItem(QString item)
{
    _items.push_back(item);
}

void MilpParam0D::addItems(QStringList items)
{
    _items.append(items);
}

QString MilpParam0D::toString()
{
    QString result =  "param " + _name + ":=" + _items.join(" ")+";\n";
    return result;
}


MilpParam1D::MilpParam1D(QString name)
    :MilpParam(name)
{


}

void MilpParam1D::addItem(QString key,QString item)
{
    _items.insert(key,item);
}

void MilpParam1D::addItems(QString key,QStringList items)
{
    for(int i=0;i<items.size();i++)
    {
        _items.insert(key,items.at(i));
    }
}

QString MilpParam1D::toString()
{
    QString result;
    QString key;
    QStringList values;

    result += "param " + _name;
    for(int i = 0; i<_items.keys().size();i++)
    {
        key = _items.keys().at(i);
        values.clear();
        values.append(_items.values(key));

        result += "[" +key+"] " + values.join(" ")+"\n";
    }
    result +=";";
    return result;
}


MilpParam2D::MilpParam2D(QString name)
    :MilpParam(name)
{
}

void MilpParam2D::addItem(QString indexA,QString indexB,QString item)
{
    MilpKey2D key(indexA,indexB);
    _items.insert(key,item);
}

void MilpParam2D::addItems(QString indexA,QString indexB,QStringList items)
{
    MilpKey2D key(indexA,indexB);

    for(int i=0;i<items.size();i++)
        _items.insert(key,items.at(i));

}


QString MilpParam2D::toString()
{
    QString result;
    MilpKey2D key;
    QStringList values;

    QList<MilpKey2D> keys = _items.uniqueKeys();

    result += "param " + _name;
    for(int i = 0; i<keys.size();i++)
    {
        key = keys.at(i);
        values.clear();
        values.append(_items.values(key));

        result += "[" +key.toString()+"] " + values.join(" ")+"\n";
    }
    result +=";";

    return result;
}


MilpParam3D::MilpParam3D(QString name)
    :MilpParam(name)
{
}

void MilpParam3D::addItem(QString indexA,QString indexB,QString indexC,QString item)
{
    MilpKey3D key(indexA,indexB,indexC);
    _items.insert(key,item);
}

void MilpParam3D::addItems(QString indexA,QString indexB,QString indexC,QStringList items)
{
    MilpKey3D key(indexA,indexB,indexC);

    for(int i=0;i<items.size();i++)
        _items.insert(key,items.at(i));

}


QString MilpParam3D::toString()
{
    QString result;
    MilpKey3D key;
    QStringList values;

    QList<MilpKey3D> keys = _items.uniqueKeys();

    result += "param " + _name;
    for(int i = 0; i<keys.size();i++)
    {
        key = keys.at(i);
        values.clear();
        values.append(_items.values(key));

        result += "[" +key.toString()+"] " + values.join(" ")+"\n";
    }
    result +=";";

    return result;
}



