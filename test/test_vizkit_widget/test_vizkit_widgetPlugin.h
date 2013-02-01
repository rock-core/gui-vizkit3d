#ifndef TEST_VIZKIT_WIDGETPLUGIN_H
#define TEST_VIZKIT_WIDGETPLUGIN_H

#include <QtGui>
#include <QtDesigner/QDesignerCustomWidgetInterface>

class TestVizkitWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TestVizkitWidgetPlugin(QObject *parent = 0);
    virtual ~TestVizkitWidgetPlugin();

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget* createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *core);

private:
    bool initialized; 
};

#endif /* TEST_VIZKIT_WIDGETPLUGIN_H */  
