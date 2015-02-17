#ifndef __VIZKIT_QVIZKITWIDGET__
#define __VIZKIT_QVIZKITWIDGET__

#include "Vizkit3DPlugin.hpp"
#include <osgViewer/CompositeViewer>

#include <QtDesigner/QDesignerExportWidget>
#include <QVector3D>
#include <QTimer>

namespace osgQt { class GraphicsWindowQt;}
namespace vizkit3d
{
    class ConnexionPlugin;

    // configuration class
    class Vizkit3DConfig :public QObject
    {
        Q_OBJECT
        Q_PROPERTY( bool axes READ isAxes WRITE setAxes)
        Q_PROPERTY( bool axes_labels READ isAxesLabels WRITE setAxesLabels)
        Q_PROPERTY( QColor background READ getBackgroundColor WRITE setBackgroundColor)
        Q_PROPERTY( QStringList frame READ getVisualizationFrames WRITE setVisualizationFrame)
        Q_PROPERTY( bool transformer READ isTransformer WRITE setTransformer)

        public:
            Vizkit3DConfig(QObject *parent);

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
    };

    class QDESIGNER_WIDGET_EXPORT Vizkit3DWidget : public QWidget, public osgViewer::CompositeViewer
    {
        Q_OBJECT
        public:
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

            ConnexionPlugin *cp;

        public slots:
            void addPlugin(QObject* plugin, QObject* parent = NULL);
            void removePlugin(QObject* plugin);

            ///The frame in which the data should be displayed
            void setVisualizationFrame(const QString &frame,bool update=true);

            // we have to use a pointer here otherwise qt ruby is crashing
            QStringList* getVisualizationFrames() const;
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
    };
}
#endif
