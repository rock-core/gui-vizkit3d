#include "ConnexionPlugin.h"

#include <Eigen/Geometry>

namespace vizkit{


ConnexionPlugin::ConnexionPlugin(){
    //Different Values for this Plugin
    scale[RX] = 5000.0;
    scale[RY] = 5000.0;
    scale[RZ] = 5000.0;
    scale[TX] = 10.0;
    scale[TY] = 10.0;
    scale[TZ] = 10.0;
    matrix.makeIdentity();
}

ConnexionPlugin::~ConnexionPlugin(){
}

bool ConnexionPlugin::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us){
    //Do Nothing, otherwise the screen would be resettet, but if needed in future here could be an addtional mouse handler registered
    return false;
}

bool ConnexionPlugin::init(osgGA::MatrixManipulator *manipulator){
  this->manipulator = manipulator;
  if(!manipulator)
      return false;

  if(!ConnexionHID::init()) return false;

  timer.setSingleShot(false);
  timer.setInterval(20);
  manipulator->setByMatrix(osg::Matrixd::identity());
  timer.start();
  connect(&timer,SIGNAL(timeout()), this, SLOT(handleMouse()));
  return true;
}

void ConnexionPlugin::handleMouse(){
    controldev::connexionValues motion;

  //Maybe in future the buttons could also be handelt, currently not used but already requested
    controldev::connexionValues newValues;

  //Getting actual readings
  getValue(motion, newValues);

  //Save processing power if mouse is not moved
  if(motion.rx == 0.0 && motion.ry == 0.0 && motion.rz == 0.0 && motion.tx == 0.0 && motion.ty == 0.0 && motion.tz == 0.0) return;

  //Get current Camera matrix
  osg::Matrixd m = manipulator->getMatrix();
  
  //Create Quaternion from current Camera Orientation
  osg::Quat q;
  q.set(m);
  Eigen::Quaterniond qu(q.w(),q.x(),q.y(),q.z());

  //Create Quaternion from Rotation request
  Eigen::Quaterniond q2 = 
        Eigen::AngleAxisd(motion.rx,Eigen::Vector3d::UnitX()) *
        Eigen::AngleAxisd(motion.ry,Eigen::Vector3d::UnitY()) *
        Eigen::AngleAxisd(-motion.rz,Eigen::Vector3d::UnitZ());

  //Be sure that the rotation does not include an scake
  qu.normalize();

  //Create vector from Translation request
  Eigen::Vector3d v(motion.tx,motion.ty,-motion.tz);

  //Rotation translation Request into current Camra frame
  v = qu * v;

  //Apply Translation request to current Camera matrix
  m *= osg::Matrix::translate(v[0],v[1],v[2]);

  //Apply Rotation Request to current Camera matrix (after translation!)
  m = osg::Matrix::rotate(osg::Quat(q2.x(),q2.y(),q2.z(),q2.w())) * m;

  //Apply matrix to camera(-maipulator)
  manipulator->setByMatrix(m);

  //LOG_DEBUG("Got called! %f,%f,%f, %f, %f, %f\n",v[0],v[1],v[2],rx,ry,rz);
}

}
