#ifndef VIZKIT3D_ENV_PLUGIN_BASE_HPP
#define VIZKIT3D_ENV_PLUGIN_BASE_HPP

#include <vizkit3d/Vizkit3DPlugin.hpp>

namespace vizkit3d
{
    /** Superclass for plugins that can be used as environment plugins
     *
     * Environment plugins are plugins that render special environmental
     * effects that should be applied to the whole scene such as e.g. fog or
     * water
     */
    class EnvPluginBase : public VizPluginBase
    {
    public:
        EnvPluginBase(QObject* parent = NULL)
            : VizPluginBase(parent) {}

        /** Returns the node onto which the whole scene should be attached
         *
         * It defaults to getRootNode()
         */
        virtual osg::ref_ptr<osg::Group> getRefNode() { return getRootNode(); }
    };
}

#endif
