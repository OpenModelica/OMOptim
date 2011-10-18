#include "MyTreeView.h"


MyTreeView::MyTreeView(QWidget* parent)
    :QTreeView(parent)
{

}

MyTreeView::~MyTreeView(void)
{

}

int MyTreeView::getColHintSize(int iCol)
{
    return this->sizeHintForColumn(iCol);
}

void MyTreeView::onDataChanged(const QModelIndex & indexA,const QModelIndex & indexB)
{

    qDebug(QString("onDataChanged in treeview").toLatin1().data());
    int col1 = std::min(indexA.column(),indexB.column());
    int col2 = std::max(indexA.column(),indexB.column());

    for(int i=col1;i<=col2;i++)
        resizeColumnToContents(i);
}

void MyTreeView::resizeColumns()
{
    if(model())
    {
        for(int i=0;i<=model()->columnCount();i++)
            resizeColumnToContents(i);
    }
}
