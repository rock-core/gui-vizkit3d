#include <vizkit3d/DefaultManipulator.hpp>

using namespace vizkit3d;

DefaultManipulator::DefaultManipulator()
{
    setWheelMovement(1.0, false);
}

// doc in parent
bool DefaultManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
    // pan model
    float scale = 10.0f;

    moveRight(-scale * dx);
    moveUp(scale * dy);
    return true;
}

