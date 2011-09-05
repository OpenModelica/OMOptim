#include "WidgetEIHENView.h"
#include "ui_WidgetEIHENView.h"
#include <QtGui/QErrorMessage>

WidgetEIHENView::WidgetEIHENView(const EIHEN & eiHEN,QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::WidgetEIHENViewClass)
{
    _ui->setupUi(this);

    _eiHEN = eiHEN;
    _eiHENView = new EIHENView(this);
    _eiHENScene =  new EIHENScene(_eiHEN,this);

    _eiHENView->setScene(_eiHENScene);
    _eiHENView->setSceneRect(QRectF());
    _eiHENView->setDragMode(QGraphicsView::ScrollHandDrag);
    _eiHENView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    _eiHENView->setMinimumSize(QSize());


    _ui->layoutTable->addWidget(_eiHENView);

    connect(_ui->pushConsistency,SIGNAL(clicked()),this,SLOT(onSpecifyAsked()));
}

WidgetEIHENView::~WidgetEIHENView()
{
    delete _ui;
}

void WidgetEIHENView::onSpecifyAsked()
{
    QString msg;
    bool ok = _eiHEN.isValid(msg);

    QMessageBox *msgBox;

    if(ok)
        msgBox = new QMessageBox(QMessageBox::Information,"HEN coherency","HEN respects mass and heat balances");
    else
        msgBox = new QMessageBox(QMessageBox::Critical,"HEN coherency","HEN violates mass and heat balances :\n" + msg);

    msgBox->exec();
    delete msgBox;
}
