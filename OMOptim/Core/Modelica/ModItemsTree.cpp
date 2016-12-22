// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

  @file ModItemsTree.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "ModItemsTree.h"
#include "Project.h"



ModItemsTree::ModItemsTree(Project* project,ModLoader* modLoader,MOomc* moomc,QObject *parent)
    : QAbstractItemModel(parent)
{
    _modLoader = modLoader;
    _moomc = moomc;
    _rootElement = new ModItem(_moomc);
    _project = project;
    _enabled = true;

    _showComponents = true;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // no more setSupportedDragActions in Qt5, override supportedDropActions instead
#else
    setSupportedDragActions(Qt::CopyAction | Qt::MoveAction);
#endif


}

ModItemsTree::~ModItemsTree()
{
    clear();
    delete _rootElement;
}


void ModItemsTree::clear()
{
    beginResetModel();
    _rootElement->clear();
    endResetModel();
}


bool ModItemsTree::addChild(ModItem* parent,ModItem* child)
{
    bool ok;
    QModelIndex parentIndex = indexOf(parent);
    beginInsertRows(parentIndex,parent->childCount(),parent->childCount());
    ok = parent->addChild(child);
    endInsertRows();
    return ok;
}

void ModItemsTree::readFromOMCWThread(ModItem* parent,int depthMax,  QString direction, int curDepth)
{

    // load the models icon
    ModItemsLoader modItemsLoader(this,parent,depthMax,direction,curDepth);
    modItemsLoader.run();
    // still not working properly on all platforms
    //    while (modItemsLoader->isRunning())
    //    {
    //        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    //    }

}

void ModItemsTree::addExeModel(ModItem* parent, QString name, QFileInfo inputFile, QFileInfo exeFile)
{
    ModItem* newExeElement = new ExeModel(name,exeFile,inputFile);
    addChild(parent, newExeElement);
    parent->setChildrenReaden(true);
    parent->emitModified();
}

void ModItemsTree::emitDataChanged()
{

    if(_rootElement->childCount()>0)
    {
        QModelIndex first = indexOf(_rootElement->child(0));
        QModelIndex last = indexOf(_rootElement->child(_rootElement->childCount()-1));
        emit dataChanged(first,last);
    }

    emit dataChanged(indexOf(_rootElement),indexOf(_rootElement));

}




