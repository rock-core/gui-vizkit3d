#include "../src/Vizkit3DPlugin.hpp"
#include "GridVisualization.hpp"

namespace vizkit {
    class QtPluginVizkit : public vizkit::VizkitPluginFactory {
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
	    return pluginNames;
	}
	
        virtual QObject* createPlugin(QString const& pluginName)
        {
	    vizkit::VizPluginBase* plugin = 0;
	    if (pluginName == "GridVisualization")
	        plugin = new vizkit::GridVisualization();

	    return plugin;
        };
    };
    Q_EXPORT_PLUGIN2(QtPluginVizkit, QtPluginVizkit)
}
