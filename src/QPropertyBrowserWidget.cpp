#include "QPropertyBrowserWidget.hpp"
#include <qmetaobject.h>
#include <QString>
#include <iostream>

namespace vizkit
{

QProperyBrowserWidget::QProperyBrowserWidget(QWidget* parent) 
                      : QtTreePropertyBrowser(parent),
                        variantManager(0),
                        factory(0),
                        groupManager(0)
{
    factory = new QtVariantEditorFactory(this);
    variantManager = new QtVariantPropertyManager(this);
    groupManager = new QtGroupPropertyManager(this);
    this->setFactoryForManager(variantManager, factory);
    
    this->connect(variantManager, SIGNAL(valueChanged(QtProperty*,QVariant)), this, SLOT(propertyChangedInGUI(QtProperty*,QVariant)));
}

/**
 * Adds all properties of a QObject to the property browser widget as ungrouped global properties.
 */
void QProperyBrowserWidget::addGlobalProperties(QObject* obj, const QStringList& property_list)
{
    const QMetaObject* metaObj = obj->metaObject();
    QHash<QString, QtProperty*>* groupMap = new QHash<QString, QtProperty*>();
    
    for(int i = 1 ; i < metaObj->propertyCount(); i++)
    {
        if(property_list.contains(QString::fromAscii(metaObj->property(i).name())))
        {
            QtVariantProperty* property = variantManager->addProperty(metaObj->property(i).type(), metaObj->property(i).name());
            if(property == 0)
            {
                std::cerr << "QVariant type " << metaObj->property(i).type() << " with name " << metaObj->property(i).name() 
                        << " is not supported by the QtPropertyBrowser." << std::endl;
                continue;
            }
            property->setValue(metaObj->property(i).read(obj));
            propertyToObject[property] = obj;
            (*groupMap)[property->propertyName()] = property;
            this->addProperty(property);
        }
    }
    objectToProperties[obj] = groupMap;
    
    if (!this->connect(obj, SIGNAL(propertyChanged(QString)), this, SLOT(propertyChangedInObject(QString))))
    {
        std::cerr << "The QObject has no SIGNAL 'propertyChanged(QString)', the property browser widget won't get updated "
                  << "if properties in the QObject will change." << std::endl;
    }
}

/**
 * Adds all properties of a QObject to the property browser widget,
 * grouped by the name of the vizkit plugin.
 */
void QProperyBrowserWidget::addProperties(QObject* obj,QObject* parent)
{
    const QMetaObject* metaObj = obj->metaObject();
    QtProperty *group = NULL;
    QtProperty *parent_group = NULL;
    if(parent)
        parent_group = objectToGroup[parent];
    
    // genarate group entry and all variant properties
    QList<QtVariantProperty*> properties;
    for(int i = 1 ; i < metaObj->propertyCount(); i++)
    {
        if(strcmp(metaObj->property(i).name(),"vizkit3d_plugin_name") == 0)
        {
            QVariant var = metaObj->property(i).read(obj);
            group = groupManager->addProperty(var.toString());
            if(parent_group)
                parent_group->addSubProperty(group);
        }
        else
        {
            QtVariantProperty* property = variantManager->addProperty(metaObj->property(i).type(), metaObj->property(i).name());
            if(property == 0)
            {
                std::cerr << "QVariant type " << metaObj->property(i).type() << " with name " << metaObj->property(i).name() 
                        << " is not supported by the QtPropertyBrowser." << std::endl;
                continue;
            }
            property->setValue(metaObj->property(i).read(obj));
            properties.push_back(property);
        }
    }
    // add default plugin name if plugin name property is missing 
    if(group == 0)
    {
        group = groupManager->addProperty(obj->objectName());
        if(parent_group)
            parent_group->addSubProperty(group);
    }
    
    // add variant properties to the group
    QHash<QString, QtProperty*>* groupMap = new QHash<QString, QtProperty*>();
    for(QList<QtVariantProperty*>::const_iterator it = properties.begin(); it != properties.end(); it++)
    {
        group->addSubProperty(*it);
        propertyToObject[*it] = obj;
        (*groupMap)[(*it)->propertyName()] = *it;
    }
    
    // add group to the tree
    objectToGroup[obj] = group;
    objectToProperties[obj] = groupMap;

    // do not add property if it is already added to a parent_group
    if(!parent_group)
        this->addProperty(group);
    
    // connect plugin signal, to notice if a property has changed
    if (!this->connect(obj, SIGNAL(propertyChanged(QString)), this, SLOT(propertyChangedInObject(QString))))
    {
        std::cerr << "The QObject has no SIGNAL 'propertyChanged(QString)', the property browser widget won't get updated "
                  << "if properties in the QObject will change." << std::endl;
    }
}

/**
 * Removes all properies of a QObject from the property browser widget.
 */
void QProperyBrowserWidget::removeProperties(QObject* obj)
{
    // disconnect signal
    this->disconnect(obj, SIGNAL(propertyChanged(QString)), this, SLOT(propertyChangedInObject(QString)));
    
    // remove properties
    if(objectToGroup[obj])
    {
        QList<QtProperty*> properties = objectToGroup[obj]->subProperties();
        for(QList<QtProperty*>::iterator it = properties.begin(); it != properties.end(); it++)
        {
            propertyToObject.remove(*it); 
        }
        this->removeProperty(objectToGroup[obj]);
        objectToGroup.remove(obj);
    }
    // delete property group hash map
    if(objectToProperties[obj])
    {
        QHash<QString, QtProperty*>* groupMap = objectToProperties[obj];
        objectToProperties.remove(obj);
        delete groupMap;
    }
}

/**
 * Slot, to handle updates from the GUI
 */
void QProperyBrowserWidget::propertyChangedInGUI(QtProperty* property, const QVariant& val)
{
    if (propertyToObject[property] != 0)
    {
        propertyToObject[property]->setProperty(property->propertyName().toStdString().c_str(), val);
    }
}

/**
 * Slot, to handle updates from the QObjects
 */
void QProperyBrowserWidget::propertyChangedInObject(QString property_name)
{
    QObject* obj = QObject::sender();
    QHash<QString, QtProperty*>* groupMap = objectToProperties[obj];
    if (obj && groupMap && groupMap->contains(property_name))
    {
        QtProperty* property = groupMap->value(property_name);
        QVariant value = obj->property(property_name.toStdString().c_str());
        if(value.isValid())
        {
            variantManager->setValue(property, value);
        }
    }
}

/**
 * Slot, to handle updates from the QObjects
 * Here the method has no information which property has changed,
 * so it'll update all of them.
 */
void QProperyBrowserWidget::propertyChangedInObject()
{
    QObject* obj = QObject::sender();
    if (obj && objectToProperties[obj])
    {
        const QMetaObject* metaObj = obj->metaObject();
        QHash<QString, QtProperty*>* groupMap = objectToProperties[obj];
        
        for(int i = 1 ; i < metaObj->propertyCount(); i++)
        {
            QString property_name(metaObj->property(i).name());
            QtProperty* property = groupMap->value(property_name);
            if(property)
            {
                variantManager->setValue(property, metaObj->property(i).read(obj));
            }
        }
    }
}

}
