#include "QPropertyBrowserWidget.hpp"
#include <qmetaobject.h>
#include <QString>
#include <vizkit/Vizkit3DPlugin.hpp>
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
 * Adds all properties of a QObject to the property browser widget.
 */
void QProperyBrowserWidget::addPropertys(QObject* obj)
{
    const QMetaObject* metaObj = obj->metaObject();
    
    // genarate group entry and all variant properties
    QtProperty* group = 0;
    QList<QtVariantProperty*> properties;
    for(int i = 1 ; i < metaObj->propertyCount(); i++)
    {
        if(strcmp(metaObj->property(i).name(),"vizkit3d_plugin_name") == 0)
        {
            QVariant var = metaObj->property(i).read(obj);
            group = groupManager->addProperty(var.toString());
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
        group = groupManager->addProperty("Vizkit3DPlugin");
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
    this->addProperty(group);
    
    // connect plugin signal, to notice if a property has changed
    VizPluginBase* plugin = dynamic_cast<VizPluginBase*>(obj);
    if(plugin)
        this->connect(plugin, SIGNAL(propertyChanged(QString)), this, SLOT(propertyChangedInObject(QString)));
}

/**
 * Removes all properies of a QObject from the property browser widget.
 */
void QProperyBrowserWidget::removePropertys(QObject* obj)
{
    // disconnect signal
    VizPluginBase* plugin = dynamic_cast<VizPluginBase*>(obj);
    if(plugin)
        this->disconnect(plugin, SIGNAL(propertyChanged(QString)), this, SLOT(propertyChangedInObject(QString)));
    
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