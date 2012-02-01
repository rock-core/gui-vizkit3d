#include "Vizkit3DWidget.hpp"
#include <QVBoxLayout>
#include <QSplitter>

using namespace vizkit;

Vizkit3DWidget::Vizkit3DWidget( QWidget* parent, Qt::WindowFlags f )
    : CompositeViewerQOSG( parent, f )
{
    createSceneGraph();

    QWidget* viewWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    layout->addWidget( splitter );
    this->setLayout( layout );
    
    // create propertyBrowserWidget
    propertyBrowserWidget = new QProperyBrowserWidget( parent );
    propertyBrowserWidget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    splitter->addWidget(propertyBrowserWidget);

    view = new ViewQOSG( viewWidget );
    view->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    view->setData( root );
    addView( view );
    splitter->addWidget(viewWidget);

    // pickhandler is for selecting objects in the opengl view
    pickHandler = new PickHandler();
    view->addEventHandler( pickHandler );
    
    // set root node as default tracked node
    view->setTrackedNode(getRootNode());
    
    // add visualization of ground grid 
    groundGrid = new GridNode();
    root->addChild(groundGrid);
    
    // create visualization of the coordinate axes
    coordinateFrame = new CoordinateFrame();
    
    pluginNames = new QStringList;
    
    changeCameraView(osg::Vec3d(0,0,0), osg::Vec3d(0,-5,5));
    
    // add some properties of this widget as global properties
    QStringList property_names("show_grid");
    property_names.push_back("show_axes");
    propertyBrowserWidget->addGlobalProperties(this, property_names);
    
    connect(this, SIGNAL(addPlugins(QObject*,QObject*)), this, SLOT(addPluginIntern(QObject*,QObject*)), Qt::QueuedConnection);
    connect(this, SIGNAL(removePlugins(QObject*)), this, SLOT(removePluginIntern(QObject*)), Qt::QueuedConnection);
}

QSize Vizkit3DWidget::sizeHint() const
{
    return QSize( 1000, 600 );
}

osg::ref_ptr<osg::Group> Vizkit3DWidget::getRootNode() const
{
    return root;
}

void Vizkit3DWidget::setTrackedNode( VizPluginBase* plugin )
{
    view->setTrackedNode(plugin->getVizNode());
}

void Vizkit3DWidget::createSceneGraph() 
{
    //create root node that holds all other nodes
    root = new osg::Group;
    
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
    //   MatrixTransform to the scene graph.
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
	//ls->setStateSetModes(*state, osg::StateAttribute::ON);
	root->addChild( ls.get() );
    }
}

void Vizkit3DWidget::addDataHandler(VizPluginBase *viz)
{
    root->addChild( viz->getVizNode() );
}

void Vizkit3DWidget::removeDataHandler(VizPluginBase *viz)
{
    root->removeChild( viz->getVizNode() );
}

void Vizkit3DWidget::changeCameraView(const osg::Vec3& lookAtPos)
{
    changeCameraView(&lookAtPos, 0, 0);
}

void Vizkit3DWidget::changeCameraView(const osg::Vec3& lookAtPos, const osg::Vec3& eyePos)
{
    changeCameraView(&lookAtPos, &eyePos, 0);
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

void Vizkit3DWidget::changeCameraView(const osg::Vec3* lookAtPos, const osg::Vec3* eyePos, const osg::Vec3* upVector)
{
    osgGA::KeySwitchMatrixManipulator* switchMatrixManipulator = dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(view->getCameraManipulator());
    if (!switchMatrixManipulator) return;
    //select TerrainManipulator
    switchMatrixManipulator->selectMatrixManipulator(3);
    
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
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    if (viz_plugin)
    {
        //pass ownership to c++
        //QT-Ruby honors the parent pointer
        //if it is set, the Object will not get autodeleted
        plugin->setParent(this);
        
        plugins.push_back(viz_plugin);
	
	if(pluginToTransformData.count(viz_plugin))
	    throw std::runtime_error("Error added same plugin twice");
	
	pluginToTransformData[viz_plugin] = TransformationData();

        addDataHandler(viz_plugin);
        propertyBrowserWidget->addProperties(viz_plugin,parent);
        connect(viz_plugin, SIGNAL(pluginActivityChanged(bool)), this, SLOT(pluginActivityChanged(bool)));
    }

    // add sub plugins if object has some
    QList<QObject*> object_list = plugin->findChildren<QObject *>();
    for(QList<QObject*>::const_iterator it =object_list.begin(); it != object_list.end(); it++)
        addPluginIntern(*it,plugin);
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
        removeDataHandler(viz_plugin);
        propertyBrowserWidget->removeProperties(viz_plugin);
        disconnect(viz_plugin, SIGNAL(pluginActivityChanged(bool)), this, SLOT(pluginActivityChanged(bool)));
        
        std::vector<vizkit::VizPluginBase *>::iterator it = std::find(plugins.begin(), plugins.end(), viz_plugin);
        if(it == plugins.end())
            throw std::runtime_error("Tried to remove an vizkit3d plugin that was not registered before");

	pluginToTransformData.erase(viz_plugin);
	
        plugins.erase(it);
    }
}


