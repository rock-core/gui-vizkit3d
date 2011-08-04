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
        void addPlugin(vizkit::VizPluginBase *plugin);
        void removePlugin(vizkit::VizPluginBase* plugin);
        Vizkit3DWidget* getVizkitWidget();
        
    public slots:    
        QObject* createExternalPlugin(QObject* plugin, QString const& name);
        QStringList* getListOfAvailablePlugins();
        QObject* createPluginByName(QString pluginName);
        
    signals:
        void addPlugins();
        void removePlugins();
        
    private slots:
        void addPluginIntern();
        void removePluginIntern();
	
    private:
        std::vector<vizkit::VizPluginBase *> pluginsToAdd;
        std::vector<vizkit::VizPluginBase *> pluginsToRemove;
	Vizkit3DWidget *vizKitWidget;
};

}
#endif // QVIZKITMAINWINDOW_H
