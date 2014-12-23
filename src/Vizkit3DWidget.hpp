#ifndef __VIZKIT_QVIZKITWIDGET__
#define __VIZKIT_QVIZKITWIDGET__

#include "Vizkit3DPlugin.hpp"
#include <osgViewer/CompositeViewer>

#include <QtDesigner/QDesignerExportWidget>
#include <QVector3D>
#include <QTimer>

#include <osgGA/CameraManipulator>

namespace osgQt { class GraphicsWindowQt;}
namespace vizkit3d
{
    class Vizkit3DWidget;

    /** The list of available camera manipulators
     *
     * IMPORTANT: if you change this, you MUST edit the KNOWN_MANIPULATORS
     *            array in Vizkit3DWidget.cpp as well !
     */
    enum CAMERA_MANIPULATORS
    {
        /**
         * Wheel controls forward/backward movement, in the direction of the
         * camera. Left button + mouse movement controls pitch and yaw.
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
        FIRST_PERSON_MANIPULATOR,
        /** Behaves like a flight simulator controlled by a mouse. The position
         * of the mouse w.r.t. the center of the window causes pitch/roll. By
         * default (can be turned off), yaw is also changed if the camera banked
         * (has non-zero roll).
         *
         * In addition, the camera has a velocity and moves continuously if this
         * velocity is non-zero. The left mouse button causes the camera's
         * velocity to grow, the right mouse to decrease and the middle mouse
         * button sets it to zero.
         */
        FLIGHT_MANIPULATOR,
        /** Transforms the camera as if it was "orbiting" a point of reference
         * (the camera center).
         *
         * The mouse wheel applies a scale factor on the distance between the
         * camera eye and the center, as 1.0 + wheelZoomFactor (regardless of
         * the actual wheel movement). The default wheelZoomFactor is 0.1. If
         * this distance becomes smaller than a minimum distance (defaults to
         * 0.05), the center gets "pushed" forward so that the minimum distance
         * is kept. In practice, it means that the wheel movement is huge when
         * far from the center and tiny when close.
         *
         * Moving the mouse with the left mouse button pressed does a trackball
         * rotation, i.e. makes the camera move "as if" the mouse moved a sphere
         * centered on the center point. By default, the orbit manipulator
         * constrains the vertical axis to keep within the same plane (i.e.
         * vertical is always vertical)
         *
         * Moving the mouse with the right mouse button pressed also applies a
         * zoom (following the same rules). The scale factor applied is the
         * movement in Y, 1.0 seems to be the whole height of the window.
         *
         * Moving the mouse with the middle mouse button pressed applies a panning
         * movement. The movement is displacement * 0.3 * distance_to_center
         */
        ORBIT_MANIPULATOR,
        /** Like an orbit manipulator, with the middle-button behaviour changed
         *
         * If the mouse is moved while the middle button is pressed, X movement
         * moves the center in the camera's side direction and Y movement
         * moves it in the forward direction. The movement in each case is 0.3 *
         * distance_to_center * distance_moved. In addition, if a node is
         * associated with the camera, the center will "track" the node's shape,
         * i.e. an intersection is computed and the distance-to-node kept.
         */
        TERRAIN_MANIPULATOR,
        /** Just like an orbit manipulator, but with the fixed-vertical-axis
         * constraint set to false by default
         */
        TRACKBALL_MANIPULATOR,
        /** Like the trackball manipulator, but reacts to pinch-to-zoom vs. drag
         * multitouch events.
         */
        MULTI_TOUCH_TRACKBALL_MANIPULATOR
    };

    // configuration class
    class Vizkit3DConfig :public QObject
    {
        Q_OBJECT

        private:
            Q_PROPERTY( bool axes READ isAxes WRITE setAxes)
            Q_PROPERTY( bool axes_labels READ isAxesLabels WRITE setAxesLabels)
            Q_PROPERTY( QColor background READ getBackgroundColor WRITE setBackgroundColor)
            Q_PROPERTY( QStringList frame READ getVisualizationFrames WRITE setVisualizationFrame)
            Q_PROPERTY( bool transformer READ isTransformer WRITE setTransformer)
            Q_ENUMS( CAMERA_MANIPULATORS )
            Q_PROPERTY( QStringList manipulator READ getAvailableCameraManipulators WRITE setCameraManipulator )

        public:
            Vizkit3DConfig(Vizkit3DWidget *parent);
            Vizkit3DWidget* getWidget() const;

        signals:
            void propertyChanged(QString);

        public slots:
            bool isAxes() const;
            void setAxes(bool value);

            bool isAxesLabels() const;
            void setAxesLabels(bool value);
            QStringList getVisualizationFrames() const;
            void setVisualizationFrame(const QStringList &frames);

            bool isTransformer() const;
            void setTransformer(bool value);

            QColor getBackgroundColor()const;
            void setBackgroundColor(QColor color);

            /** Converts a manipulator ID to its name */
            static QString manipulatorIDToName(CAMERA_MANIPULATORS id);
            /** Converts a manipulator name to its ID */
            static CAMERA_MANIPULATORS manipulatorNameToID(QString const& name);
            
            /** Sets the current camera manipulator among those available */
            void setCameraManipulator(QStringList const& manipulators);
            /** Returns the list of available camera manipulators, with the
             * current one at the top
             */
            QStringList getAvailableCameraManipulators() const;
    };

