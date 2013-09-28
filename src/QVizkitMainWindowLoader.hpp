#ifndef QVIZKITMAINWINDOWLOADER_HPP
#define QVIZKITMAINWINDOWLOADER_HPP

#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <vizkit/QVizkitMainWindow.hpp>

/**
 * This class is used to create an instance of QVizkitWidget in ruby.
 */
class QVizkitMainWindowLoader : public QObject, public QDesignerCustomWidgetInterface
{
     Q_OBJECT
     Q_INTERFACES(QDesignerCustomWidgetInterface)

 public:
     QVizkitMainWindowLoader(QObject *parent = 0);

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

#endif // QVIZKITMAINWINDOWLOADER_HPP
