#include <QVBoxLayout>
#include <QSplitter>
#include <QComboBox>
#include <QGroupBox>
#include <QPlastiqueStyle>
#include <QProcessEnvironment>
#include <QPluginLoader>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <algorithm>

#include "Vizkit3DBase.hpp"
#include "Vizkit3DWidget.hpp"
#include "Vizkit3DPlugin.hpp"
#include "PickHandler.hpp"
#include "QPropertyBrowserWidget.hpp"
#include "AxesNode.hpp"
#include "OsgVisitors.hpp"
#include "TransformerGraph.hpp"
#include "EnableGLDebugOperation.hpp"
#include <boost/lexical_cast.hpp>
#include <vizkit3d/EnvPluginBase.hpp>

#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerEventHandlers>

#include <vizkit3d/DefaultManipulator.hpp>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/OrbitManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/MultiTouchTrackballManipulator>

#include <vizkit3d/WindowCaptureCallback.hpp>

using namespace vizkit3d;
using namespace std;

osg::Vec3d const Vizkit3DWidget::DEFAULT_EYE(-5, 0, 5);
osg::Vec3d const Vizkit3DWidget::DEFAULT_CENTER(0, 0, 0);
osg::Vec3d const Vizkit3DWidget::DEFAULT_UP(0, 0, 1);

Vizkit3DWidget* Vizkit3DConfig::getWidget() const
{
    Vizkit3DWidget *parent = dynamic_cast<Vizkit3DWidget*>(this->parent());
    assert(parent);
    return parent;
}

Vizkit3DConfig::Vizkit3DConfig(Vizkit3DWidget *parent):QObject(parent)
{
    setObjectName("Viewer");
    connect(parent, SIGNAL(propertyChanged(QString)),this,SIGNAL(propertyChanged(QString)));
}

bool Vizkit3DConfig::isAxes() const
{
    return getWidget()->isAxes();
}

void Vizkit3DConfig::setAxes(bool value)
{
    return getWidget()->setAxes(value);
}

QStringList Vizkit3DConfig::getVisualizationFrames() const
{
    QStringList frames = getWidget()->getVisualizationFrames();
    if (!frames.isEmpty())
        frames.push_front(getWidget()->getVisualizationFrame());
    return frames;
}

void Vizkit3DConfig::setVisualizationFrame(const QStringList &frames)
{
    return getWidget()->setVisualizationFrame(frames.front(),false);
}

bool Vizkit3DConfig::isTransformer() const
{
    return getWidget()->isTransformer();
}

void Vizkit3DConfig::setTransformer(bool value)
{
    return getWidget()->setTransformer(value);
}

bool Vizkit3DConfig::isEnvironmentPluginEnabled() const
{
    return getWidget()->isEnvironmentPluginEnabled();
}

void Vizkit3DConfig::setEnvironmentPluginEnabled(bool enabled)
{
    return getWidget()->setEnvironmentPluginEnabled(enabled);
}

float Vizkit3DConfig::getTransformerTextSize() const
{
    return getWidget()->getTransformerTextSize();
}

void Vizkit3DConfig::setTransformerTextSize(float value)
{
    return getWidget()->setTransformerTextSize(value);
}

QColor Vizkit3DConfig::getBackgroundColor() const
{
    return getWidget()->getBackgroundColor();
}

void Vizkit3DConfig::setBackgroundColor(QColor color)
{
    return getWidget()->setBackgroundColor(color);
}

void Vizkit3DConfig::setAxesLabels(bool value)
{
    return getWidget()->setAxesLabels(value);
}

bool Vizkit3DConfig::isAxesLabels() const
{
    return getWidget()->isAxesLabels();
}

namespace
{
    struct ManipulatorDefinition
    {
        QString name;
        CAMERA_MANIPULATORS id;
        bool is_public;
    };
    ManipulatorDefinition KNOWN_MANIPULATORS[] = {
        { "Default", DEFAULT_MANIPULATOR, true },
        { "First Person", FIRST_PERSON_MANIPULATOR, true },
        { "Flight", FLIGHT_MANIPULATOR, true },
        { "Multi Touch Trackball", MULTI_TOUCH_TRACKBALL_MANIPULATOR, true },
        { "Node Tracker", NODE_TRACKER_MANIPULATOR, false },
        { "Orbit", ORBIT_MANIPULATOR, true },
        { "Terrain", TERRAIN_MANIPULATOR, true },
        { "Trackball", TRACKBALL_MANIPULATOR, true },
        { "None", NO_MANIPULATOR, true },
        { 0, FIRST_PERSON_MANIPULATOR } // only the empty string is used as guard
    };
}