/**
* This function fills parent with its children. To achieve this, it calls MOOmc functions.
* @arg depthMax : maximum depth filling should go (each time functions goes in children, grand children...
* Depth is increased by one.
* @arg curDepth : current depth.
* @arg direction : allows to drive looking function along a path. This is especially used
* to look for deep modelas without covering all items.
*/
void ModItemsTree::readFromOmc(ModItem* parent,int depthMax,QString direction,int curDepth)
{
    if(parent->_readMutex.tryLock() && _project->useOmc())
    {
        if((curDepth<=depthMax)&&!parent->childrenReaden())
        {
            ModItem* newElement;



            QString childrenDirection = direction.section(".",curDepth+1,curDepth+1);

            bool readPackages = (parent->getClassRestr()==Modelica::PACKAGE) || (parent->getClassRestr()==Modelica::GENERIC);
            bool readModels = (parent->getClassRestr()==Modelica::PACKAGE) || (parent->getClassRestr()==Modelica::GENERIC);
            bool readComponents = (parent->getClassRestr()==Modelica::MODEL) || (parent->getClassRestr()==Modelica::GENERIC) || (parent->getClassRestr()==Modelica::COMPONENT);
            bool readRecords = (parent->getClassRestr()==Modelica::PACKAGE) || (parent->getClassRestr()==Modelica::MODEL) ||(parent->getClassRestr()==Modelica::GENERIC)|| (parent->getClassRestr()==Modelica::COMPONENT);
            bool readClasses = readModels;

            QString fullParentName = parent->name(ModItem::FULL);
            QString parentClassName = parent->getModClassName();
            QString prefix;
            if(!fullParentName.isEmpty())
                prefix = fullParentName+".";

            // read packages
            if(readPackages)
            {
                QStringList packageNames = _moomc->getPackages(fullParentName);
                for(int i=0;i<packageNames.size();i++)
                {
                    newElement = new ModPackage(_moomc,parent,prefix+packageNames.at(i));
                    if(addChild(parent,newElement))
                        if((childrenDirection=="") || (childrenDirection==packageNames.at(i)))
                            readFromOMCWThread(newElement,depthMax,direction,curDepth+1);
                }
            }

            // read models
            if(readModels)
            {
                QStringList modelNames = _moomc->getModels(fullParentName);
                for(int i=0;i<modelNames.size();i++)
                {
                    newElement = new ModModel(_moomc,parent,prefix+modelNames.at(i));
                    if(addChild(parent,newElement))
                        if((childrenDirection=="") || (childrenDirection==modelNames.at(i)))
                            readFromOMCWThread(newElement,depthMax,direction,curDepth+1);
                }
            }

            // read records
            if(readRecords)
            {
                QStringList recordNames = _moomc->getRecords(fullParentName);
                for(int i=0;i<recordNames.size();i++)
                {
                    newElement = new ModRecord(_moomc,parent,prefix+recordNames.at(i));
                    if(addChild(parent,newElement))
                        if((childrenDirection=="") || (childrenDirection==recordNames.at(i)))
                            readFromOMCWThread(newElement,depthMax,direction,curDepth+1);
                }
            }

            // read classes
            if(readClasses)
            {
                QStringList classNames = _moomc->getClasses(fullParentName);
                for(int i=0;i<classNames.size();i++)
                {
                    newElement = new ModItem(_moomc,parent,prefix+classNames.at(i));
                    if(addChild(parent,newElement))
                        if((childrenDirection=="") || (childrenDirection==classNames.at(i)))
                            readFromOMCWThread(newElement,depthMax,direction,curDepth+1);
                }
            }

            //read components
            if(readComponents)
            {
                QStringList compNames;
                QStringList compClasses;
                _moomc->getContainedComponents(parentClassName,compNames,compClasses,true);
                for(int i=0;i<compNames.size();i++)
                {
                    newElement = new ModComponent(_moomc,parent,prefix+compNames.at(i),compClasses.at(i));
                    if(addChild(parent,newElement))
                        if((childrenDirection=="") || (childrenDirection==compClasses.at(i)))
                            readFromOMCWThread(newElement,depthMax,direction,curDepth+1);
                }
            }
            parent->setChildrenReaden(true);
            parent->emitModified();


        }
        parent->_readMutex.unlock();
    }
}

/**
  * Another version of readFromOmc.
  * @sa readFromOmc
  * @todo benchmark reading functions
  */
void ModItemsTree::readFromOmcV2(ModItem* parent,int depthMax,QString direction,int curDepth)
{
     if(parent->_readMutex.tryLock() && _project->useOmc())
    {
        if((curDepth<=depthMax)&&!parent->childrenReaden())
        {
            QString childrenDirection = direction.section(".",curDepth+1,curDepth+1);
            QString fullParentClass = parent->getModClassName();

            QString fullParentName = parent->name(ModItem::FULL);
            QString prefix;
            if(!fullParentName.isEmpty())
                prefix = fullParentName+".";


            ModItem* newElement;

            //read packages, models and components
            QStringList packagesClasses,modelsClasses,recordNames,compsNames,compsClasses;
            _moomc->readElementInfos(fullParentClass,packagesClasses,modelsClasses,recordNames,compsNames,compsClasses);

            // get inherited components
            QStringList inheritedCompNames,inheritedCompClasses;
            _moomc->getInheritedComponents(fullParentClass,inheritedCompNames,inheritedCompClasses);
            compsNames << inheritedCompNames;
            compsClasses << inheritedCompClasses;

            // adding child packages and read them
            for(int i=0;i<packagesClasses.size();i++)
            {
                newElement = new ModPackage(_moomc,parent,prefix+packagesClasses.at(i));
                if(addChild(parent,newElement))
                    if((childrenDirection=="") || (childrenDirection==packagesClasses.at(i)))
                        readFromOmcV2(newElement,depthMax,direction,curDepth+1);
            }

            // adding child models and read them
            for(int i=0;i<modelsClasses.size();i++)
            {
                newElement = new ModModel(_moomc,parent,prefix+modelsClasses.at(i));
                if(addChild(parent,newElement))
                    if((childrenDirection=="") || (childrenDirection==modelsClasses.at(i)))
                        readFromOmcV2(newElement,depthMax,direction,curDepth+1);
            }

            // adding child components and read them
            for(int i=0;i<compsNames.size();i++)
            {
                newElement = new ModComponent(_moomc,parent,prefix+compsNames.at(i),compsClasses.at(i));
                if(addChild(parent,newElement))
                    if((childrenDirection=="") || (childrenDirection==compsClasses.at(i)))
                        readFromOmcV2(newElement,depthMax,direction,curDepth+1);
            }

            parent->setChildrenReaden(true);
            parent->emitModified();


        }
        parent->_readMutex.unlock();
    }
}

