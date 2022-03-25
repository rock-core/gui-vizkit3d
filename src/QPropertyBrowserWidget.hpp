#ifndef QPROPERTYBROWSERWIDGET_HPP
#define QPROPERTYBROWSERWIDGET_HPP

#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>
#include <qtpropertymanager.h>

#include <QWidget>
#include <QHash>

namespace vizkit3d {

class QPropertyBrowserWidget : public QtTreePropertyBrowser
{
    Q_OBJECT
    
public:
    QPropertyBrowserWidget(QWidget* parent = 0);
    void addProperties(QObject* obj,QObject* parent = NULL);
    void removeProperties(QObject* obj);
    void addGlobalProperties(QObject* obj, const QStringList &property_list);
    
protected slots:
    void propertyChangedInGUI(QtProperty *property, const QVariant &val);
    void propertyChangedInObject(QString property_name);
    void propertyChangedInObject();
    void propObjDestroyed(QObject*);
    
private:
    QHash<QtProperty*, QObject*> propertyToObject;
    QHash<QObject*, QHash<QString ,QtProperty*>* > objectToProperties;
    QHash<QObject*, QtProperty*> objectToGroup;
    QtVariantPropertyManager *variantManager;
    QtVariantEditorFactory *factory;
    QtGroupPropertyManager* groupManager;
};

}

#endif
