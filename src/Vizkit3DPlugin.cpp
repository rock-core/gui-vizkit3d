#include <osg/Group>
#include "VizPlugin.hpp"

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

VizPluginBase::VizPluginBase()
    : dirty( false )
{
    vizNode = new osg::Group();
    nodeCallback = new CallbackAdapter( this );
    vizNode->setUpdateCallback( nodeCallback );
}

osg::ref_ptr<osg::Group> VizPluginBase::getVizNode() const 
{
    return vizNode;
}

const std::string VizPluginBase::getPluginName() const 
{
    return "VizPlugin";
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

    if (!mainNode)
    {
        mainNode = createMainNode();
        vizNode->addChild(mainNode);
    }

    if( isDirty() )
    {
	updateMainNode(mainNode);
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
