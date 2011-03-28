#include "QVizkitMainWindow.hpp"
#include <vizkit/MotionCommandVisualization.hpp>
#include <vizkit/TrajectoryVisualization.hpp>
#include <vizkit/WaypointVisualization.hpp>

namespace vizkit {


QVizkitMainWindow::QVizkitMainWindow(QWidget* parent, Qt::WindowFlags flags) 
    : QMainWindow(parent, flags)
{
    vizKitWidget = new vizkit::QVizkitWidget(parent, flags);
    this->setCentralWidget(vizKitWidget);    
    connect(this, SIGNAL(addPlugins()), this, SLOT(addPluginIntern()), Qt::QueuedConnection);
    connect(this, SIGNAL(removePlugins()), this, SLOT(removePluginIntern()), Qt::QueuedConnection);
    pluginNames = new QStringList;
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
 * Creates an instance of a visualization plugin given by its name 
 * and returns the adapter collection of the plugin, used in ruby.
 * @param pluginName Name of the plugin
 * @return Instance of the adapter collection of this plugin
 */
QObject* QVizkitMainWindow::createPlugin(QString pluginName)
{
    vizkit::VizPluginBase* plugin = 0;
    if (pluginName == "WaypointVisualization")
    {
        plugin = new vizkit::WaypointVisualization();
    }
    else if (pluginName == "MotionCommandVisualization")
    {
        plugin = new vizkit::MotionCommandVisualization();
    }
    else if (pluginName == "TrajectoryVisualization")
    {
        plugin = new vizkit::TrajectoryVisualization();
    }

    if (plugin) 
    {
        addPlugin(plugin);
        VizPluginRubyAdapterCollection* adapterCollection = plugin->getRubyAdapterCollection();
        return adapterCollection;
    }
    else {
        std::cerr << "The Pluginname " << pluginName.toStdString() << " is unknown!" << std::endl;
        return NULL;
    }
}

/**
 * Creates an instance of a visualization plugin using its
 * Vizkit Qt Plugin.
 * @param plugin Qt Plugin of the visualization plugin
 * @return Instance of the adapter collection of this plugin
 */
QObject* QVizkitMainWindow::createExternalPlugin(QObject* plugin)
{
    vizkit::VizkitQtPluginBase* qtPlugin = dynamic_cast<vizkit::VizkitQtPluginBase*>(plugin);
    if (qtPlugin) 
    {
        vizkit::VizPluginBase* plugin = qtPlugin->createPlugin();
        addPlugin(plugin);
        return plugin->getRubyAdapterCollection();
    }
    else 
    {
        std::cerr << "The given attribute is no Vizkit Qt Plugin!" << std::endl;
        return NULL;
    }
}

/**
 * Returns a list of all available visualization plugins.
 * @return list of plugin names
 */
QStringList* QVizkitMainWindow::getListOfAvailablePlugins()
{
    if (!pluginNames->size()) 
    {
        pluginNames->push_back("WaypointVisualization");
        pluginNames->push_back("TrajectoryVisualization");
        pluginNames->push_back("MotionCommandVisualization");
    }
    return pluginNames;
}

}