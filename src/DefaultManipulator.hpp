#ifndef VIZKIT3D_DEFAULT_MANIPULATOR_HPP
#define VIZKIT3D_DEFAULT_MANIPULATOR_HPP

#include <osgGA/FirstPersonManipulator>

namespace vizkit3d
{
    /** Vizkit3d's default manipulator
     *
     * The wheel controls forward/backward movement, in the direction of the
     * camera. Left button + mouse movement controls pitch and yaw. Right
     * button + mouse movement pans.
     *
     * The value given to setWheelMovement is the scale factor between the
     * wheel movement and the actual movement in the scene. It defaults to
     * 0.05. There is also a way to force centering the camera on the
     * current mouse pointer position whenever the wheel is used by giving
     * the SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT flag to the constructor. The
     * centering is animated (i.e. not abrupt) and the animation step is
     * controlled by setAnimationTime
     *
     * If vertical axis is fixed (the default), the rotation maintain the
     * "up" vector. Otherwise, they don't. It can be changed with
     * setVerticalAxisFixed.
     */
    class DefaultManipulator : public osgGA::FirstPersonManipulator
    {
    public:
        DefaultManipulator();
        bool performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy );
    };
}

#endif

