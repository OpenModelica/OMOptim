#include "ModPlusCtrls.h"

#include "ModPlusOMCtrl.h"
#include "ModPlusDymolaCtrl.h"
#include "Project.h"

ModPlusCtrls::ModPlusCtrls(Project* project,ModModelPlus* modModelPlus)
{
    QString msg = "new ModPlusCtrls ["+QString::number((int)this,16)+"]";
    qDebug(msg.toLatin1().data());

    _project = project;
    _modModelPlus = modModelPlus;
    if(_modModelPlus)
    {
        for(int i=0;i<ModPlusCtrl::nbTypes;i++)
        {
            ModPlusCtrl* newCtrl;
            newCtrl = getNewCtrl((ModPlusCtrl::Type)i,project,modModelPlus);
            this->insert((ModPlusCtrl::Type)i,newCtrl);
        }

#if DEFAULTSIMULATOR==0
        setCurrentCtrlType(ModPlusCtrl::OPENMODELICA);
#else
        setCurrentCtrlType(ModPlusCtrl::DYMOLA);
#endif
    }
}

ModPlusCtrls::ModPlusCtrls(Project* project,ModModelPlus* modModelPlus,const QDomElement & cControlers)
{
    QString msg = "new ModPlusCtrls ["+QString::number((int)this,16)+"]";
    qDebug(msg.toLatin1().data());

    _project = project;
    _modModelPlus = modModelPlus;

    if(_modModelPlus)
    {
        // Create initial ones
        for(int i=0;i<ModPlusCtrl::nbTypes;i++)
        {
            ModPlusCtrl* newCtrl;
            newCtrl = getNewCtrl((ModPlusCtrl::Type)i,project,modModelPlus);
            this->insertCtrl((ModPlusCtrl::Type)i,newCtrl);
        }

#if DEFAULTSIMULATOR==0
        setCurrentCtrlType(ModPlusCtrl::OPENMODELICA);
#else
        setCurrentCtrlType(ModPlusCtrl::DYMOLA);
#endif

        if(!cControlers.isNull() && cControlers.tagName()==className())
        {
            // Controler type
            this->setCurrentCtrlType((ModPlusCtrl::Type)cControlers.attribute("curType","").toInt());

            // Update controler parameters
            QDomNodeList domControlerList = cControlers.elementsByTagName("Controler");

            QDomElement cParams;
            QDomElement cControler;
            ModPlusCtrl* curCtrl;
            ModPlusCtrl::Type curCtrlType;
            for(int iC=0;iC<domControlerList.size();iC++)
            {
                cControler = domControlerList.at(iC).toElement();
                curCtrlType = (ModPlusCtrl::Type)cControler.attribute("type","-1").toInt();
                curCtrl = this->value(curCtrlType);

                if(curCtrl)
                {
                    cParams = cControler.firstChildElement("parameters");
                    if(!cParams.isNull())
                    {
                        curCtrl->parameters()->update(cParams);
                    }
                }
            }
        }
    }
}

ModPlusCtrls::~ModPlusCtrls()
{
    for(int i=0;i<values().size();i++)
    {
        delete values().at(i);
    }
    QString msg = "delete ModPlusCtrls ["+QString::number((int)this,16)+"]";
    qDebug(msg.toLatin1().data());
}

ModPlusCtrls* ModPlusCtrls::clone()
{
    ModPlusCtrls* cloned = new ModPlusCtrls(_project,_modModelPlus);

    // first remove automatically created
    for(int i=0;i<cloned->values().size();i++)
    {
        delete cloned->values().at(i);
    }
    cloned->clear();

    // then add cloned
    for(int i=0;i<keys().size();i++)
    {
        cloned->insert(this->keys().at(i),this->value(keys().at(i))->clone());
    }

    cloned->_ctrlType = _ctrlType;

    return cloned;
}

ModPlusCtrl* ModPlusCtrls::getNewCtrl(ModPlusCtrl::Type type,Project* project,ModModelPlus* modModelPlus)
{
    switch(type)
    {
    case ModPlusCtrl::OPENMODELICA:
        return new ModPlusOMCtrl(project,modModelPlus,project->moomc());
    case ModPlusCtrl::DYMOLA:
        return new ModPlusDymolaCtrl(project,modModelPlus,project->moomc());
    default:
        return NULL;
    }
}

QDomElement ModPlusCtrls::toXmlData(QDomDocument & doc)
{
    QDomElement cControlers = doc.createElement(className());
    cControlers.setAttribute("curType",(int)currentCtrlType());

    QDomElement ccurCtrl;
    QDomElement ccurParams;
    ModPlusCtrl* curCtrl;

    for(int iCtrl=0;iCtrl<this->values().size();iCtrl++)
    {
        curCtrl = this->values().at(iCtrl);
        ccurCtrl = doc.createElement("Controler");
        ccurCtrl.setAttribute("type",(int)curCtrl->type());
        ccurParams = curCtrl->parameters()->toXmlData(doc,"parameters");

        ccurCtrl.appendChild(ccurParams);
        cControlers.appendChild(ccurCtrl);
    }

    return cControlers;
}

ModPlusCtrl* ModPlusCtrls::currentCtrl() const
{
    return this->value(_ctrlType);
}

ModPlusCtrl::Type ModPlusCtrls::currentCtrlType() const
{
    return _ctrlType;
}

void ModPlusCtrls::setCurrentCtrlType(ModPlusCtrl::Type type)
{
    if(type != _ctrlType)
    {
        _ctrlType = type;
        emit modified();
    }
}

 void ModPlusCtrls::setFromOtherCtrls(const ModPlusCtrls &newCtrls)
 {
     // clear content
     for(int i=0;i<values().size();i++)
     {
         delete values().at(i);
     }
     this->clear();

     // add clones
     ModPlusCtrl::Type curType;
     ModPlusCtrl* curCtrl;
     for(int i=0;i<newCtrls.keys().size();i++)
     {
         curType = newCtrls.keys().at(i);
         curCtrl = newCtrls.value(curType);
         this->insertCtrl(curType,curCtrl->clone());
     }
 }

void ModPlusCtrls::insertCtrl(ModPlusCtrl::Type i,ModPlusCtrl* ctrl)
{
    this->insert(i,ctrl);
    connect(ctrl,SIGNAL(modified()),this,SIGNAL(modified()));
}