void Vizkit3DWidget::setPluginDataFrame(const std::string& frame, QObject* plugin)
{
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    if(!viz_plugin)
	throw std::runtime_error("setPluginDataFrame called with something that is no vizkit plugin");
    
    if(pluginToTransformData.count(viz_plugin) == 0)
        throw std::runtime_error("Tried to set frame for unknown plugin");
    
    TransformationData td = pluginToTransformData[viz_plugin]; 
    
    if(!td.dataFrame.empty())
    {
        transformer.unregisterTransformation(td.transformation);
    }
    
    td.dataFrame = frame;
    
    if(!displayFrame.empty())
        td.transformation = &transformer.registerTransformation(td.dataFrame, displayFrame);
    else
        td.transformation = NULL;
    
    pluginToTransformData[viz_plugin] = td;
}

void Vizkit3DWidget::setVizualisationFrame(const std::string& frame)
{
    displayFrame = frame;
    
    for(std::map<vizkit::VizPluginBase *, TransformationData>::iterator it = pluginToTransformData.begin(); it != pluginToTransformData.end(); it++)
    {
        TransformationData &data(it->second);
        
        if(!data.dataFrame.empty())
        {
            if(data.transformation)
                transformer.unregisterTransformation(data.transformation);
            
            data.transformation = &transformer.registerTransformation(data.dataFrame, displayFrame);
            
            it->second = data;
        }
    }
    
}

void Vizkit3DWidget::pushDynamicTransformation(const base::samples::RigidBodyState& tr)
{
    transformer.pushDynamicTransformation(tr);
    while(transformer.step())
    {
	;
    }

    for(std::map<vizkit::VizPluginBase *, TransformationData>::iterator it = pluginToTransformData.begin(); it != pluginToTransformData.end(); it++)
    {
        TransformationData &data(it->second);
	if(data.transformation)
	{
	    Eigen::Affine3d pose;
	    if(data.transformation->get(tr.time, pose, false))
	    {
		it->first->setPose(pose.translation(), Eigen::Quaterniond(pose.rotation()));
	    }
	}
    }

}

void Vizkit3DWidget::pushStaticTransformation(const base::samples::RigidBodyState& tr)
{
    transformer.pushStaticTransformation(tr);
}

/**
 * Adds or removes a plugin if the plugin activity 
 * has changed.
 * @param enabled 
 */
void Vizkit3DWidget::pluginActivityChanged(bool enabled)
{
    QObject* obj = QObject::sender();
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(obj);
    if(viz_plugin)
    {
        // check if root node has plugin as child
        bool has_child_plugin = false;
        if(root->getChildIndex(viz_plugin->getVizNode()) < root->getNumChildren())
            has_child_plugin = true;
        
        // add or remove plugin from root node
        if(enabled && !has_child_plugin)
        {
            addDataHandler(viz_plugin);
        }
        else if (!enabled && has_child_plugin)
        {
            removeDataHandler(viz_plugin);
        }
    }
}

/**
 * @return property browser widget
 */
QWidget* Vizkit3DWidget::getPropertyWidget()
{
    return propertyBrowserWidget;
}

/**
 * @return true if ground grid is enabled
 */
bool Vizkit3DWidget::isGridEnabled()
{
    return (root->getChildIndex(groundGrid) < root->getNumChildren());
}

/**
 * Enable or disable ground grid.
 * @param enabled
 */
void Vizkit3DWidget::setGridEnabled(bool enabled)
{
    if(!enabled && isGridEnabled())
    {
        root->removeChild(groundGrid);
    }
    else if(enabled && !isGridEnabled())
    {
        root->addChild(groundGrid);
    }
    emit propertyChanged("show_grid");
}

/**
 * @return true if axes coordinates are enabled
 */
bool Vizkit3DWidget::areAxesEnabled()
{
    return (root->getChildIndex(coordinateFrame) < root->getNumChildren());
}

/**
 * Enable or disable axes of the coordinate system.
 * @param enabled
 */
void Vizkit3DWidget::setAxesEnabled(bool enabled)
{
    if(!enabled && areAxesEnabled())
    {
        root->removeChild(coordinateFrame);
    }
    else if(enabled && !areAxesEnabled())
    {
        root->addChild(coordinateFrame);
    }
    emit propertyChanged("show_axes");
}
