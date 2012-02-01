#include <vizkit/VizPlugin.hpp>
#include "LaserScanVisualization.hpp"
#include "WaypointVisualization.hpp"
#include "MotionCommandVisualization.hpp"
#include "TrajectoryVisualization.hpp"
#include "RigidBodyStateVisualization.hpp"

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
	    pluginNames->push_back("WaypointVisualization");
	    pluginNames->push_back("TrajectoryVisualization");
	    pluginNames->push_back("MotionCommandVisualization");
	    pluginNames->push_back("RigidBodyStateVisualization");
	    pluginNames->push_back("LaserScanVisualization");
	    return pluginNames;
	}
	
        virtual QObject* createPlugin(QString const& pluginName)
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
	    else if (pluginName == "RigidBodyStateVisualization")
	    {
		plugin = new vizkit::RigidBodyStateVisualization();
	    }
	    else if (pluginName == "LaserScanVisualization")
	    {
	        plugin = new vizkit::LaserScanVisualization();
	    }

	    if (plugin) 
	    {
		return plugin;
	    }
	    return NULL;
        };
    };
    Q_EXPORT_PLUGIN2(QtPluginVizkit, QtPluginVizkit)
}
