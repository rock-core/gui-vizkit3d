#include "ConnexionPlugin.h"
#include <iostream>

namespace vizkit3d{


ConnexionPlugin::ConnexionPlugin(){
    //Different Values for this Plugin
    scale[RX] = 0.02;
    scale[RY] = 0.02;
    scale[RZ] = 0.02;
    scale[TX] = 0.3;
    scale[TY] = 0.3;
    scale[TZ] = 0.3;
    matrix = osg::Matrix::rotate(osg::Quat(
                M_PI/2.0, osg::Vec3f(1,0,0), 
                0.0f, osg::Vec3f(0,1,0), 
                -M_PI/2.0, osg::Vec3f(0,0,1))) * 
        osg::Matrix::translate(-4,0,1);


}

ConnexionPlugin::~ConnexionPlugin(){
}

bool ConnexionPlugin::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us){
    return handleMouse();
}

bool ConnexionPlugin::init(){
  if(!ConnexionHID::init()){
      std::cerr << "Cannot open mouse" << std::endl;
      return false;
  }
  return true;
}

bool ConnexionPlugin::handleMouse(){
  if(getFileDescriptor() < 0) return false;
  controldev::connexionValues motion;

  //Maybe in future the buttons could also be handelt, currently not used but already requested
  controldev::connexionValues newValues;

  //Getting actual readings
  getValue(motion, newValues);

  //Save processing power if mouse is not moved
  if(motion.rx == 0.0 && motion.ry == 0.0 && motion.rz == 0.0 && motion.tx == 0.0 && motion.ty == 0.0 && motion.tz == 0.0) return false;

  //Create Quaternion from current Camera Orientation
  osg::Quat q;
  q.set(matrix);
  
  //Create vector from Translation request
  osg::Vec3f translation(motion.tx,motion.tz,-motion.ty);
  //Rotation translation Request into current Camra frame
  translation = q * translation;
  //Apply Translation request to current Camera matrix
  matrix *= osg::Matrix::translate(translation);
  //Create Quaternion from Rotation request
  q.makeRotate(motion.rx,osg::Vec3f(1,0,0), -motion.ry, osg::Vec3f(0,0,1), motion.rz, osg::Vec3f(0,1,0));
  //Apply Rotation Request to current Camera matrix (after translation!)
  matrix = osg::Matrix::rotate(q) * matrix;
  return true;
}

}
