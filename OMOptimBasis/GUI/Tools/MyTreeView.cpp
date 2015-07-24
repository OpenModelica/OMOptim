
#include "MyTreeView.h"
#include "OMCases.h"
#include "ProjectBase.h"


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

OMCasesTreeView::OMCasesTreeView(ProjectBase* project,OMCasesCombiner* model,QWidget* mainwindow)
    :QTreeView(mainwindow)
{
    this->setFrameShape(QFrame::Box);
    this->setFrameShadow(QFrame::Raised);
    _project = project;
    _mainWindow = mainwindow;
    _model = model;



    this->setModel(_model);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setHeaderHidden(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    this->header()->setSectionResizeMode(QHeaderView::Stretch);
#else
    this->header()->setResizeMode(QHeaderView::Stretch);
#endif
    this->expandAll();
    connect (this,SIGNAL(customContextMenuRequested(const QPoint &)),
             this,SLOT(onRightClicked(const QPoint &)));


}

OMCasesTreeView::~OMCasesTreeView()
{
}

void OMCasesTreeView::onRightClicked(const QPoint &point)
{

    //Popup on cases tree
    QModelIndex  index = this->indexAt(point);
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    OMCase* selectedCase = _model->item(index);

    if ( !index.isValid() == -1 )
    {
        // no item selected
    }
    else
    {
        if(indexes.size()<=1)
        {
            if(selectedCase)
            {
                QMenu* caseMenu = createOMCasePopupMenu(selectedCase);
                if(caseMenu)
                    caseMenu->exec(this->mapToGlobal(point));
            }
        }
        else
        {
            QMenu* caseMenu = createOMCasesPopupMenu();
            if(caseMenu)
                caseMenu->exec(this->mapToGlobal(point));
        }
    }
}


QMenu* OMCasesTreeView::createOMCasePopupMenu(OMCase* selectedCase)
{
    QMenu *menu = new QMenu();

    bool isProblem = (dynamic_cast<Problem*>(selectedCase)!=NULL);

    //Open folder
    //Open folder
    QAction *openFolderAct = new QAction("Open folder",menu);
    connect(openFolderAct,SIGNAL(triggered()),selectedCase,SLOT(openFolder()));
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/icons/folder")), QIcon::Normal, QIcon::Off);
    openFolderAct->setIcon(icon);
    //openFolderAct->setIconSize(QSize(20, 20));
    menu->addAction(openFolderAct);

    //Rename problem
    QAction *renameAct = new QAction("Rename case...",menu);
    connect(renameAct,SIGNAL(triggered()),this,SLOT(onRenameAsked()));
    menu->addAction(renameAct);

    //Remove problem
    QAction *removeAct = new QAction("Remove case",menu);
    connect(removeAct,SIGNAL(triggered()),this,SLOT(onRemoveAsked()));
    QIcon iconRem;
    iconRem.addPixmap(QPixmap(QString::fromUtf8(":/icons/Remove")), QIcon::Normal, QIcon::Off);
    removeAct->setIcon(iconRem);
    menu->addAction(removeAct);

    return menu;
}



QMenu* OMCasesTreeView::createOMCasesPopupMenu()
{
    QMenu *menu = new QMenu();


    //Remove problem
    QAction *removeAct = new QAction("Remove cases",menu);
    connect(removeAct,SIGNAL(triggered()),this,SLOT(onRemoveAsked()));
    QIcon iconRem;
    iconRem.addPixmap(QPixmap(QString::fromUtf8(":/icons/Remove")), QIcon::Normal, QIcon::Off);
    removeAct->setIcon(iconRem);
    menu->addAction(removeAct);

    return menu;
}

void OMCasesTreeView::onRemoveAsked()
{
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    QList<OMCase*> selectedCases;
    for(int i=0;i<indexes.size();i++)
        selectedCases.push_back(_model->item(indexes.at(i)));

    QString msg= "Are you sure you want to remove following cases ? \n";
    for(int i=0;i<selectedCases.size();i++)
    {
        if(selectedCases.at(i))
            msg.push_back(" - "+selectedCases.at(i)->name()+"\n");
    }
    QMessageBox msgBox;
    msgBox.setText("Removing problem cannot be undone.");
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if(msgBox.exec() == QMessageBox::Yes)
    {
        _project->removeCases(selectedCases);
    }
}

void OMCasesTreeView::onRenameAsked()
{
    QModelIndex index = currentIndex();
    if(index.isValid())
    {
        OMCase* curCase = static_cast<OMCase*>(index.internalPointer());
        if (curCase)
        {
            bool ok;
            QString newName = QInputDialog::getText(this, "Rename...",
                                                    "New name :", QLineEdit::Normal,curCase->name(),&ok);

            if (ok && !newName.isEmpty())
            {
                _project->renameCase(curCase,newName);
            }
        }
    }
}


