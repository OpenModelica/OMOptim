
#include "widgetloadexemodel.h"
#include "ui_widgetloadexemodel.h"

WidgetLoadExeModel::WidgetLoadExeModel(Project* project,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WidgetLoadExeModel)
{
    ui->setupUi(this);
    _project = project;

    connect(ui->pushOpenExe,SIGNAL(clicked()),this,SLOT(openExe()));
    connect(ui->pushOpenInput,SIGNAL(clicked()),this,SLOT(openInput()));
    connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(validate()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

WidgetLoadExeModel::~WidgetLoadExeModel()
{
    delete ui;
}

QString WidgetLoadExeModel::name()
{
    return ui->lineModelName->text();
}

QFileInfo WidgetLoadExeModel::exeFileInfo()
{
    return QFileInfo(ui->lineExe->text());
}

QFileInfo WidgetLoadExeModel::inputFileInfo()
{
    return QFileInfo(ui->lineInput->text());
}


void WidgetLoadExeModel::openExe()
{
    // get starting dir
    QDir startDir = QFileInfo(ui->lineExe->text()).absoluteDir();

    QString filename = QFileDialog::getOpenFileName(this,"Executable",startDir.absolutePath());
    if(!filename.isEmpty())
        ui->lineExe->setText(filename);
}

void WidgetLoadExeModel::openInput()
{
    // get starting dir
    QDir startDir;
    if(!ui->lineInput->text().isEmpty())
        startDir.setPath(QFileInfo(ui->lineInput->text()).absolutePath());
    else
        startDir.setPath(QFileInfo(ui->lineExe->text()).absolutePath());

    QString filename = QFileDialog::getOpenFileName(this,"Input file",startDir.absolutePath());
    if(!filename.isEmpty())
        ui->lineInput->setText(filename);
}

void WidgetLoadExeModel::validate()
{
    bool ok = true;
    QString msg;
    // check
    if(!exeFileInfo().exists())
    {
        ok = false;
        msg+= "Exe file does not exist. \n";
    }

    if(!inputFileInfo().exists())
    {
        ok = false;
        msg+= "Input file does not exist. \n";
    }

    if(name().isEmpty())
    {
        ok = false;
        msg+= "Model name is empty. \n";
    }
    if(_project->findModItem(name()))
    {
        ok = false;
        msg+= "A similar named model already exists in project. \n";
    }
    if(ok)
        accept();
    else
    {
        QMessageBox msgBox;
        msgBox.setText(msg);
        msgBox.exec();
    }
}