/**
  * Another version of readFromOmc.
  * @sa readFromOmc
  * @todo benchmark reading functions
  */
void ModItemsTree::readFromOmcV3(ModItem* parent,int depthMax,QString direction,int curDepth)
{
     if(parent->_readMutex.tryLock() && _project->useOmc())
    {
        if((curDepth<=depthMax)&&!parent->childrenReaden())
        {
            QString childrenDirection = direction.section(".",curDepth+1,curDepth+1);
            QString fullParentClass = parent->getModClassName();
            QString fullParentName = parent->name(ModItem::FULL);


            QString prefix;
            if(!fullParentName.isEmpty())
                prefix = fullParentName+".";

            ModItem* newElement;

            //read packages, models and components
            QStringList packagesClasses,modelsClasses,recordsNames,compsNames,compsClasses;
            bool foundInLib = false;


            ModItem* _corrLibComp = findInDescendants(fullParentName);
            if(_corrLibComp)
            {
                // If class is already loaded in package, use its information
                foundInLib = true;
                childrenInfos(_corrLibComp,packagesClasses,modelsClasses,recordsNames,compsNames,compsClasses);
            }
            else
            {
                // Call parent->_moomc
                _moomc->readElementInfos(fullParentClass,packagesClasses,modelsClasses,recordsNames,compsNames,compsClasses);
                // get inherited components
                QStringList inheritedCompNames,inheritedCompClasses;
                _moomc->getInheritedComponents(fullParentClass,inheritedCompNames,inheritedCompClasses);
                compsNames << inheritedCompNames;
                compsClasses << inheritedCompClasses;
            }


            // adding child packages and read them
            for(int i=0;i<packagesClasses.size();i++)
            {
                newElement = new ModPackage(_moomc,parent,prefix+packagesClasses.at(i));
                if(addChild(parent,newElement))
                    if((childrenDirection=="") || (childrenDirection==packagesClasses.at(i)))
                        readFromOmcV3(newElement,depthMax,direction,curDepth+1);
            }

            // adding child models and read them
            for(int i=0;i<modelsClasses.size();i++)
            {
                newElement = new ModModel(_moomc,parent,prefix+modelsClasses.at(i));
                if(addChild(parent,newElement))
                    if((childrenDirection=="") || (childrenDirection==modelsClasses.at(i)))
                        readFromOmcV3(newElement,depthMax,direction,curDepth+1);
            }

            // adding child components and read them
            for(int i=0;i<compsNames.size();i++)
            {
                newElement = new ModComponent(_moomc,parent,prefix+compsNames.at(i),compsClasses.at(i));
                if(addChild(parent,newElement))
                    if((childrenDirection=="") || (childrenDirection==compsClasses.at(i)))
                        readFromOmcV3(newElement,depthMax,direction,curDepth+1);
            }

            parent->setChildrenReaden(true);
            parent->emitModified();

        }
        parent->_readMutex.unlock();
    }
}



/**
  * Finding function : within parent, look for children whom fullname equals argument
  */
ModItem* ModItemsTree::findInDescendants(QString fullName,ModItem* parent)
{
    if(parent==NULL)
        parent = _rootElement;

    ModItem* curChild;
    QString curFullName = parent->name(ModItem::FULL);

    int curDepth = parent->depth();
    int lookingDepth = fullName.split(".").size()-1;

    // check if it is this component
    if(curFullName == fullName)
        return parent;

    //first check name compatibility
    if(fullName.indexOf(curFullName)!=0)
        return NULL;

    // then check children are readen
    if(!parent->childrenReaden())
    {
        // if not, check in its direction
        readFromOmc(parent,lookingDepth,fullName,curDepth);
    }

    //get child name to visit
    //QString childShortName = fullName.section(".",curDepth,curDepth);
    QString childShortName = fullName;
    // first remove parent name
    // A ajouter la une condition if not executable else childShortName = curFullName !!!!!!
    if(!fullName.endsWith(".exe"))
    {
        childShortName.remove(QRegExp("^"+curFullName+"\\."));
        // then take first section
        childShortName = childShortName.section(".",0,0);


        // looking in children
        for(int iChild=0;iChild<parent->childCount();iChild++)
        {
            curChild = parent->child(iChild);
            if(curChild->name(ModItem::SHORT)==childShortName)
                return findInDescendants(fullName,curChild);
        }
    }
    else
    {
        for(int iChild=0;iChild<parent->childCount();iChild++)
        {
            curChild = parent->child(iChild);
            if(curChild->name(ModItem::FULL)==childShortName)
                return findInDescendants(fullName,curChild);
        }


    }
    return NULL;
}