QStringList Vizkit3DConfig::getAvailableCameraManipulators() const
{
    QStringList names;
    for (int i = 0; KNOWN_MANIPULATORS[i].name != 0; ++i)
    {
        if (KNOWN_MANIPULATORS[i].is_public)
            names.append(KNOWN_MANIPULATORS[i].name);
    }

    QString current = getWidget()->getCameraManipulatorName();
    // When using the node tracker, the visualization frame name is not part of
    // the list of available frames. Add it at the back to ensure the qt
    // property browser handles it properly
    if (!names.contains(current))
        names.push_back(current);

    // The first element of the list is interpreted as the currently selected
    // frame by the property browser
    names.push_front(current);
    return names;
}

QString Vizkit3DConfig::manipulatorIDToName(CAMERA_MANIPULATORS id)
{
    for (int i = 0; KNOWN_MANIPULATORS[i].name != 0; ++i)
    {
        if (id == KNOWN_MANIPULATORS[i].id)
            return KNOWN_MANIPULATORS[i].name;
    }
    throw std::invalid_argument("camera manipulator ID " + boost::lexical_cast<std::string>(id) + " has no declared name");
}

CAMERA_MANIPULATORS Vizkit3DConfig::manipulatorNameToID(QString const& name)
{
    for (int i = 0; KNOWN_MANIPULATORS[i].name != 0; ++i)
    {
        if (name == KNOWN_MANIPULATORS[i].name)
            return KNOWN_MANIPULATORS[i].id;
    }
    throw std::invalid_argument("camera manipulator name " + name.toStdString() + " does not exist");
}

void Vizkit3DConfig::setCameraManipulator(QStringList const& manipulator)
{
    if (getWidget()->getCameraManipulatorName() == manipulator.front())
        return;
    
    CAMERA_MANIPULATORS id = manipulatorNameToID(manipulator.front());
    return getWidget()->setCameraManipulator(id);
}

Vizkit3DWidget::Vizkit3DWidget( QWidget* parent,int width,int height,const QString &world_name,bool auto_update)
    : QWidget(parent)
    , env_plugin(NULL)
{
    //create layout
    //objects will be owned by the parent widget (this)
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setObjectName("main_layout");
    layout->setContentsMargins(2,2,2,2);
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->setObjectName("splitter");

    layout->addWidget(splitter);
    this->setLayout(layout);

    // set threading model
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    if (getenv("VIZKIT_GL_DEBUG") && (std::string(getenv("VIZKIT_GL_DEBUG")) == "1"))
    {
        osg::setNotifyLevel(osg::DEBUG_INFO);
        setRealizeOperation(new EnableGLDebugOperation());
    }
    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    // create root scene node
    root = createSceneGraph(world_name);

    // create osg widget
    QWidget* widget = addViewWidget(createGraphicsWindow(0,0,width,height), root);
    widget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    widget->setObjectName(QString("View Widget"));
    splitter->addWidget(widget);

    // create propertyBrowserWidget
    QPropertyBrowserWidget *propertyBrowserWidget = new QPropertyBrowserWidget( parent );
    propertyBrowserWidget->setObjectName("PropertyBrowser");
    propertyBrowserWidget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    propertyBrowserWidget->resize(200,600);
    splitter->addWidget(propertyBrowserWidget);

    // add config object to the property browser
    Vizkit3DConfig *config =  new Vizkit3DConfig(this);
    addProperties(config,NULL);

    //connect signals and slots
    connect(this, SIGNAL(addPlugins(QObject*,QObject*)), this, SLOT(addPluginIntern(QObject*,QObject*)));
    connect(this, SIGNAL(removePlugins(QObject*)), this, SLOT(removePluginIntern(QObject*)));
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );

    current_frame = QString(root->getName().c_str());

    //start timer responsible for updating osg viewer
    if (auto_update)
        _timer.start(10);
}

Vizkit3DWidget::~Vizkit3DWidget() {}

//qt ruby is crashing if we use none pointer here
QStringList* Vizkit3DWidget::getVisualizationFramesRuby() const
{
    return new QStringList(getVisualizationFrames());
}

