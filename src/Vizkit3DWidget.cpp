#include "Vizkit3DWidget.hpp"
#include <QVBoxLayout>
#include <QSplitter>
#include <QComboBox>
#include <QGroupBox>
#include <QPlastiqueStyle>

#include <vizkit/QOSGWidget.hpp>
#include <vizkit/Vizkit3DPlugin.hpp>
#include <vizkit/GridNode.hpp>
#include <vizkit/CoordinateFrame.hpp>
#include <vizkit/PickHandler.hpp>
#include <vizkit/QPropertyBrowserWidget.hpp>
#include <algorithm>

using namespace vizkit;
using namespace std;

Vizkit3DWidget::Vizkit3DWidget( QWidget* parent, Qt::WindowFlags f )
    : CompositeViewerQOSG( parent, f )
{
    createSceneGraph();

    QWidget* viewWidget = new QWidget;
    viewWidget->setObjectName(QString("View Widget"));
    QWidget* controlWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    splitter = new QSplitter(Qt::Horizontal);
    layout->addWidget( splitter );
    this->setLayout( layout );

    QVBoxLayout* controlLayout = new QVBoxLayout;
    controlWidget->setLayout(controlLayout);
    
    frameSelector = new QComboBox();
    groupBox = new QGroupBox();
    QVBoxLayout* groupBoxLayout = new QVBoxLayout;
    groupBox->setLayout(groupBoxLayout);
    groupBox->setTitle("Select Visualization Frame");
    groupBox->setEnabled(false);
    QPlastiqueStyle* style = new QPlastiqueStyle;
    groupBox->setStyle(style);
    groupBoxLayout->addWidget(frameSelector);
    controlLayout->addWidget(groupBox);

    // create propertyBrowserWidget
    propertyBrowserWidget = new QProperyBrowserWidget( parent );
    propertyBrowserWidget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    controlLayout->addWidget(propertyBrowserWidget);
    splitter->addWidget(controlWidget);
    
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
    
    changeCameraView(osg::Vec3d(0,0,0), osg::Vec3d(-5,0,5));
    
    // add some properties of this widget as global properties
    QStringList property_names("show_grid");
    property_names.push_back("show_axes");
    propertyBrowserWidget->addGlobalProperties(this, property_names);
    
    initalDisplayFrame = "";
    
    connect(this, SIGNAL(addPlugins(QObject*,QObject*)), this, SLOT(addPluginIntern(QObject*,QObject*)));
    connect(this, SIGNAL(removePlugins(QObject*)), this, SLOT(removePluginIntern(QObject*)));
    connect(frameSelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(setVisualizationFrame(QString)));

}

Vizkit3DWidget::~Vizkit3DWidget() {}

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
    view->setTrackedNode(plugin->getRootNode());
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