/**
  * Finding function : returns all ModItem whom classname equals argument className.
  */
QList<ModItem*> ModItemsTree::findInDescendantsByClass(QString className,ModItem* parent)
{
    if(parent==NULL)
        parent = _rootElement;

    ModItem* curClass;
    QList<ModItem*> curClasses;
    int iChild;

    int curDepth = parent->depth();



    // then check children are readen
    if(!parent->childrenReaden())
    {
        // if not, check in its direction
        readFromOMCWThread(parent,curDepth+1,QString(),curDepth);
    }

    // looking if one child corresponds
    for(iChild=0;iChild<parent->childCount();iChild++)
    {
        curClass = parent->child(iChild);
        if(curClass->getModClassName()==className)
            curClasses.push_back(curClass);

        //look deeper in children
        curClasses <<  findInDescendantsByClass(className,curClass);
    }

    return curClasses;

}

/**
  * Finding function : returns all ModItem whom class equals or inherits className.
  */
QList<ModItem*> ModItemsTree::findInDescendantsByInheritedClass(QString className,ModItem* parent)
{
    if(parent==NULL)
        parent = _rootElement;

    ModItem* curChild;
    QString curChildClass;
    QList<ModItem*> result;
    int iChild;

    int curDepth = parent->depth();

    // then check children are readen
    if(!parent->childrenReaden())
    {
        // if not, check in its direction
        readFromOMCWThread(parent,curDepth+1,QString(),curDepth);
    }

    // looking if one child corresponds
    for(iChild=0;iChild<parent->childCount();iChild++)
    {
        curChild = parent->child(iChild);
        curChildClass = curChild->getModClassName();
        if((curChildClass==className)||_moomc->inherits(curChildClass,className))
            result.push_back(curChild);

        //look deeper in children
        result <<  findInDescendantsByInheritedClass(className,curChild);
    }

    return result;
}


/**
  * Finding function : returns all components whom classname inherits argument className.
  */
QList<ModItem*> ModItemsTree::findCompOfInheritingClassInDescendants(QString className,ModItem* parent)
{
    if(parent==NULL)
        parent = _rootElement;

    ModItem* curComponent;
    QList<ModItem*> curComponents;
    int iChild;
    QString curCompClass;

    int curDepth = parent->depth();

    // then check children are readen
    if(!parent->childrenReaden())
    {
        // if not, check in its direction
        readFromOMCWThread(parent,curDepth+1,QString(),curDepth);
    }

    int nbCompChild = parent->compChildCount();
    int nbModelChild = parent->modelChildCount();
    int nbPackageChild = parent->packageChildCount();

    // looking if one child corresponds
    for(iChild=0;iChild<nbCompChild;iChild++)
    {
        curComponent = parent->compChild(iChild);
        curCompClass = curComponent->getModClassName();
        if((curCompClass==className)||_moomc->inherits(curCompClass,className))
            curComponents.push_back(curComponent);
    }

    //look deeper in components children
    iChild=0;
    while(iChild<nbCompChild)
    {
        curComponents <<  findCompOfInheritingClassInDescendants(className,parent->compChild(iChild));
        iChild++;
    }

    //look deeper in models children
    iChild=0;
    while(iChild<nbModelChild)
    {
        curComponents <<   findCompOfInheritingClassInDescendants(className,parent->modelChild(iChild));
        iChild++;
    }

    //look deeper in packages children
    iChild=0;
    while(iChild<nbPackageChild)
    {
        curComponents <<  findCompOfInheritingClassInDescendants(className,parent->packageChild(iChild));
        iChild++;
    }

    return curComponents;
}

/**
  * Finding function : returns all components whom classname inherits argument className.
  * In a multimap since there are different classNames here.
  * @param classFilter : stop seeking in parent class does not go through this filter
  */
