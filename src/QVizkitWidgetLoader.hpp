#ifndef QVIZKITWIDGETLOADER_HPP
#define QVIZKITWIDGETLOADER_HPP

#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <vizkit/QVizkitMainWindow.hpp>

/**
 * This class is used to create an instance of QVizkitWidget in ruby.
 */
class QVizkitWidgetLoader : public QObject, public QDesignerCustomWidgetInterface
{
     Q_OBJECT
     Q_INTERFACES(QDesignerCustomWidgetInterface)

 public:
     QVizkitWidgetLoader(QObject *parent = 0);

     bool isContainer() const;
     bool isInitialized() const;
     QIcon icon() const;
     QString domXml() const;
     QString group() const;
     QString includeFile() const;
     QString name() const;
     QString toolTip() const;
     QString whatsThis() const;
     QWidget *createWidget(QWidget *parent);
     void initialize(QDesignerFormEditorInterface *core);

 private:
     bool initialized;
};

#endif // QVIZKITWIDGETLOADER_HPP
