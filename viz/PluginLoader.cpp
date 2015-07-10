#include "../src/Vizkit3DPlugin.hpp"
#include "GridVisualization.hpp"
#include "TextureBoxVisualization.hpp"
#include "ModelVisualization.hpp"

namespace vizkit3d {
    class QtPluginVizkit : public vizkit3d::VizkitPluginFactory {
    private:
    public:
	
	QtPluginVizkit() {
	}
	
	/**
	* Returns a list of all available visualization plugins.
	* @return list of plugin names
	*/
        virtual QStringList* getAvailablePlugins() const
	{
	    QStringList *pluginNames = new QStringList();
	    pluginNames->push_back("GridVisualization");
	    pluginNames->push_back("TextureBoxVisualization");
	    pluginNames->push_back("ModelVisualization");
	    return pluginNames;
	}
	
        virtual QObject* createPlugin(QString const& pluginName)
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
    };
    Q_EXPORT_PLUGIN2(QtPluginVizkit, QtPluginVizkit)
}