void  ModItemsTree::findCompOfInheritingClassesInDescendants(QStringList classNames,ModItem* parent,QMultiMap<QString,ModItem*> &result,QRegExp classFilter)
{
    if(parent==NULL)
        return;

    ModItem* curComponent;

    int iChild;
    QString curCompClass;

    QString parentClass = parent->getModClassName();
    if(classFilter.isValid() && !parentClass.contains(classFilter))
        return ;// no need to carry on

    int curDepth = parent->depth();
    // then check children are readen
    if(!parent->childrenReaden())
    {
        // if not, check in its direction
        readFromOMCWThread(parent,curDepth+1,QString(),curDepth);
    }

    int nbCompChild = parent->compChildCount();
    int nbModelChild = parent->modelChildCount();
    int nbPackageChild = parent->packageChildCount();

    // looking if one child corresponds
    for(iChild=0;iChild<nbCompChild;iChild++)
    {
        curComponent = parent->compChild(iChild);
        curCompClass = curComponent->getModClassName();

        foreach(QString  className, classNames)
        {
            if((curCompClass==className)||_moomc->inherits(curCompClass,className))
                result.insert(className,curComponent);
        }
    }

    //look deeper in components children
    iChild=0;
    while(iChild<nbCompChild)
    {
        findCompOfInheritingClassesInDescendants(classNames,parent->compChild(iChild),result);
        iChild++;
    }

    //look deeper in models children
    iChild=0;
    while(iChild<nbModelChild)
    {
        findCompOfInheritingClassesInDescendants(classNames,parent->modelChild(iChild),result);
        iChild++;
    }

    //look deeper in packages children
    iChild=0;
    while(iChild<nbPackageChild)
    {
        findCompOfInheritingClassesInDescendants(classNames,parent->packageChild(iChild),result);
        iChild++;
    }
}

void  ModItemsTree::findCompOfInheritingClassesInDescendants2(const QStringList & classNames,QString parentName,QString parentClass,QMultiMap<QString,QString> &result,QRegExp classFilter)
{

    if(classFilter.isValid() && !parentClass.contains(classFilter))
        return ;// no need to carry on


    QStringList compShortNames,compClasses;
    QString childLongName;
    // quick version but does not catch everything
    _moomc->getContainedComponents(parentClass,compShortNames,compClasses,true);

    foreach(QString className, classNames)
    {
        for(int i=0;i<compClasses.size();i++)
        {
            if(!classFilter.isValid() || parentClass.contains(classFilter))
            {
                if((compClasses.at(i)==className)||_moomc->inherits(compClasses.at(i),className))
                {
                    childLongName = parentClass.isEmpty() ? compShortNames.at(i) : parentName+"."+compShortNames.at(i);
                    result.insert(className,childLongName);
                }
            }
        }
    }

    ModItem* compItem;
    for(int i=0;i<compClasses.size();i++)
    {
        childLongName = parentClass.isEmpty() ? compShortNames.at(i) : parentName+"."+compShortNames.at(i);
        findCompOfInheritingClassesInDescendants2(classNames,childLongName,compClasses.at(i),result,classFilter);
    }
}



/**
  * Finding function : returns all components whom classname equals argument className.
  */
QList<ModItem*> ModItemsTree::findCompOfClassInDescendants(QString className,ModItem* parent)
{
    if(parent==NULL)
        parent = _rootElement;

    ModItem* curComponent;
    QList<ModItem*> curComponents;
    int iChild;

    int curDepth = parent->depth();
    // then check children are readen
    if(!parent->childrenReaden())
    {
        // if not, check in its direction
        readFromOMCWThread(parent,curDepth+1,QString(),curDepth);
    }

    int nbCompChild = parent->compChildCount();
    int nbModelChild = parent->modelChildCount();
    int nbPackageChild = parent->packageChildCount();

    // looking if one child corresponds
    for(iChild=0;iChild<nbCompChild;iChild++)
    {
        curComponent = parent->compChild(iChild);
        if(curComponent->getFieldValue(ModComponent::CLASSNAME)==className)
            curComponents.push_back(curComponent);
    }

    //look deeper in components children
    iChild=0;
    while(iChild<nbCompChild)
    {
        curComponents <<  findCompOfClassInDescendants(className,parent->compChild(iChild));
        iChild++;
    }

    //look deeper in models children
    iChild=0;
    while(iChild<nbModelChild)
    {
        curComponents <<   findCompOfClassInDescendants(className,parent->modelChild(iChild));
        iChild++;
    }

    //look deeper in packages children
    iChild=0;
    while(iChild<nbPackageChild)
    {
        curComponents <<  findCompOfClassInDescendants(className,parent->packageChild(iChild));
        iChild++;
    }

    return curComponents;
}