QStringList Vizkit3DWidget::getVisualizationFrames() const
{
    QStringList list;
    vector<string> names = TransformerGraph::getFrameNames(*getRootNode());
    for (unsigned int i = 0; i != names.size(); ++i)
        list.append(QString::fromStdString(names[i]));
    return list;
}

QString Vizkit3DWidget::getVisualizationFrame() const
{
    return current_frame;
}

struct ImageGrabOperation : public vizkit3d::CaptureOperation
{
    uint64_t frame_id;
    QImage image;

    ImageGrabOperation()
        : frame_id(0) {}

    void operator()(const osg::Image& image, const unsigned int)
    {
        frame_id++;

        QImage::Format qtFormat;
        if (image.getPixelFormat() == GL_BGR)
            qtFormat = QImage::Format_RGB888;
        else if (image.getPixelFormat() == GL_BGRA)
            qtFormat = QImage::Format_ARGB32;
        else
            throw std::runtime_error("cannot interpret osg-provided image format " +
                    boost::lexical_cast<std::string>(image.getPixelFormat()));

        QImage img(image.data(), image.s(), image.t(), qtFormat);
        this->image = img.mirrored(false, true);
    }
};

void Vizkit3DWidget::enableGrabbing(GrabbingMode mode)
{
    if (captureCallback)
        return;

    WindowCaptureCallback* callback = new WindowCaptureCallback(-1,
            mode, WindowCaptureCallback::END_FRAME, GL_BACK, GL_BGR);
    ImageGrabOperation* op = new ImageGrabOperation;
    callback->setCaptureOperation(op);

    captureCallback  = callback;
    captureOperation = op;

    // If in multi-pbo mode, we have to grab a few frames "for starter" to make
    // sure that grab() will always return a valid image
    int count = 0;
    if (mode == DOUBLE_PBO)
        count = 1;
    else if (mode == TRIPLE_PBO)
        count = 2;
    if (count > 0)
    {
        getView(0)->getCamera()->setFinalDrawCallback(captureCallback);
        for (int i = 0; i < count; ++i)
            frame();
        getView(0)->getCamera()->setFinalDrawCallback(0);
    }
}

void Vizkit3DWidget::disableGrabbing()
{
    captureOperation = NULL;
    captureCallback = NULL;
}

QImage Vizkit3DWidget::grab()
{
    if (!captureCallback)
    {
        qWarning("you must call enableGrabbing() before grab()");
        return QImage();
    }

    getView(0)->getCamera()->setFinalDrawCallback(captureCallback);
    frame();
    getView(0)->getCamera()->setFinalDrawCallback(0);
    return static_cast<ImageGrabOperation&>(*captureOperation).image;
};

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
    camera->setCullMask(~INVISIBLE_NODE_MASK);

    view->setSceneData(scene);
    view->addEventHandler( new osgViewer::StatsHandler );
    QString manipulator_name = getenv("ROCK_DEFAULT_MANIPULATOR");
    if(manipulator_name.isEmpty())
        manipulator_name = "Terrain";
    CAMERA_MANIPULATORS manipulator_id = 
        Vizkit3DConfig::manipulatorNameToID(manipulator_name);
    setCameraManipulator(manipulator_id);

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
    return setTrackedNode(plugin->getRootNode(), QString("<Plugin %1>").arg(plugin->getPluginName()));
}

void Vizkit3DWidget::setTrackedNode( osg::Node* node, QString tracked_object_name )
{
    osgViewer::View *view = getView(0);
    assert(view);

    osgGA::NodeTrackerManipulator* manipulator = new osgGA::NodeTrackerManipulator;
    view->setCameraManipulator(manipulator);
    manipulator->setTrackNode(node);
    manipulator->setHomePosition(osg::Vec3(-5, 0, 5), osg::Vec3(0,0,0), osg::Vec3(0,0,1));
    manipulator->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);
    if (current_manipulator != NODE_TRACKER_MANIPULATOR)
        last_manipulator = current_manipulator;
    current_manipulator = NODE_TRACKER_MANIPULATOR;
    view->home();
    this->tracked_object_name = tracked_object_name;
    emit propertyChanged("manipulator");
}


