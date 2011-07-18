#ifndef VIZKIT_OSG_MATRIXMANIPULATOR_HH
#define VIZKIT_OSG_MATRIXMANIPULATOR_HH

#include <osg/Version>

#if defined(OSG_MIN_VERSION_REQUIRED)
#if OSG_MIN_VERSION_REQUIRED(3,0,0)
#include <osgGA/CameraManipulator>
namespace osgGA {
    typedef CameraManipulator MatrixManipulator;
}
#else
#include <osgGA/MatrixManipulator>
#endif
#else
#include <osgGA/MatrixManipulator>
#endif
#endif

