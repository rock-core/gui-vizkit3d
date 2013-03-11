#include <osg/Group>
//#include <osg/Text>
#include <typeinfo>
#include <cxxabi.h>
#include "Vizkit3DPlugin.hpp"
#include "Vizkit3DHelper.hpp"
#include "PickHandler.hpp"

using namespace vizkit;

/** this adapter is used to forward the update call to the plugin
 */
struct VizPluginBase::CallbackAdapter : public osg::NodeCallback
{
    VizPluginBase* plugin;
    CallbackAdapter( VizPluginBase* plugin ) : plugin( plugin ) {}
    void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
	plugin->updateCallback( node );
	osg::NodeCallback::operator()(node, nv);
    }
};

VizPluginBase::VizPluginBase(QObject *parent)
    : QObject(parent), oldNodes(NULL), isAttached(false), dirty( false ),  plugin_enabled(true),
    keep_old_data(false),max_old_data(100)
{
    position.setZero();
    orientation = Eigen::Quaterniond::Identity();
    rootNode = new osg::Group();
    nodeCallback = new CallbackAdapter(this);
    rootNode->setUpdateCallback(nodeCallback);
    vizNode = new osg::PositionAttitudeTransform();
    rootNode->addChild(vizNode);
    oldNodes = new osg::Group();
    rootNode->addChild(oldNodes);
    
    // Add plugin name to root node as custom user text
    //rootNode->setUserData(new osgText::String("MyNode"));
    rootNode->setUserData(new osg::Geode);
}

osg::ref_ptr<osg::Group> VizPluginBase::getVizNode() const 
{
    return vizNode;
}

osg::ref_ptr<osg::Group> VizPluginBase::getRootNode() const 
{
    return rootNode;
}

void VizPluginBase::setPose(const base::Vector3d& position, const base::Quaterniond& orientation)
{
    boost::mutex::scoped_lock lock(updateMutex);
    
    this->position = position;
    this->orientation = orientation;    
}

const QString VizPluginBase::getPluginName() const 
{
    if(vizkit3d_plugin_name.isEmpty())
        return abi::__cxa_demangle(typeid(*this).name(), 0, 0, 0);
    else
        return vizkit3d_plugin_name;
}

void VizPluginBase::setPluginName(const QString &name)
{
    vizkit3d_plugin_name = name;
    emit propertyChanged("vizkit3d_plugin_name");
}

osg::ref_ptr<osg::Node> VizPluginBase::createMainNode()
{
    osg::Group* main_node = new osg::Group;
    //rootNode->setUserData(new PickedUserData(this));
    
    return main_node;
}

std::vector< QDockWidget* > VizPluginBase::getDockWidgets()
{
    if (dockWidgets.empty()) createDockWidgets();
    return dockWidgets;
}

void VizPluginBase::createDockWidgets()
{

}

void VizPluginBase::clearVisualization()
{
    vizNode->removeChild(mainNode);
    isAttached = false;
}

void VizPluginBase::updateCallback(osg::Node* node)
{
    boost::mutex::scoped_lock lockit(updateMutex);

    vizNode->setPosition(eigenVectorToOsgVec3(position));
    vizNode->setAttitude(eigenQuatToOsgQuat(orientation));
    
    if (!mainNode)
    {
        mainNode = createMainNode();
        vizNode->addChild(mainNode);
	isAttached = true;
    }

    if( isDirty() )
    {
	updateMainNode(mainNode);
        if(keep_old_data)
        {
            oldNodes->addChild(cloneCurrentViz());
            if(oldNodes->getNumChildren() > max_old_data)
                oldNodes->removeChild(0,oldNodes->getNumChildren() -max_old_data);
        }
        if(!isAttached)
	{
	    vizNode->addChild(mainNode);
	    isAttached = true;
	}
	dirty = false;
    }
}

bool VizPluginBase::isDirty() const
{
    return dirty;
}

void VizPluginBase::setDirty() 
{
    dirty = true;
}

QObject* vizkit::VizPluginBase::getRubyAdapterCollection()
{
    return &adapterCollection;
}

bool VizPluginBase::isPluginEnabled()
{
    return plugin_enabled;
}

void VizPluginBase::setPluginEnabled(bool enabled)
{
    plugin_enabled = enabled;
    emit pluginActivityChanged(enabled);
    emit propertyChanged("enabled");
}

osg::ref_ptr<osg::Node> VizPluginBase::cloneCurrentViz()
{
    return (osg::Node*)getVizNode()->clone(osg::CopyOp::DEEP_COPY_ALL);
}

void VizPluginBase::setKeepOldData(bool value)
{
    keep_old_data = value;
    if(!value)
        deleteOldData();
    emit propertyChanged("KeepOldData");
}

void VizPluginBase::setMaxOldData(int value )
{
    if(value < 0)
        value = 0;
    max_old_data=(unsigned int) value;
    emit propertyChanged("MaxOldData");
}

bool VizPluginBase::isKeepOldDataEnabled()
{
    return keep_old_data;
}

void VizPluginBase::deleteOldData()
{
    oldNodes->removeChild(0,oldNodes->getNumChildren());
}
