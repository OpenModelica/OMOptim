#include "MilpVariableResult.h"


MilpVariableResult::MilpVariableResult(QString name)
{
    _name = name;
}



MilpVariableResult0D::MilpVariableResult0D(QString name)
    :MilpVariableResult(name)
{

}




MilpVariableResult1D::MilpVariableResult1D(QString name)
    :MilpVariableResult(name)
{


}


MilpVariableResult2D::MilpVariableResult2D(QString name)
    :MilpVariableResult(name)
{
}


MilpVariableResult3D::MilpVariableResult3D(QString name)
    :MilpVariableResult(name)
{
}



MilpVariableResult4D::MilpVariableResult4D(QString name)
    :MilpVariableResult(name)
{
}



QString MilpVariableResult4D::toString() const
{

        QString result;
        MilpKey4D key;
        QList<double> values;
        for(int i = 0; i<_values.uniqueKeys().size();i++)
        {
            key = _values.uniqueKeys().at(i);
            values.clear();
            values.append(_values.values(key));
            result += _name + "[" +key.toString()+"] :=";
            for(int j=0;j<values.size();j++)
                result += QString::number(values.at(j))+" ";
            result+=";\n";
        }
        return result;
}


