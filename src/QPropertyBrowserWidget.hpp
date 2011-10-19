#ifndef QPROPERTYBROWSERWIDGET_HPP
#define QPROPERTYBROWSERWIDGET_HPP

#include <vizkit/qtpropertybrowser/qttreepropertybrowser.h>
#include <vizkit/qtpropertybrowser/qtvariantproperty.h>
#include <vizkit/qtpropertybrowser/qtpropertymanager.h>
#include <QWidget>
#include <QHash>

namespace vizkit {
    
class QProperyBrowserWidget : public QtTreePropertyBrowser
{
    Q_OBJECT
    
public:
    QProperyBrowserWidget(QWidget* parent = 0);
    void addProperties(QObject* obj);
    void removeProperties(QObject* obj);
    
protected slots:
    void propertyChangedInGUI(QtProperty *property, const QVariant &val);
    void propertyChangedInObject(QString property_name);
    void propertyChangedInObject();
    
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
