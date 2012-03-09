#include <osg/Group>
#include <typeinfo>
#include <cxxabi.h>
#include "Vizkit3DPlugin.hpp"
#include "Vizkit3DHelper.hpp"

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
    : QObject(parent), oldNodes(NULL), dirty( false ),  plugin_enabled(true),
    keep_old_data(false),max_number_of_old_data(100)
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
    return abi::__cxa_demangle(typeid(*this).name(), 0, 0, 0);
}

osg::ref_ptr<osg::Node> VizPluginBase::createMainNode()
{
    return new osg::Group();
}

std::vector< QDockWidget* > VizPluginBase::getDockWidgets()
{
    if (dockWidgets.empty()) createDockWidgets();
    return dockWidgets;
}

void VizPluginBase::createDockWidgets()
{

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
    }

    if( isDirty() )
    {
	updateMainNode(mainNode);
        if(keep_old_data)
        {
            oldNodes->addChild(cloneCurrentViz());
            if(oldNodes->getNumChildren() > max_number_of_old_data)
                oldNodes->removeChild(0,oldNodes->getNumChildren() -max_number_of_old_data);
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
}

bool VizPluginBase::isKeepOldDataEnabled()
{
    return keep_old_data;
}

void VizPluginBase::deleteOldData()
{
    oldNodes->removeChild(0,oldNodes->getNumChildren());
}
