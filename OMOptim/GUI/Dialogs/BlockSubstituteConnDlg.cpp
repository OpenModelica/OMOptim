// $Id$
/**
  @file BlockSubstituteConnDlg.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "BlockSubstituteConnDlg.h"
#include "ui_BlockSubstituteConnDlg.h"
#include <QtGui/QErrorMessage>
#include "ModModel.h"

#include <QtGui/QListWidget>


BlockSubstituteConnDlg::BlockSubstituteConnDlg( BlockSubstitution* _blockSub, bool _isEditable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlockSubstituteConnDlgClass)
{
    ui->setupUi(this);

    isEditable = _isEditable;
    orgBlockSub = _blockSub;
    tmpBlockSub = _blockSub->clone();



    //connect
    connect(ui->pushCancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->pushOk,SIGNAL(clicked()),this,SLOT(validate()));


    // combo
    initCombos();


    // set it uneditable if necessary
    if(!isEditable)
    {
        for(int i=0;i<portCombos.size();i++)
            for(int j=0;j<portCombos.at(i).size();j++)
                portCombos.at(i).at(j)->setEnabled(false);
    }
}

BlockSubstituteConnDlg::~BlockSubstituteConnDlg()
{
    delete ui;
}


void BlockSubstituteConnDlg::validate()
{
    bool isOk = true;



    for(int i=0;i<portCombos.size();i++)
    {
        QStringList connComps;
        for(int j=0;j<portCombos.at(i).size();j++)
        {
            connComps.append(portCombos.at(i).at(j)->itemData(portCombos.at(i).at(j)->currentIndex()).toString());
        }
        tmpBlockSub->_subConnectedComps.replace(i,connComps);
    }


    orgBlockSub->copyFrom(tmpBlockSub);
    accept();

    //// getting corresponding connections and selected component name
    //QModelIndex index = ui->treeBlocks->currentIndex();
    //if(isOk && index.isValid())
    //{
    //	ModClass *selectedComponent = static_cast<ModClass*>(index.internalPointer());
    //	QString subClassName = selectedComponent->name(Modelica::FULL);

    //	newBSub = new BlockSubstitution(project,project->getModel(),orgComponent,subClassName);
    //	newBSub->orgConnections = orgComponent->getOutsideConnectedComponentNames(true,false);

    //	QStringList scConnNames = selectedComponent->getOutsideConnectedComponentNames(true,false);
    //	QStringList corrNames;
    //	int i=0,curIndex;
    //	while(isOk && i<connectionCombos.size())
    //	{

    //		curIndex = connectionCombos.at(i)->currentIndex();
    //		if(curIndex>-1)
    //			corrNames.push_back(scConnNames.at(curIndex));
    //		else
    //			isOk = false;
    //		i++;
    //	}
    //	newBSub->subCorrConnections = corrNames;
    //}

    //if(isOk)
    //	accept();
    //else
    //{
    //	QErrorMessage *errorMessageDialog = new QErrorMessage(this);
    //	errorMessageDialog->showMessage("Problems in block replacement definition");
    //}
}

void BlockSubstituteConnDlg::initCombos()
{


    fillBasicItems();


    // create labels and layout
    for(int i=0; i < tmpBlockSub->_subPorts.size(); i++)
    {
        QList<QComboBox*> curPortCombos;
        QString portName = tmpBlockSub->_subPorts.at(i);
        QString portNameShort = portName.section(".",-1,-1);
        QLabel* portLabel = new QLabel(portNameShort,ui->groupConnections);
        portLabel->setObjectName(portName);
        ui->connLayout->addWidget(portLabel,i*2,0);
        portLabels.append(portLabel);

        // add button
        if(isEditable)
        {
            QIcon icon1;
            icon1.addFile(QString::fromUtf8(":/icons/Add"), QSize(30,30), QIcon::Normal, QIcon::Off);
            QPushButton* newPush = new QPushButton(icon1,"",ui->groupConnections);
            newPush->setObjectName(portName+"push");
            newPush->setMaximumSize(QSize(30,30));
            ui->connLayout->addWidget(newPush,i*2,1);
            connect(newPush,SIGNAL(clicked()),this,SLOT(addPortCombo()));
        }

        QGridLayout* portLayout = new QGridLayout(ui->groupConnections);
        ui->connLayout->addLayout(portLayout,2*i+1,0);
        portLayout->setObjectName(portName+"layout");
        portLayouts.append(portLayout);


        // add combos in layout
        if(tmpBlockSub->_subConnectedComps.at(i).size()==0)
        {
            curPortCombos.append(addCombo(basicItems,basicItemsData,portName));
        }
        else
        {
            for(int j=0;j<tmpBlockSub->_subConnectedComps.at(i).size();j++)
            {
                QString connComp = tmpBlockSub->_subConnectedComps.at(i).at(j);
                if(!basicItemsData.contains(connComp))
                {
                    InfoSender::instance()->debug("ERROR in BlockSubstitutions initCombos()");
                }
                else
                {
                    curPortCombos.append(addCombo(basicItems,basicItemsData,portName,-1,connComp));
                }
            }
        }

        portCombos.append(curPortCombos);
    }



}



QComboBox* BlockSubstituteConnDlg::addCombo(QStringList items, QStringList itemsData,QString portName,int iPos,QString dataSel)
{


    int index = tmpBlockSub->_subPorts.indexOf(portName);
    QGridLayout* portLayout = portLayouts.at(index);
    if(iPos==-1)
        iPos=portLayout->rowCount()-1;



    // combobox
    QComboBox* newCB = new QComboBox(ui->groupConnections);
    newCB->setObjectName(portName+"##"+QString::number(iPos)+"##combo");
    portLayout->addWidget(newCB,2*iPos,0,1,2);


    //adding items in combo
    for(int i = 0; i < items.size(); i++)
    {
        newCB->addItem(items.at(i),itemsData.at(i));
    }

    // select specified
    newCB->setCurrentIndex(newCB->findData(QVariant(dataSel)));



    // spacer
    QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    portLayout->addItem(spacer,2*iPos+1,0,1,1);

    return newCB;
}

void BlockSubstituteConnDlg::addPortCombo()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    QString portName = button->objectName();
    portName.remove(portName.lastIndexOf("push"),QString("push").length());

    addCombo(basicItems,basicItemsData,portName);
}




void BlockSubstituteConnDlg::fillBasicItems()
{

    basicItems.clear();
    basicItemsData.clear();

    basicItems.append("-");
    basicItemsData.append("");

    // fill basic items from org
    for(int i=0;i<tmpBlockSub->_orgPorts.size();i++)
    {
        for(int j=0;j<tmpBlockSub->_orgConnectedComps.at(i).size();j++)
        {
            QString connComp =  tmpBlockSub->_orgConnectedComps.at(i).at(j);
            QString comboText = connComp;
            comboText += " (connected to "+tmpBlockSub->_orgPorts.at(i).section(".",-1,-1) +")";
            basicItems.push_back(comboText);
            basicItemsData.push_back(connComp);
        }
    }

    for(int i=0;i<tmpBlockSub->_subPorts.size();i++)
    {
        for(int j=0;j<tmpBlockSub->_subConnectedComps.at(i).size();j++)
        {
            QString connComp =  tmpBlockSub->_subConnectedComps.at(i).at(j);
            if(!basicItemsData.contains(connComp))
            {
                QString comboText = connComp;
                basicItems.push_back(comboText);
                basicItemsData.push_back(connComp);
            }
        }
    }
}

void BlockSubstituteConnDlg::joinAuto()
{

}
