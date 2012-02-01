#ifndef QVIZKITMAINWINDOW_H
#define QVIZKITMAINWINDOW_H
#include <QMainWindow>
#include <vizkit/VizPlugin.hpp>
#include <vizkit/Vizkit3DWidget.hpp>
#include <QtDesigner/QDesignerExportWidget>

namespace vizkit {

class QDESIGNER_WIDGET_EXPORT QVizkitMainWindow : public QMainWindow
{
    Q_OBJECT
        
    public:
        QVizkitMainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
        Vizkit3DWidget* getVizkitWidget();
        
    public slots:    
        void addPlugin(QObject* plugin);
        void removePlugin(QObject* plugin);
    private:
        Vizkit3DWidget *vizKitWidget;
};

}
#endif // QVIZKITMAINWINDOW_H
