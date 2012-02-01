#ifndef __VIZKIT_QVIZKITWIDGET__
#define __VIZKIT_QVIZKITWIDGET__

#include <vizkit/QOSGWidget.hpp>
#include <vizkit/Vizkit3DPlugin.hpp>
#include <vizkit/GridNode.hpp>
#include <vizkit/CoordinateFrame.hpp>
#include <vizkit/CompositeViewerQOSG.hpp>
#include <vizkit/PickHandler.hpp>
#include <vizkit/QPropertyBrowserWidget.hpp>
#include <QtDesigner/QDesignerExportWidget>

namespace vizkit 
{

class QDESIGNER_WIDGET_EXPORT Vizkit3DWidget : public CompositeViewerQOSG 
{
    Q_OBJECT
    Q_PROPERTY(bool show_grid READ isGridEnabled WRITE setGridEnabled)
    Q_PROPERTY(bool show_axes READ areAxesEnabled WRITE setAxesEnabled)

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
    void addPlugin(QObject* plugin, QObject* parent = NULL);
    void removePlugin(QObject* plugin);
    
    QWidget* getPropertyWidget();

    void setCameraLookAt(double x, double y, double z);
    void setCameraEye(double x, double y, double z);
    void setCameraUp(double x, double y, double z);
        
signals:
    void addPlugins(QObject* plugin,QObject* parent);
    void removePlugins(QObject* plugin);
    void propertyChanged(QString propertyName);
    
private slots:
    void addPluginIntern(QObject* plugin,QObject *parent=NULL);
    void removePluginIntern(QObject* plugin);
    void pluginActivityChanged(bool enabled);

protected:
    void changeCameraView(const osg::Vec3* lookAtPos,
            const osg::Vec3* eyePos,
            const osg::Vec3* upVector);
    bool isGridEnabled();
    void setGridEnabled(bool enabled);
    bool areAxesEnabled();
    void setAxesEnabled(bool enabled);

    osg::ref_ptr<osg::Group> root;
    void createSceneGraph();
    osg::ref_ptr<PickHandler> pickHandler;
    osg::ref_ptr<ViewQOSG> view;
    osg::ref_ptr<GridNode> groundGrid;
    osg::ref_ptr<CoordinateFrame> coordinateFrame;
    QStringList* pluginNames;
    QProperyBrowserWidget* propertyBrowserWidget;
};

}

#endif
