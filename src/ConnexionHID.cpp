#include "ConnexionHID.h"

#include <linux/input.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <Eigen/Geometry>
#include <base/logging.h>

#define PATH_BUFFER_SIZE (1024)

namespace vizkit{


ConnexionHID::ConnexionHID(){
    scale[RX] = 5000.0;
    scale[RY] = 5000.0;
    scale[RZ] = 5000.0;
    scale[TX] = 10.0;
    scale[TY] = 10.0;
    scale[TZ] = 10.0;
    matrix.makeIdentity();
}

ConnexionHID::~ConnexionHID(){
    closeConnexionHID();
}

double& ConnexionHID::axisScalinig(Mapping id){
    return scale[id];
}

bool ConnexionHID::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us){
    //Do Nothing, otherwise the screen would be resettet, but if needed in future here could be an addtional mouse handler registered
    return false;
}

int ConnexionHID::getFileDescriptor() {
    return fd;
}

bool ConnexionHID::init(osgGA::CameraManipulator *manipulator){
  this->manipulator = manipulator;
  if(!manipulator)
      return false;

  struct dirent *entry;
  DIR *dp;
  char path[PATH_BUFFER_SIZE];
  struct input_id device_info;
  const char *devDirectory = "/dev/input/";

  /* open the directory */
  dp = opendir(devDirectory);
  if(dp == NULL) {
    return false;
  }
  /* walk the directory (non-recursively) */
  while((entry = readdir(dp))) {
    strncpy(path, devDirectory, sizeof(path));
    /* if strlen(devDirectory) > sizeof(path) the path won't be NULL terminated
     * and *bad things* will happen. Therfore, we force NULL termination.
     */
    path[PATH_BUFFER_SIZE-1] = '\0';
    strncat(path, entry->d_name, sizeof(path));

    fd = open(path, O_RDONLY | O_NONBLOCK);
    if(-1 == fd) {
      /* could not open file. probably we do not have read permission. */
      continue;
    }

    /* try to read the vendor and device ID */
    if(!ioctl(fd, EVIOCGID, &device_info)) {
      
      LOG_DEBUG("Spacemouse: vendor %04hx product %04hx version %04hx\n",
              device_info.vendor, device_info.product, device_info.version);
      if((device_info.vendor == LOGITECH_VENDOR_ID) && 
         (device_info.product == LOGITECH_SPACE_NAVIGATOR_DEVICE_ID)) {
        /* BINGO!!! this is it! */
        break;
      }
    }
    close(fd);
    fd = -1;
  }

  if(-1 == fd) {
    LOG_WARN("could not find SpaceMouse! \nDo you have read permission on the /dev/input/ device?\n");
    return false;
  }
  closedir(dp);
  timer.setSingleShot(false);
  timer.setInterval(20);
  manipulator->setByMatrix(osg::Matrixd::identity());
  timer.start();
  connect(&timer,SIGNAL(timeout()), this, SLOT(handleMouse()));
  return true;
}

void ConnexionHID::closeConnexionHID() {
  if(fd > 0) {
    close(fd);
  }
  fd = -1;
}

