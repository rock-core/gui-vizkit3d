#ifndef QVIZKITMAINWINDOW_H
#define QVIZKITMAINWINDOW_H
#include <QMainWindow>
#include <vizkit/VizPlugin.hpp>
#include <vizkit/QVizkitWidget.hpp>
#include <QtDesigner/QDesignerExportWidget>

namespace vizkit {

class QDESIGNER_WIDGET_EXPORT QVizkitMainWindow : public QMainWindow
{
    Q_OBJECT
        
    public:
	QVizkitMainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
        void addPlugin(vizkit::VizPluginBase *plugin);
        void removePlugin(vizkit::VizPluginBase* plugin);
        QVizkitWidget* getVizkitWidget();
        
    public slots:    
        QStringList* getListOfAvailablePlugins();
        QObject* createPlugin(QString pluginName);
        QObject* createExternalPlugin(QObject* plugin);
        
    signals:
        void addPlugins();
        void removePlugins();
        
    private slots:
        void addPluginIntern();
        void removePluginIntern();
	
    private:
        std::vector<vizkit::VizPluginBase *> pluginsToAdd;
        std::vector<vizkit::VizPluginBase *> pluginsToRemove;
        QStringList* pluginNames;
	QVizkitWidget *vizKitWidget;
};

}
#endif // QVIZKITMAINWINDOW_H