osg::Group *Vizkit3DWidget::createSceneGraph(const QString &world_name)
{
    //create root node that holds all other nodes
    osg::Group *root = TransformerGraph::create(world_name.toStdString())->asGroup();
    assert(root);

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
            light->setDiffuse( osg::Vec4( .3f, .3f, .3f, 1.f ));
            light->setSpecular( osg::Vec4( .3f, .3f, .3f, 1.f ));
            light->setPosition( osg::Vec4( -1.f, -3.f, 1.f, 0.f ));
        }

        osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
        ls->setLight( light.get() );
        root->addChild( ls.get() );
    }

    // add coordinate axes
    osg::Switch *switch_node = new osg::Switch();
    switch_node->addChild(AxesNode::create());
    switch_node->setAllChildrenOn();
    switch_node->setName("axes_node");
    root->addChild(switch_node);

    return root;
}

void Vizkit3DWidget::registerDataHandler(VizPluginBase* viz)
{
    osg::Group* initial_parent = TransformerGraph::getFrameGroup(*getRootNode());
    assert(initial_parent);
    plugins.insert(make_pair(viz, initial_parent));
}

void Vizkit3DWidget::deregisterDataHandler(VizPluginBase* viz)
{
    PluginMap::iterator it = plugins.find(viz);
    if (it == plugins.end())
        throw std::runtime_error("trying to deregister a plugin that is not registered on this widget");

    if (viz == env_plugin)
        clearEnvironmentPlugin();

    disableDataHandler(viz);
    plugins.erase(it);
}

void Vizkit3DWidget::enableDataHandler(VizPluginBase *viz)
{
    if (viz == env_plugin)
        throw std::invalid_argument("attempted to enable the environment plugin");

    PluginMap::iterator it = plugins.find(viz);
    if (it != plugins.end())
        it->second->addChild(viz->getRootNode());
}

void Vizkit3DWidget::disableDataHandler(VizPluginBase *viz)
{
    PluginMap::iterator it = plugins.find(viz);
    if (it != plugins.end())
        it->second->removeChild( viz->getRootNode() );
}