/**
  * Returns whether or not a ModItem is in this tree.
  */
bool ModItemsTree::isInDescendants(QString fullName,ModItem* parent)
{
    ModItem* foundClass = findInDescendants(fullName,parent);
    return (bool)(foundClass);
}


int ModItemsTree::columnCount(const QModelIndex &parent) const
{
    //return ModItem::nbFields;
    return 1;
}

QVariant ModItemsTree::data(const QModelIndex &index, int role) const
{
    if(_enabled)
    {
        if (!index.isValid())
            return QVariant();


        if (role != Qt::DisplayRole && role !=Qt::ToolTipRole && role != Qt::DecorationRole)
            return QVariant();

        if(index.column()<0 || index.column()>=ModItem::nbFields)
            return QVariant();

        ModItem *item = static_cast<ModItem*>(index.internalPointer());

        if(item)
        {
            if(role==Qt::DecorationRole)
                return getModelicaNodeIcon(item);

            // fullfiling is now done in ::fetchmore
            // if(!item->childrenReaden())
            // readFromOmcV2(item,1);

            if (role == Qt::ToolTipRole)
            {
                QString tooltip = item->getStrToolTip();
                return tooltip;
            }

            // if display, only display short name (since hierarchy is visible)
            if((role == Qt::DisplayRole) && (index.column()==ModItem::NAME))
                return item->name(ModItem::SHORT);

            return item->getFieldValue(index.column(),role);
        }
        else
        {
            return QVariant();
        }
    }
    else
        return QVariant();
}


QMimeData* ModItemsTree::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QString csv;

    // select only first column indexes (since selection is made by row)
    QModelIndexList rowIndexes;
    QList<ModItem*> items;
    for(int i=0;i<indexes.size();i++)
    {
        if(indexes.at(i).column()==0)
        {
            rowIndexes.push_back(indexes.at(i));
            items.push_back((ModItem*)indexes.at(i).internalPointer());
        }
    }

    // Remove children of contained parents -> avoid twice copying
    QList<ModItem*> uniqueItems;
    for(int i=0;i<items.size();i++)
    {
        //    if(!items.contains(items.at(i)->parent()) || (items.at(i)->parent()==_rootElement))
        uniqueItems.push_back(items.at(i));
    }

    // create text data
    for(int i=0;i<uniqueItems.size();i++)
    {
        ModItem* item = uniqueItems.at(i);
        csv.push_back(item->name(ModItem::FULL)+"\n");
    }
    if(csv.size()>0)
        csv.remove(csv.size()-QString("\n").size(),QString("\n").size());
    mimeData->setText(csv);
    mimeData->setData("application/ModItemName",csv.toAscii());
    return mimeData;
}


Qt::ItemFlags ModItemsTree::flags(const QModelIndex &index) const
{
    if(!_enabled)
        return Qt::NoItemFlags;

    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

}

QVariant ModItemsTree::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
    if(!_enabled)
        return QVariant();

    if(section<0 || section>=ModItem::nbFields)
        return QVariant();

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return ModItem::sFieldName(section,role);

    return QVariant();
}


QModelIndex ModItemsTree::index(int row, int column, const QModelIndex &parent)
const
{
    if(!_enabled)
        return QModelIndex();

    if(!hasIndex(row,column,parent))
        return QModelIndex();

    ModItem *parentComponent;

    if (!parent.isValid())
        parentComponent = _rootElement;
    else
        parentComponent = static_cast<ModItem*>(parent.internalPointer());

    // looking in children
    //    int nbPacks = parentComponent->packageChildCount();
    //    int nbModels = parentComponent->modelChildCount();
    //    int nbComps = parentComponent->compChildCount();

    if(row<0 || row>= parentComponent->childCount())
        return QModelIndex();

    ModItem *childElement = parentComponent->child(row);

    if (!childElement)
        return QModelIndex();

    if(!_showComponents&&(childElement->getClassRestr()==Modelica::COMPONENT))
        return QModelIndex();
    else
        return createIndex(row, column, childElement);
}

QModelIndex ModItemsTree::indexOf(ModItem* item,int column)
{
    if(item==_rootElement)
        return createIndex(0,column,item);
    else
        return createIndex(item->indexInParent(),column,item);
}

