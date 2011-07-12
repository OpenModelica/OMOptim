#include "EIConnConstrs.h"

EIConnConstrs::EIConnConstrs(void)
{
}

EIConnConstrs::~EIConnConstrs(void)
{
}

EIConnConstrs::EIConnConstrs(const EIConnConstrs &b)
{
    for(int i=0;i<b.items.size();i++)
        items.push_back(b.items.at(i)->clone());
}

EIConnConstrs* EIConnConstrs::clone()
{
        EIConnConstrs* cloned = new EIConnConstrs(*this);
	return cloned;
}


EIConnConstrs::EIConnConstrs(QDomElement & domList, EITree* eiTree)
{
    setItems(domList,eiTree);
}

void EIConnConstrs::setItems(QDomElement domList,EITree* eiTree)
{
	QDomNode n = domList.firstChild();
	while( !n.isNull() )
	{
		QDomElement domItem = n.toElement();
		QDomAttr domAttr;
		if( !domItem.isNull() )
		{
                        EIConnConstr* newItem = new EIConnConstr(domItem,eiTree);
			addItem(newItem);
		}
		n = n.nextSibling();
	}
}

/**
  * Extracts from all EIConnConstr a map of all streams forbidden connections.
  * Includes a flatening process : if a connection is forbidden between a group and a stream,
  * map will include all group children streams.
  * @sa EIConnConstr::getForbiddenMatchs
  */
QMultiMap<QString,QString> EIConnConstrs::getForbiddenMatchs(MOOptVector *variables)
{
        QMultiMap<QString,QString> result;
        QMultiMap<QString,QString> curMap;
	for(int iMap=0;iMap<items.size();iMap++)
	{
                curMap = items.at(iMap)->getForbiddenMatchs(variables);
		for(int i=0;i<curMap.keys().size();i++)
			for(int j=0;j<curMap.values(curMap.keys().at(i)).size();j++)
			result.insert(curMap.keys().at(i),curMap.values(curMap.keys().at(i)).at(j));
	}
	return result;
}

/**
  * Check if a connection between two streams (str1 and str2) is forbidden.
  * @arg map : map of forbidden connections. Obtained with getForbiddenMatchs.
  * Giving a non-empty map avoid calculation of new map. If map is empty, then map is
  * re-calculated with getForbiddenMatchs.
  */
bool EIConnConstrs::isForbiddenMatch(QString str1,QString str2,QMultiMap<QString,QString> map,MOOptVector* variables)
{
    if(map.isEmpty())
        map = getForbiddenMatchs(variables);

    bool isForb = map.values(str1).contains(str2) || map.values(str2).contains(str1) ;

    return isForb;
}