void Vizkit3DWidget::setPluginEnabled(QObject* plugin, bool enabled)
{
    vizkit3d::VizPluginBase* viz_plugin = dynamic_cast<vizkit3d::VizPluginBase*>(plugin);
    if (!viz_plugin)
        return;

    if (viz_plugin == env_plugin)
    {
        setEnvironmentPluginEnabled(enabled);
        return;
    }

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

void Vizkit3DWidget::setEnvironmentPlugin(QObject* plugin)
{
    EnvPluginBase* env_plugin = dynamic_cast<EnvPluginBase*>(plugin);
    if (!env_plugin)
        throw std::invalid_argument("plugin given to setEnvironmentPlugin is not from a subclass of EnvPluginBase");

    PluginMap::iterator it = plugins.find(env_plugin);
    if (it == plugins.end())
    {
        addPlugin(env_plugin);
        it = plugins.find(env_plugin);
    }

    clearEnvironmentPlugin();

    it->second->removeChild(env_plugin->getRootNode());
    env_plugin->getRefNode()->addChild(root);
    this->env_plugin = env_plugin;
    setEnvironmentPluginEnabled(env_plugin->isPluginEnabled());
}

void Vizkit3DWidget::setEnvironmentPluginEnabled(bool enabled)
{
    if (!env_plugin)
        return;

    osgViewer::View *view = getView(0);
    if (enabled)
        view->setSceneData(env_plugin->getRootNode());
    else
        view->setSceneData(root);
    emit propertyChanged("environment");
}

bool Vizkit3DWidget::isEnvironmentPluginEnabled() const
{
    if (!env_plugin)
        return false;
    return getView(0)->getSceneData() != root;
}

void Vizkit3DWidget::clearEnvironmentPlugin()
{
    if (!env_plugin)
        return;

    setEnvironmentPluginEnabled(false);

    env_plugin->getRefNode()->removeChild(root);
    PluginMap::iterator it = plugins.find(env_plugin);
    if (it != plugins.end())
        it->second->addChild(env_plugin->getRootNode());
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
    QSplitter *splitter = findChild<QSplitter*>("splitter");
    if(!splitter)
        return;
    QList<int> sizes;
    sizes.push_front(0);
    splitter->setSizes(sizes);
}


void Vizkit3DWidget::setSmallFeatureCullingPixelSize(float val)
{
    osgViewer::View *view = getView(0);
    assert(view);
    view->getCamera()->setSmallFeatureCullingPixelSize(val);
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

    osgGA::CameraManipulator* manipulator = dynamic_cast<osgGA::CameraManipulator*>(view->getCameraManipulator());
    if (!manipulator)
    {
        osg::Vec3d eye, center, up;
        view->getCamera()->getViewMatrixAsLookAt(eye, center, up);
        if (lookAtPos)
            center = *lookAtPos;
        if (eyePos)
            eye = *eyePos;
        if (upVector)
            up = *upVector;
        view->getCamera()->setViewMatrixAsLookAt(eye, center, up);
    }
    else
    {
        osg::Vec3d eye, center, up;
        manipulator->getHomePosition(eye, center, up);

        if (lookAtPos)
            center = *lookAtPos;
        if (eyePos)
            eye = *eyePos;
        if (upVector)
            up = *upVector;

        //set new values
        manipulator->setHomePosition(eye, center, up);
        view->home();
    }
}

QColor Vizkit3DWidget::getBackgroundColor()const
{
    const osgViewer::View *view = getView(0);
    assert(view);
    osg::Vec4 color = view->getCamera()->getClearColor();
    return QColor(color.r()*255,color.g()*255,color.b()*255,color.a()*255);
}

void Vizkit3DWidget::setBackgroundColor(QColor color)
{
    osgViewer::View *view = getView(0);
    assert(view);
    view->getCamera()->setClearColor(::osg::Vec4(color.red()/255.0,color.green()/255.0,color.blue()/255.0,1.0));
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

void Vizkit3DWidget::addProperties(QObject* plugin,QObject *parent)
{
    QPropertyBrowserWidget *propertyBrowserWidget = dynamic_cast<QPropertyBrowserWidget*>(getPropertyWidget());
    if(propertyBrowserWidget)
        propertyBrowserWidget->addProperties(plugin,parent);
}

/**
 * This slot adds all plugins in the list to the OSG and
 * their properties to the property browser widget.
 */
void Vizkit3DWidget::addPluginIntern(QObject* plugin,QObject *parent)
{
    assert(plugin);

    vizkit3d::VizPluginBase* viz_plugin = dynamic_cast<vizkit3d::VizPluginBase*>(plugin);
    bool has_plugin = plugins.find(viz_plugin) != plugins.end();
    if (viz_plugin && !has_plugin)
    {
        viz_plugin->setParent(this);
        viz_plugin->setVisualizationFrame(getRootNode()->getName().c_str());

        registerDataHandler(viz_plugin);
        setPluginEnabled(viz_plugin, viz_plugin->isPluginEnabled());
        addProperties(viz_plugin,parent);


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
    vizkit3d::VizPluginBase* viz_plugin = dynamic_cast<vizkit3d::VizPluginBase*>(plugin);
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

QWidget* Vizkit3DWidget::getPropertyWidget() const
{
    return findChild<QPropertyBrowserWidget*>("PropertyBrowser");
}

void Vizkit3DWidget::setPluginDataFrame(const QString& frame, QObject* plugin)
{
    vizkit3d::VizPluginBase* viz= dynamic_cast<vizkit3d::VizPluginBase*>(plugin);
    if(!viz)
        throw std::runtime_error("setPluginDataFrame called with something that is not a vizkit3d plugin");

    //always call plugin to be synchronized
    viz->setVisualizationFrame(frame);
}

QString Vizkit3DWidget::getPluginDataFrame(QObject* plugin)const
{
    vizkit3d::VizPluginBase* viz= dynamic_cast<vizkit3d::VizPluginBase*>(plugin);
    if(!viz)
        throw std::runtime_error("setPluginDataFrame called with something that is not a vizkit3d plugin");
    return viz->getVisualizationFrame();
}

// should be called from the plugin 
void Vizkit3DWidget::setPluginDataFrameIntern(const QString& frame, QObject* plugin)
{
    vizkit3d::VizPluginBase* viz= dynamic_cast<vizkit3d::VizPluginBase*>(plugin);
    if(!viz)
        throw std::runtime_error("setPluginDataFrame called with something that is not a vizkit3d plugin");

    TransformerGraph::addFrame(*getRootNode(),frame.toStdString());
    osg::Group* node = TransformerGraph::getFrameGroup(*getRootNode(),frame.toStdString());
    assert(node);
    PluginMap::iterator it = plugins.find(viz);
    if (it != plugins.end())
    {
        if(viz != env_plugin && viz->isPluginEnabled())
        {
            disableDataHandler(viz);
            it->second = node;
            enableDataHandler(viz);
        }
        else
            it->second = node;
    }
}

QString Vizkit3DWidget::getRootVisualizationFrame() const
{
    return QString::fromStdString(getRootNode()->getName());
}

void Vizkit3DWidget::setVisualizationFrame(const QString& frame)
{
    if (current_frame == frame)
        return;
    else if (frame.isEmpty() || frame == getRootVisualizationFrame())
    {
        // Reset to the previously used manipulator. This will reset the frame
        // to the root frame and emit the propertyChanged event
        setCameraManipulator(last_manipulator);
        return;
    }

    osgViewer::View *view = getView(0);
    assert(view);
    // the following is not working if the directly track the transformation 
    // therefore use a child
    osg::Node *node = TransformerGraph::getFrameGroup(*getRootNode(),frame.toStdString());
    if (!node)
        throw std::invalid_argument("frame " + frame.toStdString() + " does not exist");

    current_frame = frame;
    setTrackedNode(node, QString("<Frame %1>").arg(frame));
    emit propertyChanged("frame");
}

void Vizkit3DWidget::setTransformation(const QString &source_frame,const QString &target_frame,
        const QVector3D &_position, const QQuaternion &_quat)
{
    // enforce length of 1
    QQuaternion quat = _quat.normalized();
    // enforce valid position
    QVector3D position = _position;
    if(std::isnan(position.x()) ||std::isnan(position.y()) || std::isnan(position.z()))
        position = QVector3D();
    int count = getVisualizationFrames().size();
    TransformerGraph::setTransformation(*getRootNode(),source_frame.toStdString(),target_frame.toStdString(),
                                         osg::Quat(quat.x(),quat.y(),quat.z(),quat.scalar()),
                                         osg::Vec3d(position.x(),position.y(),position.z()));

    if(count != getVisualizationFrames().size())
    {
        emit propertyChanged("frame");
        PluginMap::iterator it = plugins.begin();
        for(;it != plugins.end();++it)
            it->first->setVisualizationFrame(it->first->getVisualizationFrame());
    }

    if(!root_frame.isEmpty())
        TransformerGraph::makeRoot(*getRootNode(), root_frame.toStdString());
}

void Vizkit3DWidget::setRootFrame(QString frame)
{
    TransformerGraph::makeRoot(*getRootNode(), frame.toStdString());
    root_frame = frame;
}

void Vizkit3DWidget::getTransformation(const QString &source_frame,const QString &target_frame, QVector3D &position, QQuaternion &orientation)const
{
    osg::Quat quat; osg::Vec3d t;
    TransformerGraph::getTransformation(*getRootNode(),source_frame.toStdString(),target_frame.toStdString(),quat,t);
    position = QVector3D(t[0],t[1],t[2]);
    orientation = QQuaternion(quat.w(),quat.x(),quat.y(),quat.z());
}

QString Vizkit3DWidget::getWorldName()const
{
    return QString(TransformerGraph::getWorldName(*getRootNode()).c_str());
}

void Vizkit3DWidget::displayTransformGraph() const
{
    TransformerGraph::GraphDescription description = TransformerGraph::getGraphDescription(*getRootNode());
    std::cerr << description.size() << " edges in transform graph" << std::endl;
    std::cerr << "Root frame: " << getRootVisualizationFrame().toStdString() << std::endl;
    for (list<TransformerGraph::EdgeDescription>::const_iterator it = description.begin();
            it != description.end(); ++it)
    {
        std::cout << "  " << it->first << " -> " << it->second << std::endl;
    }

}

bool Vizkit3DWidget::isTransformer() const
{
    return TransformerGraph::areFrameAnnotationVisible(*getRootNode());
}

void Vizkit3DWidget::setTransformer(bool value)
{
    TransformerGraph::showFrameAnnotation(*getRootNode(),value);
    emit propertyChanged("transformer");
}

float Vizkit3DWidget::getTransformerTextSize() const
{
    return TransformerGraph::getTextSize(*getRootNode());
}

void Vizkit3DWidget::setTransformerTextSize(float size)
{
    TransformerGraph::setTextSize(*getRootNode(),size);
    emit propertyChanged("transformerTextSize");
}

void Vizkit3DWidget::setAxesLabels(bool value)
{
    osg::Node *node = FindNode::find(*getRootNode(),"axes_node");
    if(!node)
        return;
    osg::Switch *switch_node = node->asSwitch();
    if(switch_node)
        AxesNode::displayLabels(switch_node->getChild(0),value);
}


bool Vizkit3DWidget::isAxesLabels() const
{
    osg::Node *node = FindNode::find(*getRootNode(),"axes_node");
    if(!node)
        return false;
    osg::Switch *switch_node = node->asSwitch();
    if(switch_node)
        return AxesNode::hasLabels(switch_node->getChild(0));
    return false;
}

bool Vizkit3DWidget::isAxes() const
{
    osg::Node *node = FindNode::find(*getRootNode(),"axes_node");
    if(node && node->asSwitch())
        return node->asSwitch()->getValue(0);
    return false;
}

void Vizkit3DWidget::setAxes(bool value)
{
    osg::Node *node = FindNode::find(*getRootNode(),"axes_node");
    if(node && node->asSwitch())
    {
        if(value)
            node->asSwitch()->setAllChildrenOn();
        else
            node->asSwitch()->setAllChildrenOff();
    }
    emit propertyChanged("axes");
}

QString Vizkit3DWidget::findPluginPath(QString plugin_name)
{
    QStringList *list = getAvailablePlugins();
    QStringList::iterator iter = list->begin();
    for(;iter != list->end();++iter)
    {
        QStringList plugin = iter->split("@");
        if(plugin.at(0) == plugin_name)
            return plugin.at(1);
    }
    return QString();
}

QString Vizkit3DWidget::findLibPath(QString lib_name)
{
    QStringList *list = getAvailablePlugins();
    QStringList::iterator iter = list->begin();
    for(;iter != list->end();++iter)
    {
        QStringList plugin = iter->split("@");
        QRegExp rx(".*lib"+lib_name+"-viz\\..{2,5}$");
        if(0 <= rx.indexIn(plugin.at(1)))
            return plugin.at(1);
    }
    return QString();
}

QObject* Vizkit3DWidget::loadLib(QString file_path)
{
    QPluginLoader loader(file_path);
    loader.load();
    if(!loader.isLoaded())
        throw std::runtime_error("Cannot load " + file_path.toStdString() + ". Last error is :" + loader.errorString().toStdString());
    QObject* plugin_instance = loader.instance();
    if(!plugin_instance)
        throw std::runtime_error("Cannot load " + file_path.toStdString() + ". Last error is :" + loader.errorString().toStdString());
    return plugin_instance;
}

QStringList* Vizkit3DWidget::getAvailablePlugins()
{
    // qt ruby is crashing if not a pointer is returned
    QStringList *plugins = new QStringList;

    QStringList name_filters;
    name_filters << "lib*-viz.so" << "lib*-viz.dylib" << "lib*-viz.dll";

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString path_string = env.value("VIZKIT_PLUGIN_RUBY_PATH","/usr/local/lib:/usr/lib");
    QStringList paths = path_string.split(":");
    QStringList::iterator iter = paths.begin();
    for(;iter != paths.end();++iter)
    {
        QDir dir(*iter);
        if(!dir.exists())
            continue;
        QStringList files = dir.entryList(name_filters,QDir::Files);
        QStringList::iterator iter2 = files.begin();
        for(;iter2 != files.end();++iter2)
        {
            QFileInfo file_info(dir, *iter2);
            try
            {
                QObject * qt_plugin = loadLib(file_info.absoluteFilePath());
                VizkitPluginFactory *factory = dynamic_cast<VizkitPluginFactory*>(qt_plugin);
                if(!factory)
                    throw std::runtime_error((file_info.absoluteFilePath() + " is not a VizkitPluginFactory!").toStdString());
                QStringList* lib_plugins  = factory->getAvailablePlugins();
                QStringList::iterator iter3 = lib_plugins->begin();
                for(;iter3 != lib_plugins->end();++iter3)
                    *plugins << QString(*iter3 + "@" + file_info.absoluteFilePath());
            }
            catch(std::runtime_error e)
            {
                std::cerr << "WARN: cannot load vizkit plugin library " << e.what() << std::endl;
            }
        }
    }
    return plugins;
}

QObject* Vizkit3DWidget::loadPlugin(QString lib_name,QString plugin_name)
{
    //check if the plugin name is encoded into the lib_name
    QStringList plugin_strings = lib_name.split("@");
    if(plugin_strings.size() == 2)
    {
        lib_name = plugin_strings.at(0);
        plugin_name = plugin_strings.at(1);
    }

    //if no lib_name is given try to find it from plugin_name
    if(lib_name.isEmpty() && !plugin_name.isEmpty())
        lib_name = findPluginPath(plugin_name);

    //check if the lib name is a path
    QFileInfo file_info(lib_name);
    QString path;
    if(file_info.isFile())
        path = file_info.absolutePath();
    else
        path = findLibPath(lib_name);

    if(path.isEmpty())
    {
        std::cerr << "cannot find lib" + lib_name.toStdString()+"-viz in VIZKIT_PLUGIN_RUBY_PATH." << std::endl;
        return NULL;
    }

    VizkitPluginFactory *lib= dynamic_cast<VizkitPluginFactory*>(loadLib(path));
    if(plugin_name.isEmpty())
    {
        if(lib->getAvailablePlugins()->size() > 1)
        {
            std::cerr << lib_name.toStdString()+" defines multiple plugins (and you must select one explicitly)" << std::endl;
            return NULL;
        }
        else
            plugin_name = lib->getAvailablePlugins()->front();
    }
    QObject *plugin = lib->createPlugin(plugin_name);
    if(plugin == NULL)
    {
        std::cerr << "library " << lib_name.toStdString() << " does not have any vizkit plugin called " << 
                      plugin_name.toStdString() << ", available plugins are: " << 
                      lib->getAvailablePlugins()->join(", ").toStdString() << std::endl;
        return NULL;
    }
    addPlugin(plugin);
    return plugin;
}

QString Vizkit3DWidget::getCameraManipulatorName() const
{
    if (getCameraManipulator() == NODE_TRACKER_MANIPULATOR)
        return tracked_object_name;
    else
        return Vizkit3DConfig::manipulatorIDToName(getCameraManipulator());
}

CAMERA_MANIPULATORS Vizkit3DWidget::getCameraManipulator() const
{
    return current_manipulator;
}

void Vizkit3DWidget::setCameraManipulator(osg::ref_ptr<osgGA::CameraManipulator> manipulator, bool resetToDefaultHome)
{
    osgViewer::View *view = getView(0);
    assert(view);

    osg::Vec3d
        eye = DEFAULT_EYE,
        center = DEFAULT_CENTER,
        up = DEFAULT_UP;

    osgGA::CameraManipulator* current = view->getCameraManipulator();
    if (!resetToDefaultHome && current)
        current->getHomePosition(eye, center, up);

    if (manipulator)
        manipulator->setHomePosition(eye, center, up);

    view->setCameraManipulator(manipulator);
    view->home();
}

void Vizkit3DWidget::setCameraManipulator(QString manipulator, bool resetToDefaultHome)
{
    return setCameraManipulator(Vizkit3DConfig::manipulatorNameToID(manipulator), resetToDefaultHome);
}

void Vizkit3DWidget::setCameraManipulator(CAMERA_MANIPULATORS manipulatorType, bool resetToDefaultHome)
{
    osg::ref_ptr<osgGA::CameraManipulator> newManipulator;
    switch(manipulatorType)
    {
        case DEFAULT_MANIPULATOR:
            newManipulator = new vizkit3d::DefaultManipulator;
            break;
        case FIRST_PERSON_MANIPULATOR:
            newManipulator = new osgGA::FirstPersonManipulator;
            break;
        case FLIGHT_MANIPULATOR:
            newManipulator = new osgGA::FlightManipulator;
            break;
        case ORBIT_MANIPULATOR:
            newManipulator = new osgGA::OrbitManipulator;
            break;
        case TERRAIN_MANIPULATOR:
            newManipulator = new osgGA::TerrainManipulator;
            break;
        case TRACKBALL_MANIPULATOR:
            newManipulator = new osgGA::TrackballManipulator;
            break;
        case MULTI_TOUCH_TRACKBALL_MANIPULATOR:
            newManipulator = new osgGA::MultiTouchTrackballManipulator;
            break;
        case NODE_TRACKER_MANIPULATOR:
            throw std::invalid_argument("cannot set the manipulaor to NODE_TRACKER_MANIPULATOR using setCameraManipulator, use setTrackedNode instead");
        case NO_MANIPULATOR:
            // NULL
            break;
        default:
            throw std::invalid_argument("invalid camera manipulator type provided");
    };

    setCameraManipulator(newManipulator);
    current_manipulator = manipulatorType;
    emit propertyChanged("manipulator");

    if (current_frame != getRootVisualizationFrame())
    {
        current_frame = getRootVisualizationFrame();
        emit propertyChanged("frame");
    }
}