QModelIndex ModItemsTree::parent(const QModelIndex &index) const
{
    if(!_enabled)
        return QModelIndex();

    if (!index.isValid())
        return QModelIndex();

    ModItem *childElement = static_cast<ModItem*>(index.internalPointer());
    if (childElement == _rootElement)
        return QModelIndex();

    ModItem *parentElement  = NULL;
    parentElement = childElement->parentItem();

    if (parentElement == _rootElement)
        return QModelIndex();

    ModItem *grandParentElement = NULL;
    grandParentElement = parentElement->parentItem();

    int iC = grandParentElement->children().indexOf(parentElement);

    if(iC==-1)
    {
        // ERROR
        return QModelIndex();
    }

    return createIndex(iC, 0, parentElement);
}
bool ModItemsTree::hasChildren ( const QModelIndex & parent ) const
{
    bool hasChildren;
    bool triedToFind;

    if(!_enabled)
        return false;

    ModItem *parentElement;

    if (parent.column() > 0)
        return false;

    if (!parent.isValid())
        parentElement = rootElement();
    else
        parentElement = static_cast<ModItem*>(parent.internalPointer());

    if(parentElement->childrenReaden())
    {
        if(_showComponents)
            return (parentElement->childCount()>0);
        else
            return (parentElement->childCount()-parentElement->compChildCount()>0);
    }
    else
    {
        triedToFind = true;
        hasChildren = false;

        QStringList children = _moomc->getClassNames(parentElement->name());
        children.removeAll(QString());
        if(!children.isEmpty())
            hasChildren = true;
        else if(parentElement->getClassRestr()==Modelica::MODEL)
        {
            if(!_showComponents)
            {
                hasChildren = false;
                triedToFind = false;
            }
            else
            {
                // look if has component children
                QStringList compNames;
                QStringList compClasses;
                _moomc->getContainedComponents(parentElement->getModClassName(),compNames,compClasses,true);
                hasChildren = (!compNames.isEmpty());
            }
        }
        if(triedToFind && !hasChildren)
            parentElement->setChildrenReaden(true); // avoid re-reading

        return hasChildren;
    }
}
int ModItemsTree::rowCount(const QModelIndex &parent) const
{
    if(!_enabled)
        return 0;

    ModItem *parentElement;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentElement = _rootElement;
    else
        parentElement = static_cast<ModItem*>(parent.internalPointer());

    int count = parentElement->childCount();

    if(!_showComponents)
        count-=parentElement->compChildCount();

    return count;
}

/**
* \brief
* Return first ModModel in item parents (including himself).
* This function can for instance be used to know which model compile
* in order to update item value.
* @param itemName : full name of item whom who are looking parent model
*/
ModItem *ModItemsTree::modelOf(QString itemName)
{
    ModItem* item = findInDescendants(itemName);
    return modelOf(item);
}
/**
* \brief
* Return first ModModel in item parents (including himself).
* This function can for instance be used to know which model compile
* in order to update item value.
*/
ModModel* ModItemsTree::modelOf(ModItem* item)
{
    if(item==NULL)
        return NULL;

    if(item->getClassRestr()==Modelica::MODEL)
        return dynamic_cast<ModModel*>(item);
    else
        return dynamic_cast<ModModel*>(modelOf(item->parentItem()));
}


ModItem* ModItemsTree::findItem(QString fullName)
{
    return findInDescendants(fullName,_rootElement);
}



void ModItemsTree::childrenInfos(ModItem* parent,QStringList &packagesClasses,QStringList &modelsClasses,QStringList &recordsClasses,QStringList &compsNames,QStringList &compsClasses)
{
    // read children if necessary
    if(!parent->childrenReaden())
    {
        int curDepth = parent->depth();
        readFromOmcV2(parent,curDepth+1,QString(),curDepth);
    }

    int nbCompChild = parent->compChildCount();
    int nbModelChild = parent->modelChildCount();
    int nbPackageChild = parent->packageChildCount();
    int nbRecordChild = parent->recordChildCount();

    //clear
    packagesClasses.clear();
    modelsClasses.clear();
    compsClasses.clear();
    compsNames.clear();
    recordsClasses.clear();

    // packages
    for(int i=0;i<nbPackageChild;i++)
        packagesClasses.push_back(parent->packageChild(i)->name(ModItem::SHORT));

    // models
    for(int i=0;i<nbModelChild;i++)
        modelsClasses.push_back(parent->modelChild(i)->name(ModItem::SHORT));

    // records
    for(int i=0;i<nbRecordChild;i++)
        recordsClasses.push_back(parent->modelChild(i)->name(ModItem::SHORT));

    // components
    for(int i=0;i<nbCompChild;i++)
    {
        compsClasses.push_back(parent->compChild(i)->getFieldValue(ModComponent::CLASSNAME).toString());
        compsNames.push_back(parent->compChild(i)->name(ModItem::SHORT));
    }
}


