#include "PluginLoader.hpp"
#include "GridVisualization.hpp"
#include "TextureBoxVisualization.hpp"
#include "ModelVisualization.hpp"

using namespace vizkit3d;
QtPluginVizkit::QtPluginVizkit() { }

QStringList* QtPluginVizkit::getAvailablePlugins() const
{
    QStringList *pluginNames = new QStringList();
    pluginNames->push_back("GridVisualization");
    pluginNames->push_back("TextureBoxVisualization");
    pluginNames->push_back("ModelVisualization");
    return pluginNames;
}

QObject* QtPluginVizkit::createPlugin(QString const& pluginName)
{
    vizkit3d::VizPluginBase* plugin = 0;
    if (pluginName == "GridVisualization")
	plugin = new vizkit3d::GridVisualization();
    else if (pluginName == "TextureBoxVisualization")
	plugin = new vizkit3d::TextureBoxVisualization();
    else if (pluginName == "ModelVisualization")
	plugin = new vizkit3d::ModelVisualization();
    return plugin;
};
