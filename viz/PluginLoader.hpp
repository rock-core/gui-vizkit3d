#ifndef VIZKIT3D_VIZKIT_PLUGIN_FACTORY
#define VIZKIT3D_VIZKIT_PLUGIN_FACTORY

#include "../src/Vizkit3DPlugin.hpp"

namespace vizkit3d {
    class QtPluginVizkit : public vizkit3d::VizkitPluginFactory {
      Q_OBJECT
      Q_PLUGIN_METADATA(IID "rock.vizkit3d.QtPluginVizkit")
    private:
    public:
	
	QtPluginVizkit();
	
	/**
	* Returns a list of all available visualization plugins.
	* @return list of plugin names
	*/
        virtual QStringList* getAvailablePlugins() const;
        virtual QObject* createPlugin(QString const& pluginName);
    };
}

#endif