// get all ports of component parent (not only those connected)
QList<ModItem*> ModItemsTree::getPorts(ModItem* parent)
{
    QList<ModItem*> ports;

    int curDepth = parent->depth();
    if(!parent->childrenReaden())
    {
        readFromOmc(parent,curDepth+1,QString(),curDepth);
    }

    int nbCompChild = parent->compChildCount();
    for(int i=0; i<nbCompChild;i++)
    {
        ModComponent* curComp = (ModComponent*)parent->compChild(i);
        if(_moomc->isConnector(curComp->getFieldValue(ModComponent::CLASSNAME).toString()))
            ports.push_back(curComp);
    }
    return ports;
}

QStringList ModItemsTree::getPorts(ModItem* parent,ModItem::NameFormat format)
{
    QList<ModItem*> _ports = getPorts(parent);

    QStringList _portsNames;
    for(int i=0;i<_ports.size();i++)
        _portsNames.push_back(_ports.at(i)->name(format));

    return _portsNames;
}


QIcon ModItemsTree::getModelicaNodeIcon(ModItem* ModItem) const
{
    switch (ModItem->getClassRestr())
    {
    case Modelica::MODEL:
        return QIcon(":/icons/model-icon.png");
    case Modelica::CLASS:
        return QIcon(":/icons/class-icon.png");
    case Modelica::CONNECTOR:
        return QIcon(":/icons/connector-icon.png");
    case Modelica::RECORD:
        return QIcon(":/icons/record-icon.png");
    case Modelica::BLOCK:
        return QIcon(":/icons/block-icon.png");
    case Modelica::FUNCTION:
        return QIcon(":/icons/function-icon.png");
    case Modelica::PACKAGE:
        return QIcon(":/icons/package-icon.png");
    case Modelica::TYPE:
        return QIcon(":/icons/type-icon.png");
    case Modelica::EXECUTABLE:
        return QIcon(":/icons/exe-icon.png");
    default :
        return QIcon();
    }
}

bool ModItemsTree::canFetchMore ( const QModelIndex & parent ) const
{
    ModItem *item = static_cast<ModItem*>(parent.internalPointer());

    if(!_enabled)
        return false;

    if (!parent.isValid())
        return false;

    if(parent.column()<0 || parent.column()>=ModItem::nbFields)
        return false;

    //    ModItem *item = static_cast<ModItem*>(parent.internalPointer());

    if(item && !item->childrenReaden())
        return true;
    else
        return false;
}

void ModItemsTree::fetchMore ( const QModelIndex & parent )
{
    if(!_enabled)
        return;

    if (!parent.isValid())
        return;

    ModItem *item = static_cast<ModItem*>(parent.internalPointer());

    if(item)
    {
        if(!item->childrenReaden())
            readFromOMCWThread(item,_modLoader->getDepthMax());
    }
}

void ModItemsTree::setShowComponent(bool showComponents)
{
    if(showComponents!=_showComponents)
    {
        beginResetModel();
        _showComponents = showComponents;
        endResetModel();
    }
}

//Qt::DropActions ModItemsTree::supportedDropActions() const
//{
//    return Qt::CopyAction | Qt::MoveAction;
//}



ModItemsLoader::ModItemsLoader(ModItemsTree* modItemsTree,ModItem* parent,int depthMax,  QString direction , int curDepth )
{
    _modItemsTree = modItemsTree;
    _parent = parent;
    _depthMax = depthMax;
    _direction = direction;
    _curDepth = curDepth;


    connect(this, SIGNAL(readFromOmc(ModItem*,int,QString,int)),
            _modItemsTree, SLOT(readFromOmc(ModItem*,int, QString,int)));
    //    connect(this, SIGNAL(started()), this, SLOT(hasStarted()));
    //    connect(this, SIGNAL(finished()), this, SLOT(hasFinished()));
}



void ModItemsLoader::run()
{
    emit readFromOmc(_parent,_depthMax,_direction,_curDepth);
}