void Vizkit3DWidget::pluginDeleted(QObject* plugin)
{
    PluginMap::iterator it = plugins.find(static_cast<VizPluginBase*>(plugin));
    if (it != plugins.end())
        plugins.erase(it);
    pluginToTransformData.erase(static_cast<VizPluginBase*>(plugin));
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
void Vizkit3DWidget::collapsePropertyBrowser()
{
    QList<int> sizes;
    sizes.push_front(0);
    splitter->setSizes(sizes);
}

void Vizkit3DWidget::changeCameraView(const osg::Vec3* lookAtPos, const osg::Vec3* eyePos, const osg::Vec3* upVector)
{
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
* Returns the pointer of the instance of the used viewer, 
* which is also of the ViewerQOSG type.
*/
osg::ref_ptr<ViewQOSG> Vizkit3DWidget::getViewer()
{
    return view;
}

/**
 * This slot adds all plugins in the list to the OSG and
 * their properties to the property browser widget.
 */
void Vizkit3DWidget::addPluginIntern(QObject* plugin,QObject *parent)
{
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    bool has_plugin = plugins.find(viz_plugin) != plugins.end();
    if (viz_plugin && !has_plugin)
    {
        // Make sure that the plugins do have a parent.
        //
        // Do NOT reset the parent flag if it already has one !
        if (!viz_plugin->parent())
            viz_plugin->setParent(view);
        
	if(pluginToTransformData.count(viz_plugin))
	    throw std::runtime_error("Error added same plugin twice");
	
	pluginToTransformData[viz_plugin] = TransformationData();

        registerDataHandler(viz_plugin);
        setPluginEnabled(viz_plugin, viz_plugin->isPluginEnabled());
        propertyBrowserWidget->addProperties(viz_plugin,parent);
        connect(viz_plugin, SIGNAL(pluginActivityChanged(bool)), this, SLOT(pluginActivityChanged(bool)));
        connect(viz_plugin, SIGNAL(childrenChanged()), this, SLOT(pluginChildrenChanged()));
        connect(viz_plugin, SIGNAL(destroyed(QObject*)), this, SLOT(pluginDeleted(QObject*)));
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
        propertyBrowserWidget->removeProperties(viz_plugin);
        disconnect(viz_plugin, SIGNAL(pluginActivityChanged(bool)), this, SLOT(pluginActivityChanged(bool)));
        disconnect(viz_plugin, SIGNAL(childrenChanged()), this, SLOT(pluginChildrenChanged()));
	pluginToTransformData.erase(viz_plugin);
    }
}


void Vizkit3DWidget::setPluginDataFrame(const QString& frame, QObject* plugin)
{
    vizkit::VizPluginBase* viz_plugin = dynamic_cast<vizkit::VizPluginBase*>(plugin);
    if(!viz_plugin)
	throw std::runtime_error("setPluginDataFrame called with something that is no vizkit plugin");
    
    if(pluginToTransformData.count(viz_plugin) == 0)
        throw std::runtime_error("Tried to set frame for unknown plugin");
    
    TransformationData td = pluginToTransformData[viz_plugin]; 
    
    if(td.dataFrame == frame.toStdString())
	return;

    if(!td.dataFrame.empty())
    {
        transformer.unregisterTransformation(td.transformation);
    }
    
    td.dataFrame = frame.toStdString();
    
    if(!displayFrame.empty())
        td.transformation = &transformer.registerTransformation(td.dataFrame, displayFrame);
    else
        td.transformation = NULL;
    
    pluginToTransformData[viz_plugin] = td;
}

void Vizkit3DWidget::setVisualizationFrame(const QString& frame)
{
    displayFrame = frame.toStdString();
    
    if(initalDisplayFrame.empty())
        initalDisplayFrame = displayFrame;
    
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
    updateTransformations();
}

void Vizkit3DWidget::pushDynamicTransformation(const base::samples::RigidBodyState& tr)
{
    if(!tr.hasValidPosition() || !tr.hasValidOrientation())
    {
        std::cerr << "Vizkit3DWidget ignoring invalid dynamic transformation " << tr.sourceFrame << " --> " << tr.targetFrame << std::endl;
        return;
    }

    checkAddFrame(tr.sourceFrame);
    checkAddFrame(tr.targetFrame);

    transformer.pushDynamicTransformation(tr);
    while(transformer.step())
    {
	;
    }
    updateTransformations();
}

void Vizkit3DWidget::updateTransformations()
{
    for(std::map<vizkit::VizPluginBase *, TransformationData>::iterator it = pluginToTransformData.begin(); it != pluginToTransformData.end(); it++)
    {
        TransformationData &data(it->second);
	if(data.transformation)
	{
	    Eigen::Affine3d pose;
	    if(data.transformation->get(base::Time(), pose, false))
	    {
		it->first->setPose(pose.translation(), Eigen::Quaterniond(pose.rotation()));
	    }
	}
    }
}

void Vizkit3DWidget::pushStaticTransformation(const base::samples::RigidBodyState& tr)
{
    if(!tr.hasValidPosition() || !tr.hasValidOrientation())
    {
        std::cerr << "Vizkit3DWidget ignoring invalid static transformation " << tr.sourceFrame << " --> " << tr.targetFrame << std::endl;
        return;
    }
    checkAddFrame(tr.sourceFrame);
    checkAddFrame(tr.targetFrame);
    transformer.pushStaticTransformation(tr);
}

void Vizkit3DWidget::checkAddFrame(const std::string& frame)
{
    std::map<std::string, bool>::iterator it;
    it = availableFrames.find(frame);
    if(it == availableFrames.end())
    {
	availableFrames[frame] = true;
	frameSelector->addItem(QString::fromStdString(frame));
	if(frame == initalDisplayFrame)
	{
	    int index = frameSelector->findText(QString::fromStdString(frame));
	    if(index != -1) {
		frameSelector->setCurrentIndex(index);
	    }
	}
	// enable frame selector groupBox if needed
	if(!groupBox->isEnabled())
        {
            groupBox->setEnabled(true);
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
