#include "EIConnConstr.h"

EIConnConstr::EIConnConstr(EITree* _eiTree)
{
        eiTree = _eiTree;
	a=NULL;
	b=NULL;
	type = FORBIDDEN;

	_editableFields << ITEMA << ITEMB << TYPE;
}

EIConnConstr::~EIConnConstr(void)
{
}

EIConnConstr::EIConnConstr(QDomElement & domEl, EITree* _eiTree)
{
        eiTree = _eiTree;

	QDomNamedNodeMap attributes = domEl.attributes();
	QString fieldName;
	QString fieldValue;

	for(int i=0;i<attributes.count();i++)
	{
		fieldName = attributes.item(i).toAttr().name();
		fieldName.replace(XMLTools::space()," ");
		fieldValue = attributes.item(i).toAttr().value();
		fieldValue.replace(XMLTools::space()," ");

		MOItem::setFieldValue(fieldName,QVariant(fieldValue));
	}
}

void EIConnConstr::setItems(EIItem* _a,EIItem* _b)
{
	a = _a;
	b = _b;
}

void EIConnConstr::setType(Type _type)
{
	type = _type;
}

QVariant EIConnConstr::getFieldValue(int iField, int role ) const
{
	switch(iField)
	{
	case ITEMA :
		if(a)
			return a->name();
		break;
	case ITEMB :
		if(b)
			return b->name();
		break;
	case TYPE :
		switch(role)
		{
		case Qt::UserRole :
			return type;
		case Qt::DisplayRole :
			return strType(type);			
		}
		break;
	}

	return QVariant();
}

QString EIConnConstr::sFieldName(int field, int role)
{
	switch(field)
	{
	case ITEMA :
		return "FirstItem";
	case ITEMB :
		return "SecondItem";
	case TYPE :
		return "Type";
	}
	return QString();
}

bool EIConnConstr::setFieldValue(int field,QVariant value)
{
	switch(field)
	{
	case ITEMA :
                a = eiTree->findItem(value.toString());
		return a;
	case ITEMB :
                b = eiTree->findItem(value.toString());
		return b;
	case TYPE :
		type = (Type)value.toInt();
		return true;
	}
	return false;
}

EIConnConstr* EIConnConstr::clone()
{
        EIConnConstr* cloned = new EIConnConstr(eiTree);
	cloned->a = a;
	cloned->b = b;
	cloned->type = type;

	return cloned;
}

bool EIConnConstr::isValid()
{
	return a && b;
}


QMultiMap<EIStream*,EIStream*> EIConnConstr::getMapStreams(MOOptVector *variables)
{
	QList<EIStream*> aStreams;
	QList<EIStream*> bStreams;
	if(a->getEIType()==EI::STREAM)
		aStreams.push_back((EIStream*)a);
	else
		aStreams.append(EIReader::getStreams(a));

	if(b->getEIType()==EI::STREAM)
		bStreams.push_back((EIStream*)b);
	else
		bStreams.append(EIReader::getStreams(b));

	QMultiMap<EIStream*,EIStream*> result;

	EIStream* curAStream;
	EIStream* curBStream;
	for(int ia=0;ia<aStreams.size();ia++)
	{
		curAStream = aStreams.at(ia);
		for(int ib=0;ib<bStreams.size();ib++)
		{
			curBStream = bStreams.at(ib);
			
			if(curAStream->isHot(variables)&&!curBStream->isHot(variables))
				result.insert(curAStream,curBStream);
			if(!curAStream->isHot(variables)&&curBStream->isHot(variables))
				result.insert(curBStream,curAStream);
		}
	}
	return result;
}

QString EIConnConstr::strType(Type _type)
{
	switch(_type)
	{
	case FORBIDDEN :
		return "Forbidden";
	default :
		return "-";
	}
}
