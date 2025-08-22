#pragma once

#include "Vizkit3DPlugin.hpp"
//#include <QMetaObject>
#include <QMetaMethod>

namespace vizkit3d
{

/**
 * @brief allows to set different default settings for plugins than 
 * 
 */
class Vizkit3DPluginDefaultSettingsBase{
 public:

    virtual ~Vizkit3DPluginDefaultSettingsBase(){};
    virtual bool apply(VizPluginBase *plugin) = 0;

};



template <class PLUGIN> class Vizkit3DPluginDefaultSettings : public Vizkit3DPluginDefaultSettingsBase {
 public:

    /**
     * @brief add a confiugation to be added afer laoding the plugin settings->addConfig("setPointSize", Q_ARG(double, pointSize));
     * 
     * @param paramname name of the plugin function to invoke (e.g. "setPointSize")
     * @param param a paramater defien by the Q_ARG macro (e.g. Q_ARG(double, pointSize))
     */
    void addConfig(const std::string& paramname, const QGenericArgument &param) {
        params[paramname] = param;
    }

    bool apply(VizPluginBase *plugin) override {
        PLUGIN* viz = dynamic_cast<PLUGIN*>(plugin);
        if (viz) {
            for (auto& param : params) {
                QMetaObject::invokeMethod(viz, param.first.c_str(), Qt::DirectConnection, param.second);
            }
            return true;
        }
        return false;
    }

 private:
    std::map<std::string, QGenericArgument> params;
};

}
