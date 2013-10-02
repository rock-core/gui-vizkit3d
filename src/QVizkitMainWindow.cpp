#include "QVizkitMainWindow.hpp"

namespace vizkit3d {


QVizkitMainWindow::QVizkitMainWindow(QWidget* parent, Qt::WindowFlags flags) 
    : QMainWindow(parent, flags)
{
    vizKitWidget = new vizkit3d::Vizkit3DWidget(parent);
    this->setCentralWidget(vizKitWidget);
}

/**
 * Adds the plugin to the vizkit3d widget and
 * adds its qdockwidgets to the right side of the 
 * main window.
 * @param plugin Vizkit Plugin
 */
void QVizkitMainWindow::addPlugin(QObject* plugin)
{
    vizKitWidget->addPlugin(plugin);
    vizkit3d::VizPluginBase* viz_plugin = dynamic_cast<vizkit3d::VizPluginBase*>(plugin);
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
 * Removes the plugin from the vizkit3d widget and
 * removes its qdockwidgets from the main window.
 * @param plugin Vizkit Plugin
 */
void QVizkitMainWindow::removePlugin(QObject* plugin)
{
    vizKitWidget->removePlugin(plugin);
    vizkit3d::VizPluginBase* viz_plugin = dynamic_cast<vizkit3d::VizPluginBase*>(plugin);
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
}
