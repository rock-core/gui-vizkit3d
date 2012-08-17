#ifndef _CONNEXIONHID_H_
#define _CONNEXIONHID_H_

#include <QObject>
#include <QTimer>
#include <osgViewer/View>
#include <vizkit/osgMatrixManipulator.h>

#define LOGITECH_VENDOR_ID          0x046d
#define LOGITECH_SPACE_TRAVELLER_DEVICE_ID 0xc623
#define LOGITECH_SPACE_PILOT_DEVICE_ID     0xc625
#define LOGITECH_SPACE_NAVIGATOR_DEVICE_ID 0xc626
#define LOGITECH_SPACE_EXPLORER_DEVICE_ID  0xc627

namespace vizkit{

  struct connexionValues {
    double tx;
    double ty;
    double tz;
    double rx;
    double ry;
    double rz;
    int button1;
    int button2;
  };

class ConnexionHID : public QObject, public osgGA::MatrixManipulator{
    Q_OBJECT
public:
  enum Mapping{
    TX = 0,
    TY,
    TZ,
    RX,
    RY,
    RZ
  };
  
  ConnexionHID();
  ~ConnexionHID();

  /* Scan all devices in /dev/input/ to find the SpaceMouse.
   * Returns the true if an SpaceMouse could be found.
   */
  bool init(osgGA::CameraManipulator *manipulator);

  /**
   * Returns the file Decriptor, -1 if no valid FD is there
   */
  int getFileDescriptor();

  /** 
   * Refrence getter for axis scalinig, modifications are allowed via this reference member
   */
  double& axisScalinig(Mapping id);

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
  void getValue(double *coordiantes, struct connexionValues *rawValues);
  void closeConnexionHID();
  int fd;
  osg::ref_ptr<osgGA::CameraManipulator> manipulator;
  QTimer timer;
  osg::Matrixd matrix;
  double scale[6];

public slots:
    /**
     * In this method is the real handling for the mouse values
     * this method need to called periodicly, this is normally done
     * via an QT-Timer which is initialized in the init() function
     */
    void handleMouse();
    
};

#endif

};
