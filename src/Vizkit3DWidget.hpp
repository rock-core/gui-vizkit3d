#ifndef __VIZKIT_QVIZKITWIDGET__
#define __VIZKIT_QVIZKITWIDGET__

#include <vizkit/QOSGWidget.hpp>
#include <vizkit/VizPlugin.hpp>
#include <vizkit/CompositeViewerQOSG.hpp>
#include <vizkit/PickHandler.hpp>
#include <QtDesigner/QDesignerExportWidget>

namespace vizkit 
{

class QDESIGNER_WIDGET_EXPORT Vizkit3DWidget : public CompositeViewerQOSG 
{
    Q_OBJECT

public:
    Vizkit3DWidget( QWidget* parent = 0, Qt::WindowFlags f = 0 );
    osg::ref_ptr<osg::Group> getRootNode() const;
    void addDataHandler(VizPluginBase *viz);
    void removeDataHandler(VizPluginBase *viz);
    
    /**
     * Sets the camera focus to specific position.
     * @param lookAtPos focus this point
     */
    void changeCameraView(const osg::Vec3& lookAtPos);
    /**
     * Sets the camera focus and the camera itself to specific position.
     * @param lookAtPos focus this point
     * @param eyePos position of the camera
     */
    void changeCameraView(const osg::Vec3& lookAtPos, const osg::Vec3& eyePos);
    void setTrackedNode( vizkit::VizPluginBase* plugin );

    QSize sizeHint() const;
    
public slots:
    /**
     * Creates an instance of a visualization plugin using its
     * Vizkit Qt Plugin.
     * @param plugin Qt Plugin of the visualization plugin
     * @return Instance of the adapter collection of this plugin
     */
    QObject* createExternalPlugin(QObject* plugin, QString const& name);
    /**
     * Returns the list of visualization plugins a library provides
     *
     * @param plugin Qt Plugin used to discover the visualization plugins
     * @return the list of plugin names
     */
    QStringList* getListOfExternalPlugins(QObject* qt_plugin);
    QStringList* getListOfAvailablePlugins();
    QObject* createPluginByName(QString pluginName);

public slots:
    void setCameraLookAt(double x, double y, double z);
    void setCameraEye(double x, double y, double z);
    void setCameraUp(double x, double y, double z);

protected:
    void changeCameraView(const osg::Vec3* lookAtPos,
            const osg::Vec3* eyePos,
            const osg::Vec3* upVector);

    osg::ref_ptr<osg::Group> root;
    void createSceneGraph();
    osg::ref_ptr<PickHandler> pickHandler;
    osg::ref_ptr<ViewQOSG> view;
    QStringList* pluginNames;
};

}

#endif