void ConnexionHID::getValue(double *coordinates, struct connexionValues *rawValues) {
  /* If input events don't come in fast enough a certain DoF may not be 
   * updated during a frame. This results in choppy and ugly animation.
   * To solve this we record the number of frames a certain DoF was idle
   * and only set the DoF to 0 if we reach a certain idleThreshold.
   * When there is activity on a axis the idleFrameCount is reset to 0.
   */
  int i, eventCnt;
  /* how many bytes were read */
  size_t bytesRead;
  /* the events (up to 64 at once) */
  struct input_event events[64];
  /* keep track of idle frames for each DoF for smoother animation. see above */
  static int idleFrameCount[6] = {0, 0, 0, 0, 0, 0};
  int idleThreshold = 3;

  /* read the raw event data from the device */
  bytesRead = read(fd, events, sizeof(struct input_event) * 64);
  eventCnt = (int) ((long)bytesRead / (long)sizeof(struct input_event));
  if (bytesRead < (int) sizeof(struct input_event)) {
    perror("evtest: short read");
    return;
  }

  /* Increase all idle counts. They are later reset if there is activity */
  for(i = 0; i < 6; ++i) {
    ++idleFrameCount[i];
  }

  /* handle input events sequentially */
  for(i = 0; i < eventCnt; ++i) {
    if(EV_KEY == events[i].type) {
      switch(events[i].code) {
      case BTN_0:
        rawValues->button1 = events[i].value;
        break;
      case BTN_1:
        rawValues->button2 = events[i].value;
        break;
      }
    } else if(EV_REL == events[i].type || EV_ABS == events[i].type) {
      switch(events[i].code) {
      //case ABS_X: //Same value as REL_* so because of the check above, this is not needed
      case REL_X:
        rawValues->tx = events[i].value;
        idleFrameCount[0] = 0;
        break;
      //case ABS_Y:
      case REL_Y:
        rawValues->ty = events[i].value;
        idleFrameCount[1] = 0;
        break;
      //case ABS_Z:
      case REL_Z:
        rawValues->tz = events[i].value;
        idleFrameCount[2] = 0;
        break;
      //case ABS_RX:
      case REL_RX:
        rawValues->rx = events[i].value;
        idleFrameCount[3] = 0;
        break;
      //case ABS_RY:
      case REL_RY:
        rawValues->ry = events[i].value;
        idleFrameCount[4] = 0;
        break;
      //case ABS_RZ:
      case REL_RZ:
        rawValues->rz = events[i].value;
        idleFrameCount[5] = 0;
        break;
      }
    }
  }

  /* Set rawValue to zero if DoF was idle for more than idleThreshold frames */
  for(i = 0; i < 6; ++i) {
    if(idleFrameCount[i] >= idleThreshold) {
      if(0==i) {
        rawValues->tx = 0;
      } else if (1==i) {
        rawValues->ty = 0;
      } else if (2==i) {
        rawValues->tz = 0;
      } else if (3==i) {
        rawValues->rx = 0;
      } else if (4==i) {
        rawValues->ry = 0;
      } else if (5==i) {
        rawValues->rz = 0;
      }
    }
  }

  coordinates[0] = rawValues->tx * fabs(rawValues->tx * 0.001);
  coordinates[1] = -rawValues->tz * fabs(rawValues->tz * 0.001);
  coordinates[2] = -rawValues->ty * fabs(rawValues->ty * 0.001);
  coordinates[3] = rawValues->rx * fabs(rawValues->rx * 0.0008);
  coordinates[4] = -rawValues->rz * fabs(rawValues->rz * 0.0008);
  coordinates[5] = -rawValues->ry * fabs(rawValues->ry * 0.0008);
}

void ConnexionHID::handleMouse(){
  double motion[6];

  //Maybe in future the buttons could also be handelt, currently not used but already requested
  connexionValues newValues;

  //Getting actual readings
  getValue(motion, &newValues);

  bool set = false;
  for(int i=0;i<6;i++){
      if(fabs(motion[i])>0.0)
          set = true;
      motion[i] /= scale[i];
  }
  //Save processing power if mouse is not moved
  if(!set) return;

  //Get current Camera matrix
  osg::Matrixd m = manipulator->getMatrix();
  
  //Create Quaternion from current Camera Orientation
  osg::Quat q;
  q.set(m);
  Eigen::Quaterniond qu(q.w(),q.x(),q.y(),q.z());

  //Create Quaternion from Rotation request
  Eigen::Quaterniond q2 = 
        Eigen::AngleAxisd(motion[RX],Eigen::Vector3d::UnitX()) *
        Eigen::AngleAxisd(motion[RY],Eigen::Vector3d::UnitY()) *
        Eigen::AngleAxisd(-motion[RZ],Eigen::Vector3d::UnitZ());

  //Be sure that the rotation does not include an scake
  qu.normalize();

  //Create vector from Translation request
  Eigen::Vector3d v(motion[TX],motion[TY],-motion[TZ]);

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
