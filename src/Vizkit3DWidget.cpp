#include <QVBoxLayout>
#include <QSplitter>
#include <QComboBox>
#include <QGroupBox>
#include <QPlastiqueStyle>
#include <algorithm>

#include "Vizkit3DWidget.hpp"
#include "Vizkit3DPlugin.hpp"
#include "PickHandler.hpp"
#include "QPropertyBrowserWidget.hpp"

#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>

using namespace vizkit;
using namespace std;

Vizkit3DWidget::Vizkit3DWidget( QWidget* parent)
    : QWidget(parent)
{
    //create layout
    //objects will be owned by the parent widget (this)
    QVBoxLayout* controlLayout = new QVBoxLayout;
    QVBoxLayout* layout = new QVBoxLayout;
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    QWidget* controlWidget = new QWidget;

    layout->addWidget(splitter);
    this->setLayout(layout);
    controlWidget->setLayout(controlLayout);
    splitter->addWidget(controlWidget);

    // set threading model
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    // create root scene node
    root = createSceneGraph();

    // create osg widget
    QWidget* widget = addViewWidget(createGraphicsWindow(0,0,100,100), root);
    widget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    widget->setObjectName(QString("View Widget"));
    splitter->addWidget(widget);

    // create propertyBrowserWidget
    QPropertyBrowserWidget *propertyBrowserWidget = new QPropertyBrowserWidget( parent );
    propertyBrowserWidget->setObjectName("PropertyBrowser");
    propertyBrowserWidget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    controlLayout->addWidget(propertyBrowserWidget);

    // create visualization of the coordinate axes
    // TODO
    // changeCameraView(osg::Vec3d(0,0,0), osg::Vec3d(-5,0,5));

    //connect signals and slots
    connect(this, SIGNAL(addPlugins(QObject*,QObject*)), this, SLOT(addPluginIntern(QObject*,QObject*)));
    connect(this, SIGNAL(removePlugins(QObject*)), this, SLOT(removePluginIntern(QObject*)));
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );

    //start timer responsible for updating osg viewer
    _timer.start(10);
}

Vizkit3DWidget::~Vizkit3DWidget() {}

QWidget* Vizkit3DWidget::addViewWidget( osgQt::GraphicsWindowQt* gw, ::osg::Node* scene )
{
    osgViewer::View* view = new osgViewer::View;
    addView(view);

    ::osg::Camera* camera = view->getCamera();
    camera->setGraphicsContext( gw );

    const ::osg::GraphicsContext::Traits* traits = gw->getTraits();

    camera->setClearColor(::osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new ::osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

    view->setSceneData(scene);
    view->addEventHandler( new osgViewer::StatsHandler );
    // view->setCameraManipulator( new osgGA::TrackballManipulator );
    view->setCameraManipulator( new osgGA::TerrainManipulator);

    // pickhandler is for selecting objects in the opengl view
    PickHandler* pickHandler = new PickHandler();
    view->addEventHandler(pickHandler);

    return gw->getGLWidget();
}

osgQt::GraphicsWindowQt* Vizkit3DWidget::createGraphicsWindow( int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
    ::osg::DisplaySettings* ds = ::osg::DisplaySettings::instance().get();
    ::osg::ref_ptr< ::osg::GraphicsContext::Traits> traits = new ::osg::GraphicsContext::Traits;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();
    return new osgQt::GraphicsWindowQt(traits.get());
}

void Vizkit3DWidget::paintEvent( QPaintEvent* event )
{
    frame();
}

QSize Vizkit3DWidget::sizeHint() const
{
    return QSize( 1000, 600 );
}

osg::Group* Vizkit3DWidget::getRootNode() const
{
    return root;
}

void Vizkit3DWidget::setTrackedNode( VizPluginBase* plugin )
{
    osgViewer::View *view = getView(0);
    assert(view);
    //TODO 
}

osg::Group *Vizkit3DWidget::createSceneGraph()
{
    //create root node that holds all other nodes
    osg::Group *root = new osg::Group;

    osg::ref_ptr<osg::StateSet> state = root->getOrCreateStateSet();
    state->setGlobalDefaults();
    state->setMode( GL_LINE_SMOOTH, osg::StateAttribute::ON );
    state->setMode( GL_POINT_SMOOTH, osg::StateAttribute::ON );
    state->setMode( GL_BLEND, osg::StateAttribute::ON );    
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON);
    state->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    state->setMode( GL_LIGHT0, osg::StateAttribute::ON );
    state->setMode( GL_LIGHT1, osg::StateAttribute::ON );

    root->setDataVariance(osg::Object::DYNAMIC);

    // Add the Light to a LightSource. Add the LightSource and
    // MatrixTransform to the scene graph.
    for(size_t i=0;i<2;i++)
    {
        osg::ref_ptr<osg::Light> light = new osg::Light;
        light->setLightNum(i);
        switch(i) {
        case 0:
            light->setAmbient( osg::Vec4( .1f, .1f, .1f, 1.f ));
            light->setDiffuse( osg::Vec4( .8f, .8f, .8f, 1.f ));
            light->setSpecular( osg::Vec4( .8f, .8f, .8f, 1.f ));
            light->setPosition( osg::Vec4( 1.f, 1.5f, 2.f, 0.f ));
            break;
        case 1:
            light->setAmbient( osg::Vec4( .1f, .1f, .1f, 1.f ));
            light->setDiffuse( osg::Vec4( .1f, .3f, .1f, 1.f ));
            light->setSpecular( osg::Vec4( .1f, .3f, .1f, 1.f ));
            light->setPosition( osg::Vec4( -1.f, -3.f, 1.f, 0.f ));
        }

        osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
        ls->setLight( light.get() );
        root->addChild( ls.get() );
    }
    return root;
}

