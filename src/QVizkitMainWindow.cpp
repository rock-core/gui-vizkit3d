#include "QVizkitMainWindow.hpp"

namespace vizkit {


QVizkitMainWindow::QVizkitMainWindow(QWidget* parent, Qt::WindowFlags flags) 
    : QMainWindow(parent, flags)
{
    vizKitWidget = new vizkit::Vizkit3DWidget(parent, flags);
    this->setCentralWidget(vizKitWidget);
}

/**
 * Adds the plugin to the vizkit widget and
 * adds its qdockwidgets to the right side of the 
 * main window.
 * @param plugin Vizkit Plugin
 */
void QVizkitMainWindow::addPlugin(QObject* plugin)
{
    vizKitWidget->addPlugin(plugin);
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    if (viz_plugin)
    {
        std::vector<QDockWidget*> dockWidgets = viz_plugin->getDockWidgets();
        for(std::vector<QDockWidget*>::iterator dockit = dockWidgets.begin(); dockit != dockWidgets.end(); dockit++)
        {
            this->addDockWidget(Qt::RightDockWidgetArea, *dockit);
        }
    }
}

/**
 * Removes the plugin from the vizkit widget and
 * removes its qdockwidgets from the main window.
 * @param plugin Vizkit Plugin
 */
void QVizkitMainWindow::removePlugin(QObject* plugin)
{
    vizKitWidget->removePlugin(plugin);
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    if (viz_plugin)
    {
        std::vector<QDockWidget*> dockWidgets = viz_plugin->getDockWidgets();
        for(std::vector<QDockWidget*>::iterator dockit = dockWidgets.begin(); dockit != dockWidgets.end(); dockit++)
        {
            this->removeDockWidget(*dockit);
        }
    }
}

/**
 * @return Instance of the OSG Widget
 */
Vizkit3DWidget* QVizkitMainWindow::getVizkitWidget()
{
    return vizKitWidget;
}

/**
 * Creates an instance of a visualization plugin given by its name 
 * and returns the adapter collection of the plugin, used in ruby.
 * @param pluginName Name of the plugin
 * @return Instance of the vizkit plugin
 */
QObject* QVizkitMainWindow::createPluginByName(QString pluginName)
{
    return vizKitWidget->createPluginByName(pluginName);
}

/**
 * Returns a list of all available visualization plugins.
 * @return list of plugin names
 */
QStringList* QVizkitMainWindow::getListOfAvailablePlugins()
{
    return vizKitWidget->getListOfAvailablePlugins();
}

}
