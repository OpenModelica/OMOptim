#include "MilpTools.h"


MilpKey2D::MilpKey2D(QString row ,QString col)
{
    _row = row;
    _col = col;
}


MilpKey2D MilpKey2D::operator=(const MilpKey2D& b)
{
    _row = b.row();
    _col = b.col();

    return *this;
}

bool MilpKey2D::operator==(const MilpKey2D & b) const
{
    return ((_row==b._row)&&(_col==b._col));
}

bool MilpKey2D::operator<(const MilpKey2D& b) const
{
if(_row==b._row)
    return (_col < b._col);
else
    return (_row < b._row);
}


QString MilpKey2D::toString()
{
    return _row+","+_col;
}


MilpKey3D::MilpKey3D(QString i1 ,QString i2,QString i3)
{
    _i1 = i1;
    _i2 = i2;
    _i3 = i3;
}

MilpKey3D MilpKey3D::operator=(const MilpKey3D& b)
{
    _i1 = b.i1();
    _i2 = b.i2();
    _i3 = b.i3();

    return *this;
}

bool MilpKey3D::operator==(const MilpKey3D & b) const
{
    return ((_i1==b._i1)&&(_i2==b._i2)&&(_i3==b._i3));
}

bool MilpKey3D::operator<(const MilpKey3D& b) const
{
if(_i1==b._i1)
    if(_i2==b._i2)
        return (_i3< b._i3);
    else
        return (_i2< b._i2);
else
    return (_i1 < b._i1);
}


QString MilpKey3D::toString()
{
    return _i1+","+_i2+","+_i3;
}


MilpKey4D::MilpKey4D(QString i1 ,QString i2,QString i3,QString i4)
{
    _i1 = i1;
    _i2 = i2;
    _i3 = i3;
    _i4 = i4;

}

MilpKey4D MilpKey4D::operator=(const MilpKey4D& b)
{
    _i1 = b.i1();
    _i2 = b.i2();
    _i3 = b.i3();
    _i4 = b.i4();

    return *this;
}

bool MilpKey4D::operator==(const MilpKey4D & b) const
{
    return ((_i1==b._i1)&&(_i2==b._i2)&&(_i3==b._i3)&&(_i4==b._i4));
}

bool MilpKey4D::operator<(const MilpKey4D& b) const
{
    if(_i1==b._i1)
        if(_i2==b._i2)
            if(_i3==b._i3)
                return (_i4 < b._i4);
            else
                return (_i3< b._i3);
        else
            return (_i2< b._i2);
    else
        return (_i1 < b._i1);
}


QString MilpKey4D::toString()
{
    return _i1+","+_i2+","+_i3+","+_i4;
}
