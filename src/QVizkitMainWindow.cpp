#include "QVizkitMainWindow.hpp"

namespace vizkit {


QVizkitMainWindow::QVizkitMainWindow(QWidget* parent, Qt::WindowFlags flags) 
    : QMainWindow(parent, flags)
{
    vizKitWidget = new vizkit::QVizkitWidget(parent, flags);
    this->setCentralWidget(vizKitWidget);    
    connect(this, SIGNAL(addPlugins()), this, SLOT(addPluginIntern()), Qt::QueuedConnection);
    connect(this, SIGNAL(removePlugins()), this, SLOT(removePluginIntern()), Qt::QueuedConnection);
}

/**
 * Puts the plugin in a list and emits a signal.
 * Adding the new Plugin will be handled by the main thread.
 * @param plugin Vizkit Plugin
 */
void QVizkitMainWindow::addPlugin(vizkit::VizPluginBase* plugin)
{
    if (plugin)
    {
        pluginsToAdd.push_back(plugin);
        emit addPlugins();
    }
}

/**
 * Puts the plugin in a list and emits a signal.
 * Removing the new Plugin will be handled by the main thread.
 * @param plugin Vizkit Plugin
 */
void QVizkitMainWindow::removePlugin(VizPluginBase* plugin)
{
    if (plugin)
    {
        pluginsToRemove.push_back(plugin);
        emit removePlugins();
    }
}

/**
 * This slot adds all plugins in the list to the OSG and
 * their QDockWidgets to the QMainWindow.
 */
void QVizkitMainWindow::addPluginIntern()
{
    for(std::vector<vizkit::VizPluginBase*>::iterator pluginIt = pluginsToAdd.begin(); pluginIt != pluginsToAdd.end(); pluginIt++)
    {
        vizKitWidget->addDataHandler(*pluginIt);
        std::vector<QDockWidget*> dockWidgets = (*pluginIt)->getDockWidgets();
        for(std::vector<QDockWidget*>::iterator dockit = dockWidgets.begin(); dockit != dockWidgets.end(); dockit++)
        {
            this->addDockWidget(Qt::LeftDockWidgetArea, *dockit);
        }
    }
    pluginsToAdd.clear();
}

/**
 * This slot removes all plugins in the list from the OSG and
 * their QDockWidgets from the QMainWindow.
 */
void QVizkitMainWindow::removePluginIntern()
{
    for(std::vector<vizkit::VizPluginBase*>::iterator pluginIt = pluginsToRemove.begin(); pluginIt != pluginsToRemove.end(); pluginIt++)
    {
        std::vector<QDockWidget*> dockWidgets = (*pluginIt)->getDockWidgets();
        for(std::vector<QDockWidget*>::iterator dockit = dockWidgets.begin(); dockit != dockWidgets.end(); dockit++)
        {
            this->removeDockWidget(*dockit);
        }
        vizKitWidget->removeDataHandler(*pluginIt);
    }
    pluginsToRemove.clear();
}

/**
 * @return Instance of the OSG Widget
 */
QVizkitWidget* QVizkitMainWindow::getVizkitWidget()
{
    return vizKitWidget;
}

/**
 * Creates an instance of a visualization plugin using its
 * Vizkit Qt Plugin.
 * @param plugin Qt Plugin of the visualization plugin
 * @return Instance of the adapter collection of this plugin
 */
QObject* QVizkitMainWindow::createExternalPlugin(QObject* plugin, QString const& name)
{
    return vizKitWidget->createExternalPlugin(plugin, name);
}

/**
 * Creates an instance of a visualization plugin given by its name 
 * and returns the adapter collection of the plugin, used in ruby.
 * @param pluginName Name of the plugin
 * @return Instance of the adapter collection of this plugin
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