void Vizkit3DWidget::registerDataHandler(VizPluginBase* viz)
{
    osg::Node::ParentList current_parents =
        viz->getRootNode()->getParents();
    osg::Group* initial_parent = root;
    if (!current_parents.empty())
    {
        initial_parent = current_parents.front()->asGroup();
        if (!initial_parent)
            throw std::logic_error("the parent OSG node of the given vizkit plugin's root node is not an osg::Group. This is required");
    }
    plugins.insert(make_pair(viz, initial_parent));
}

void Vizkit3DWidget::deregisterDataHandler(VizPluginBase* viz)
{
    PluginMap::iterator it = plugins.find(viz);
    if (it == plugins.end())
        throw std::runtime_error("trying to deregister a plugin that is not registered on this widget");

    disableDataHandler(viz);
    plugins.erase(it);
}

void Vizkit3DWidget::enableDataHandler(VizPluginBase *viz)
{
    PluginMap::iterator it = plugins.find(viz);
    if (it != plugins.end())
        it->second->addChild( viz->getRootNode() );
}

void Vizkit3DWidget::disableDataHandler(VizPluginBase *viz)
{
    PluginMap::iterator it = plugins.find(viz);
    if (it != plugins.end())
        it->second->removeChild( viz->getRootNode() );
}

void Vizkit3DWidget::setPluginEnabled(QObject* plugin, bool enabled)
{
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    if (!viz_plugin)
        return;

    PluginMap::const_iterator plugin_it = plugins.find(viz_plugin);
    if (plugin_it == plugins.end())
        return;

    // Check the current state
    osg::Node::ParentList const& list = viz_plugin->getRootNode()->getParents();
    bool is_enabled = std::find(list.begin(), list.end(), plugin_it->second) != list.end();

    if (enabled && !is_enabled)
        enableDataHandler(viz_plugin);
    else if (!enabled && is_enabled)
        disableDataHandler(viz_plugin);
}

void Vizkit3DWidget::pluginActivityChanged(bool enabled)
{
    return setPluginEnabled(QObject::sender(), enabled);
}

void Vizkit3DWidget::setCameraLookAt(double x, double y, double z)
{
    osg::Vec3 lookAt(x, y, z);
    changeCameraView(&lookAt, 0, 0);
}

void Vizkit3DWidget::setCameraEye(double x, double y, double z)
{
    osg::Vec3 eye(x, y, z);
    changeCameraView(0, &eye, 0);
}

void Vizkit3DWidget::setCameraUp(double x, double y, double z)
{
    osg::Vec3 up(x, y, z);
    changeCameraView(0, 0, &up);
}

void Vizkit3DWidget::collapsePropertyBrowser()
{
    //TODO (disable property browser)
}

void Vizkit3DWidget::getCameraView(QVector3D& lookAtPos, QVector3D& eyePos, QVector3D& upVector)
{
    osg::Vec3d eye, lookAt, up;

    osgViewer::View *view = getView(0);
    assert(view);
    view->getCamera()->getViewMatrixAsLookAt(eye, lookAt, up);

    eyePos.setX(eye.x());
    eyePos.setY(eye.y());
    eyePos.setZ(eye.z());
    lookAtPos.setX(lookAt.x());
    lookAtPos.setY(lookAt.y());
    lookAtPos.setZ(lookAt.z());
    upVector.setX(up.x());
    upVector.setY(up.y());
    upVector.setZ(up.z());
}

