#ifndef QVIZKITMAINWINDOW_H
#define QVIZKITMAINWINDOW_H

#ifndef Q_MOC_RUN
    #include "Vizkit3DPlugin.hpp"
    #include "Vizkit3DWidget.hpp"
#endif


#include <QMainWindow>
#if QT_VERSION < 0x050000
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

namespace vizkit3d {

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