    class QDESIGNER_WIDGET_EXPORT Vizkit3DWidget : public QWidget, public osgViewer::CompositeViewer
    {
        Q_OBJECT
        public:
            static osg::Vec3d const DEFAULT_EYE;
            static osg::Vec3d const DEFAULT_CENTER;
            static osg::Vec3d const DEFAULT_UP;

            friend class VizPluginBase;
            Vizkit3DWidget(QWidget* parent = 0,const QString &world_name = "world_osg");

            /** Defined to avoid unnecessary dependencies in the headers
             *
             * If it is not defined explicitely, GCC will try to emit it inline, which
             * means that the types used in the osg_ptr below must be defined.
             */
            ~Vizkit3DWidget();

            osg::Group* getRootNode() const;
            void setTrackedNode(vizkit3d::VizPluginBase* plugin);
            QSize sizeHint() const;

            /** Sets the current camera manipulator */
            void setCameraManipulator(osg::ref_ptr<osgGA::CameraManipulator> manipulator, bool resetToDefaultHome = false);

        public slots:
            void addPlugin(QObject* plugin, QObject* parent = NULL);
            void removePlugin(QObject* plugin);

            ///The frame in which the data should be displayed
            void setVisualizationFrame(const QString &frame,bool update=true);

            // we have to use a pointer here otherwise qt ruby is crashing
            QStringList* getVisualizationFramesRuby() const;
            QStringList getVisualizationFrames() const;
            QString getVisualizationFrame() const;

            /**
             * Sets frame plugin data for a given plugin.
             * The pluging data frame is the frame in which the 
             * plugin expects the data to be.  
             * e.g. in case of the LaserScanVisualization 'laser'
             * */
            void setPluginDataFrame(const QString &frame, QObject *plugin);
            QString getPluginDataFrame(QObject *plugin)const;

            void setTransformation(const QString &source_frame,const QString &target_frame,
                    const QVector3D &position, const QQuaternion &orientation);
            void getTransformation(const QString &source_frame,const QString &target_frame, QVector3D &position, QQuaternion &orientation)const;
            QString getWorldName()const;

            void setCameraLookAt(double x, double y, double z);
            void setCameraEye(double x, double y, double z);
            void setCameraUp(double x, double y, double z);
            void getCameraView(QVector3D& eye, QVector3D& lookAt, QVector3D& up);

            void setSmallFeatureCullingPixelSize(float val);

            QColor getBackgroundColor()const;
            void setBackgroundColor(QColor color);

            void collapsePropertyBrowser();
            QWidget* getPropertyWidget()const;

            bool isTransformer() const;
            void setTransformer(bool value);
            bool isAxes() const;
            void setAxes(bool value);

            bool isAxesLabels() const;
            void setAxesLabels(bool value);
            /** Enables grabbing
             *
             * Must be called before grab()
             */
            void enableGrabbing();
            /** Disables grabbing
             *
             * You will have to call enableGrabbing() again before you can use
             * grab()
             */
            void disableGrabbing();
            /** Captures the current state of the 3D view
             *
             * You must call enableGrabbing() first. Will return an empty image
             * if you did not do so.
             */
            QImage grab();

            QString findPluginPath(QString plugin_name);
            QString findLibPath(QString lib_name);
            QObject* loadPlugin(QString lib_name,QString plugin_name);
            QStringList* getAvailablePlugins();

            /** @overload sets the camera manipulator by name */
            void setCameraManipulator(QString manipulator, bool resetToDefaultHome = false);
            /** Sets the current camera manipulator among those available */
            void setCameraManipulator(CAMERA_MANIPULATORS manipulator, bool resetToDefaultHome = false);
            /** @overload returns the name of the current camera manipulator
             * (needed for the Ruby bindings
             */
            QString getCameraManipulatorName() const;
            /** Returns the current camera manipulator */
            CAMERA_MANIPULATORS getCameraManipulator() const;

        signals:
            void addPlugins(QObject* plugin,QObject* parent);
            void removePlugins(QObject* plugin);
            void propertyChanged(QString propertyName);

        protected:
            virtual void paintEvent( QPaintEvent* event );

        private slots:
            void setPluginDataFrameIntern(const QString &frame, QObject *plugin);
            void addPluginIntern(QObject* plugin,QObject *parent=NULL);
            void removePluginIntern(QObject* plugin);
            void pluginActivityChanged(bool enabled);
            void pluginChildrenChanged();
            void addProperties(QObject* plugin,QObject *parent=NULL);

        private:
            void changeCameraView(const osg::Vec3* lookAtPos, const osg::Vec3* eyePos, const osg::Vec3* upVector);
            void setPluginEnabled(QObject* plugin, bool enabled);
            QObject* loadLib(QString file_path);

            void registerDataHandler(VizPluginBase *viz);
            void deregisterDataHandler(VizPluginBase *viz);
            void enableDataHandler(VizPluginBase *viz);
            void disableDataHandler(VizPluginBase *viz);
            osg::Group *createSceneGraph(const QString &world_name);

            QWidget* addViewWidget( osgQt::GraphicsWindowQt* gw, ::osg::Node* scene );
            osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );

        private:
            //holds the scene
            osg::ref_ptr<osg::Group> root;

            /** The set of known plugins, as a mapping from the plugin to the osg::Node
             * to which it should be attached.
             */
            typedef std::map<VizPluginBase*, osg::ref_ptr<osg::Group> > PluginMap;
            PluginMap plugins;

            QTimer _timer;
            QString current_frame;

            osg::ref_ptr<osg::Image> grabImage;

            CAMERA_MANIPULATORS currentManipulator;
    };
}
#endif
