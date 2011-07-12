#include "MilpVariableResult.h"


MilpVariableResult::MilpVariableResult(QString name)
{
    _name = name;
}

QStringList MilpVariableResult::getColNames(glp_prob *glpProblem)
{
    QStringList colNames;

    // read all column names
    int nbCols = glp_get_num_cols(glpProblem);
    for(int iCol = 1;iCol<=nbCols; iCol++)
    {
        colNames.push_back(QString(glp_get_col_name(glpProblem, iCol)));
    }
    return colNames;
}


MilpVariableResult0D::MilpVariableResult0D(QString name)
    :MilpVariableResult(name)
{

}

void MilpVariableResult0D::fill(glp_prob * glpProblem,double defaultValue,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    // read value
    int iCol=colNames.indexOf(_name,0);
    if(iCol>-1)
        _value = glp_mip_col_val(glpProblem,iCol+1); //iCol+1 since glp cols start at 1.
    else
        _value = defaultValue;
}



MilpVariableResult1D::MilpVariableResult1D(QString name)
    :MilpVariableResult(name)
{


}

void MilpVariableResult1D::fill(glp_prob * glpProblem,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    QRegExp regExp(_name + "\\[([\\w|\.|\\d]+)\\][.]*");


    double curValue;
    QString curName;

    int iCol=colNames.indexOf(regExp,0);
    while(iCol>-1)
    {
        curName = colNames.at(iCol);
        curName.remove(_name);
        curName.remove("[");
        curName.remove("\'");
        curName.remove("]");
        curName.remove(QRegExp("^\\."));
        curValue = glp_mip_col_val(glpProblem, iCol+1); //iCol+1 since glp cols start at 1.
        _values.insert(curName,curValue);
        iCol = colNames.indexOf(regExp,iCol+1);
    }
}


MilpVariableResult2D::MilpVariableResult2D(QString name)
    :MilpVariableResult(name)
{
}

void MilpVariableResult2D::fill(glp_prob * glpProblem,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    // read Qijk
    QRegExp regExp(_name+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][.]*");

    QString i1,i2;
    int iCol=colNames.indexOf(regExp,0);
    QString curColName;
    double curValue;
    while(iCol>-1)
    {
        curColName = colNames.at(iCol);
        if ((regExp.indexIn(curColName)>-1)&&(regExp.captureCount()==2))
        {
            i1 = regExp.cap(1);
            i2 = regExp.cap(2);

            curValue = glp_mip_col_val(glpProblem,iCol+1); //iCol+1 since glp cols start at 1

            _values.insert(MilpKey2D(i1,i2),curValue);

        }
        iCol = colNames.indexOf(regExp,iCol+1);
    }

}

MilpVariableResult3D::MilpVariableResult3D(QString name)
    :MilpVariableResult(name)
{
}

void MilpVariableResult3D::fill(glp_prob * glpProblem,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    // read Qijk
    QRegExp regExp(_name+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][.]*");

    QString i1,i2,i3;
    int iCol=colNames.indexOf(regExp,0);
    QString curColName;
    double curValue;
    while(iCol>-1)
    {
        curColName = colNames.at(iCol);
        if ((regExp.indexIn(curColName)>-1)&&(regExp.captureCount()==3))
        {
            i1 = regExp.cap(1);
            i2 = regExp.cap(2);
            i3 = regExp.cap(3);

            curValue = glp_mip_col_val(glpProblem,iCol+1); //iCol+1 since glp cols start at 1

            _values.insert(MilpKey3D(i1,i2,i3),curValue);

        }
        iCol = colNames.indexOf(regExp,iCol+1);
    }
}

MilpVariableResult4D::MilpVariableResult4D(QString name)
    :MilpVariableResult(name)
{
}

void MilpVariableResult4D::fill(glp_prob * glpProblem,QStringList colNames)
{
    if(colNames.isEmpty())
        colNames = getColNames(glpProblem);

    // read Qijk
    QRegExp regExp(_name+"\\[([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+),([\\w|\.|\\d]+)\\][.]*");

    QString i1,i2,i3,i4;
    int iCol=colNames.indexOf(regExp,0);
    QString curColName;
    double curValue;
    while(iCol>-1)
    {
        curColName = colNames.at(iCol);
        if ((regExp.indexIn(curColName)>-1)&&(regExp.captureCount()==4))
        {
            i1 = regExp.cap(1);
            i2 = regExp.cap(2);
            i3 = regExp.cap(3);
            i4 = regExp.cap(4);

            curValue = glp_mip_col_val(glpProblem,iCol+1); //iCol+1 since glp cols start at 1

            //_values.insert(MilpKey4D(i1,i2,i3,i4),1.1);
            _values.insert(MilpKey4D(i1,i2,i3,i4),curValue);
            qDebug(QString::number(_values.value(MilpKey4D(i1,i2,i3,i4))).toLatin1().data());

        }
        iCol = colNames.indexOf(regExp,iCol+1);
    }
}


QString MilpVariableResult4D::toString()
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


