#ifndef _CONNEXIONPLUGIN_H_
#define _CONNEXIONPLUGIN_H_

#include <QObject>
#include <QTimer>
#include <controldev/ConnexionHID.hpp>
#include <vizkit3d/osgMatrixManipulator.h>

namespace vizkit3d{


class ConnexionPlugin : public QObject, public controldev::ConnexionHID,  public osgGA::MatrixManipulator{
    Q_OBJECT
public:
  
    ConnexionPlugin();
    virtual ~ConnexionPlugin();

  /* Scan all devices in /dev/input/ to find the SpaceMouse.
   * Returns the true if an SpaceMouse could be found.
   */
  bool init();

  /**
   * Needed functions for osgGA::MatrixManipulator
   */
  virtual osg::Matrixd getInverseMatrix() const {
      return osg::Matrixd::inverse(matrix);
  };

  /**
   * Needed functions for osgGA::MatrixManipulator
   */
  virtual osg::Matrixd getMatrix() const {
      return matrix;
  }

  /**
   * Needed functions for osgGA::MatrixManipulator
   */
  virtual void setByInverseMatrix(const osg::Matrixd& matrix) {
      this->matrix = osg::Matrixd::inverse(matrix);
  };

  /**
   * Needed functions for osgGA::MatrixManipulator
   */
  virtual void setByMatrix(const osg::Matrixd& matrix) {
      this->matrix = matrix;
  };    

  /**
   * Needed functions for osgGA::MatrixManipulator
   * Do Nothing, otherwise the screen would be resettet, but if needed in future here could be an addtional mouse handler registered
   */
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

protected:
  osg::ref_ptr<osgGA::MatrixManipulator> manipulator;
  QTimer timer;
  osg::Matrixd matrix;

public slots:
    /**
     * In this method is the real handling for the mouse values
     * this method need to called periodicly, this is normally done
     * via an QT-Timer which is initialized in the init() function
     */
    bool handleMouse();
    
};

#endif

};
