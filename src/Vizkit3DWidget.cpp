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

#include "Vizkit3DWidget.hpp"
#include "Vizkit3DPlugin.hpp"
#include "PickHandler.hpp"
#include "QPropertyBrowserWidget.hpp"
#include "AxesNode.hpp"
#include "OsgVisitors.hpp"
#include "TransformerGraph.hpp"

#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerEventHandlers>

using namespace vizkit3d;
using namespace std;

Vizkit3DConfig::Vizkit3DConfig(QObject *parent):QObject(parent)
{
    setObjectName("Viewer");
    connect(parent, SIGNAL(propertyChanged(QString)),this,SIGNAL(propertyChanged(QString)));
}

bool Vizkit3DConfig::isAxes() const
{
    Vizkit3DWidget *parent = dynamic_cast<Vizkit3DWidget*>(this->parent());
    if(!parent)
        return false;
    return parent->isAxes();
}

void Vizkit3DConfig::setAxes(bool value)
{
    Vizkit3DWidget *parent = dynamic_cast<Vizkit3DWidget*>(this->parent());
    if(!parent)
        return;
    parent->setAxes(value);
}

QStringList Vizkit3DConfig::getVisualizationFrames() const
{
    Vizkit3DWidget *parent = dynamic_cast<Vizkit3DWidget*>(this->parent());
    if(!parent)
        return QStringList();
    return *parent->getVisualizationFrames();
}

void Vizkit3DConfig::setVisualizationFrame(const QStringList &frames)
{
    Vizkit3DWidget *parent = dynamic_cast<Vizkit3DWidget*>(this->parent());
    if(!parent && frames.isEmpty())
        return;
    return parent->setVisualizationFrame(frames.front(),false);
}

bool Vizkit3DConfig::isTransformer() const
{
    Vizkit3DWidget *parent = dynamic_cast<Vizkit3DWidget*>(this->parent());
    if(!parent)
        return false;
    return parent->isTransformer();
}

void Vizkit3DConfig::setTransformer(bool value)
{
    Vizkit3DWidget *parent = dynamic_cast<Vizkit3DWidget*>(this->parent());
    if(!parent)
        return;
    parent->setTransformer(value);
}

Vizkit3DWidget::Vizkit3DWidget( QWidget* parent)
    : QWidget(parent)
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

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    // create root scene node
    root = createSceneGraph();

    // create osg widget
    QWidget* widget = addViewWidget(createGraphicsWindow(0,0,800,600), root);
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
    _timer.start(10);
}

Vizkit3DWidget::~Vizkit3DWidget() {}

//qt ruby is crashing if we use none pointer here
QStringList* Vizkit3DWidget::getVisualizationFrames() const
{
    QStringList *list = new QStringList;
    std::vector<std::string> std_list = TransformerGraph::getFrameNames(*getRootNode());
    std::vector<std::string>::iterator iter = std_list.begin();
    for(;iter != std_list.end();++iter)
       *list << QString(iter->c_str());
    if(!current_frame.isEmpty())
    {
        list->removeOne(current_frame);
        list->prepend(current_frame);
    }
    return list;
}

QString Vizkit3DWidget::getVisualizationFrame() const
{
    return current_frame;
}

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
    osg::Group *root = TransformerGraph::create("worldOpenScenGraph")->asGroup();
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
            light->setDiffuse( osg::Vec4( .1f, .3f, .1f, 1.f ));
            light->setSpecular( osg::Vec4( .1f, .3f, .1f, 1.f ));
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

    disableDataHandler(viz);
    plugins.erase(it);
}

void Vizkit3DWidget::enableDataHandler(VizPluginBase *viz)
{
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
    QSplitter *splitter = findChild<QSplitter*>("splitter");
    if(!splitter)
        return;
    QList<int> sizes;
    sizes.push_front(0);
    splitter->setSizes(sizes);
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
        if(viz->isPluginEnabled())
        {
            disableDataHandler(viz);
            it->second = node;
            enableDataHandler(viz);
        }
        else
            it->second = node;
    }
}

void Vizkit3DWidget::setVisualizationFrame(const QString& frame,bool update)
{
    osgViewer::View *view = getView(0);
    assert(view);
    // the following is not working if the directly track the transformation 
    // therefore use a child
    osg::Node *node = TransformerGraph::getFrameGroup(*getRootNode(),frame.toStdString());
    assert(node);

    if(frame.size()==0 || frame == QString(getRootNode()->getName().c_str()))
    {
        osgGA::TerrainManipulator* manipulator = new osgGA::TerrainManipulator;
        manipulator->setHomePosition(osg::Vec3(-5, 0, 5), osg::Vec3(0,0,0), osg::Vec3(0,0,1));
        view->setCameraManipulator(manipulator);
    }
    else
    {
        osgGA::NodeTrackerManipulator* manipulator = new osgGA::NodeTrackerManipulator;
        view->setCameraManipulator(manipulator);
        manipulator->setTrackNode(node);
        manipulator->setHomePosition(osg::Vec3(-5, 0, 5), osg::Vec3(0,0,0), osg::Vec3(0,0,1));
        manipulator->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);
    }
    current_frame = frame;
    view->home();

    if(update)
        emit propertyChanged("frame");
}

void Vizkit3DWidget::setTransformation(const QString &source_frame,const QString &target_frame,
        const QVector3D &_position, const QQuaternion &_quat)
{
    // enforce length of 1
    QQuaternion quat = _quat.normalized();
    // enforce valid position
    QVector3D position = _position;
    if(isnan(position.x()) ||isnan(position.y()) || isnan(position.z()))
        position = QVector3D();
    int count = getVisualizationFrames()->size();
    TransformerGraph::setTransformation(*getRootNode(),source_frame.toStdString(),target_frame.toStdString(),
                                         osg::Quat(quat.x(),quat.y(),quat.z(),quat.scalar()),
                                         osg::Vec3d(position.x(),position.y(),position.z()));

    if(count != getVisualizationFrames()->size())
    {
        emit propertyChanged("frame");
        PluginMap::iterator it = plugins.begin();
        for(;it != plugins.end();++it)
            it->first->setVisualizationFrame(it->first->getVisualizationFrame());
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

