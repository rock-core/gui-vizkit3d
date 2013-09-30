#ifndef __VIZKIT_QVIZKITWIDGET__
#define __VIZKIT_QVIZKITWIDGET__

#include "Vizkit3DPlugin.hpp"
#include <osgViewer/CompositeViewer>

#include <QtDesigner/QDesignerExportWidget>
#include <QVector3D>
#include <QTimer>

namespace osgQt
{
    class GraphicsWindowQt;
}

namespace vizkit
{
    class QDESIGNER_WIDGET_EXPORT Vizkit3DWidget : public QWidget, public osgViewer::CompositeViewer
    {
        Q_OBJECT
        public:
            Vizkit3DWidget( QWidget* parent = 0);

            /** Defined to avoid unnecessary dependencies in the headers
             *
             * If it is not defined explicitely, GCC will try to emit it inline, which
             * means that the types used in the osg_ptr below must be defined.
             */
            ~Vizkit3DWidget();

            osg::Group* getRootNode() const;
            void setTrackedNode(vizkit::VizPluginBase* plugin);
            QSize sizeHint() const;

        public slots:
            void addPlugin(QObject* plugin, QObject* parent = NULL);
            void removePlugin(QObject* plugin);

            ///The frame in which the data should be displayed
            void setVisualizationFrame(const QString &frame);

            /**
             * Sets frame plugin data for a given plugin.
             * The pluging data frame is the frame in which the 
             * plugin expects the data to be.  
             * e.g. in case of the LaserScanVisualization 'laser'
             * */
            void setPluginDataFrame(const QString &frame, QObject *plugin);
            void setTransformation(const QString &source_frame,const QString &target_frame,
                    const QVector3D &position, const QQuaternion &orientation);

            void setCameraLookAt(double x, double y, double z);
            void setCameraEye(double x, double y, double z);
            void setCameraUp(double x, double y, double z);
            void getCameraView(QVector3D& eye, QVector3D& lookAt, QVector3D& up);

            void collapsePropertyBrowser();
            QWidget* getPropertyWidget();

        signals:
            void addPlugins(QObject* plugin,QObject* parent);
            void removePlugins(QObject* plugin);
            void propertyChanged(QString propertyName);

        protected:
            virtual void paintEvent( QPaintEvent* event );

        private slots:
            void addPluginIntern(QObject* plugin,QObject *parent=NULL);
            void removePluginIntern(QObject* plugin);
            void pluginActivityChanged(bool enabled);
            void pluginChildrenChanged();

        private:
            void changeCameraView(const osg::Vec3* lookAtPos,
                    const osg::Vec3* eyePos,
                    const osg::Vec3* upVector);
            void setPluginEnabled(QObject* plugin, bool enabled);

            void registerDataHandler(VizPluginBase *viz);
            void deregisterDataHandler(VizPluginBase *viz);
            void enableDataHandler(VizPluginBase *viz);
            void disableDataHandler(VizPluginBase *viz);
            osg::Group *createSceneGraph();

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
    };
}
#endif
