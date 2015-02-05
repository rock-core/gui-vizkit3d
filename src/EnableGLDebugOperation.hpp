#ifndef VIZKIT3D_ENABLE_OSGDEBUG_OPERATION_HPP
#define VIZKIT3D_ENABLE_OSGDEBUG_OPERATION_HPP

/** This code exists in various forms on the web
 *
 * This one has been shamelessly taken from
 *   https://github.com/ThermalPixel/osgdemos
 */

#include <osgViewer/ViewerEventHandlers>

namespace vizkit3d
{
    void enableGLDebugExtension(int contextid);

    class EnableGLDebugOperation : public osg::GraphicsOperation
    {
    public:
        EnableGLDebugOperation()
            : osg::GraphicsOperation("EnableGLDebugOperation", false) {
        }
        virtual void operator ()(osg::GraphicsContext* gc) {
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
            int context_id = gc->getState()->getContextID();
            enableGLDebugExtension(context_id);
        }
        OpenThreads::Mutex _mutex;
    };
}

#endif