void Vizkit3DWidget::changeCameraView(const osg::Vec3* lookAtPos, const osg::Vec3* eyePos, const osg::Vec3* upVector)
{
    osgViewer::View *view = getView(0);
    assert(view);

    osgGA::KeySwitchMatrixManipulator* switchMatrixManipulator = dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(view->getCameraManipulator());
    if (!switchMatrixManipulator) return;
    //select TerrainManipulator
    //switchMatrixManipulator->selectMatrixManipulator(3); //why this switch was needed here?, each manipulator should be able  to do the folliowing steps

    //get last values of eye, center and up
    osg::Vec3d eye, center, up;
    switchMatrixManipulator->getHomePosition(eye, center, up);

    if (lookAtPos)
        center = *lookAtPos;
    if (eyePos)
        eye = *eyePos;
    if (upVector)
        up = *upVector;

    //set new values
    switchMatrixManipulator->setHomePosition(eye, center, up);
    view->home();
}

/**
 * Puts the plugin in a list and emits a signal.
 * Adding the new Plugin will be handled by the main thread.
 * @param plugin Vizkit Plugin
 */
void Vizkit3DWidget::addPlugin(QObject* plugin,QObject *parent)
{
    emit addPlugins(plugin,parent);
}

/**
 * Puts the plugin in a list and emits a signal.
 * Removing the new Plugin will be handled by the main thread.
 * @param plugin Vizkit Plugin
 */
void Vizkit3DWidget::removePlugin(QObject* plugin)
{
    emit removePlugins(plugin);
}

/**
 * This slot adds all plugins in the list to the OSG and
 * their properties to the property browser widget.
 */
void Vizkit3DWidget::addPluginIntern(QObject* plugin,QObject *parent)
{
    assert(plugin);

    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    bool has_plugin = plugins.find(viz_plugin) != plugins.end();
    if (viz_plugin && !has_plugin)
    {
        // Make sure that the plugins do have a parent.
        if (!viz_plugin->parent())
            viz_plugin->setParent(this);

        registerDataHandler(viz_plugin);
        setPluginEnabled(viz_plugin, viz_plugin->isPluginEnabled());

        QPropertyBrowserWidget *propertyBrowserWidget = dynamic_cast<QPropertyBrowserWidget*>(getPropertyWidget());
        if(propertyBrowserWidget)
            propertyBrowserWidget->addProperties(viz_plugin,parent);

        connect(viz_plugin, SIGNAL(pluginActivityChanged(bool)), this, SLOT(pluginActivityChanged(bool)));
        connect(viz_plugin, SIGNAL(childrenChanged()), this, SLOT(pluginChildrenChanged()));
        connect(viz_plugin, SIGNAL(destroyed(QObject*)), this, SLOT(removePluginIntern(QObject*)));
    }

    // add sub plugins if object has some
    QList<QObject*> object_list = plugin->findChildren<QObject *>();
    for(QList<QObject*>::const_iterator it =object_list.begin(); it != object_list.end(); it++)
        addPluginIntern(*it,plugin);
}

void Vizkit3DWidget::pluginChildrenChanged()
{
    addPluginIntern(QObject::sender());
}

/**
 * This slot removes all plugins in the list from the OSG and
 * their properties to the property browser widget.
 */
void Vizkit3DWidget::removePluginIntern(QObject* plugin)
{
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    if (viz_plugin)
    {
        deregisterDataHandler(viz_plugin);
        QPropertyBrowserWidget *propertyBrowserWidget = dynamic_cast<QPropertyBrowserWidget*>(getPropertyWidget());
        if(propertyBrowserWidget)
            propertyBrowserWidget->removeProperties(viz_plugin);
        disconnect(viz_plugin, SIGNAL(pluginActivityChanged(bool)), this, SLOT(pluginActivityChanged(bool)));
        disconnect(viz_plugin, SIGNAL(childrenChanged()), this, SLOT(pluginChildrenChanged()));
    }
}

QWidget* Vizkit3DWidget::getPropertyWidget()
{
    return findChild<QPropertyBrowserWidget*>("PropertyBrowser");
}

void Vizkit3DWidget::setPluginDataFrame(const QString& frame, QObject* plugin)
{
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    if(!viz_plugin)
        throw std::runtime_error("setPluginDataFrame called with something that is no vizkit plugin");
}

void Vizkit3DWidget::setVisualizationFrame(const QString& frame)
{

}

void Vizkit3DWidget::setTransformation(const QString &source_frame,const QString &target_frame,
        const QVector3D &position, const QQuaternion &orientation)
{

}
