#include <osg/Group>
#include <typeinfo>
#include <cxxabi.h>
#include <memory>
#include <osgViz/Object.h>
#include <osgViz/interfaces/Clickable.h>

#include "Vizkit3DPlugin.hpp"
#include "Vizkit3DWidget.hpp"

namespace vizkit3d{
    class ClickHandler : public osgviz::Clickable
    {
        VizPluginBase& plugin;
    public:
        ClickHandler(VizPluginBase& plugin) : plugin(plugin), enabled(true){};

        virtual bool clicked(const int &buttonMask, const osg::Vec2d &cursor,
                           const osg::Vec3d &world, const osg::Vec3d &local,
                           Clickable* object, const int modifierMask,
                           osgviz::WindowInterface* window = NULL)
        {
            if (enabled){
                plugin.click((float)cursor.x(), (float)cursor.y(), buttonMask, modifierMask);
                plugin.pick((float)world.x(), (float)world.y(), (float)world.z(), buttonMask, modifierMask);
                return true;
            }
            return false;
        }

        bool isEnabled(){
            return enabled;
        }
    
        void enable(bool val = true){
            enabled = val;
        }

    private:
        bool enabled;

    };
}

using namespace vizkit3d;
/** this adapter is used to forward the update call to the plugin
 */
class VizPluginBase::CallbackAdapter : public osg::NodeCallback
{
    public:
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
    rootNode = new osgviz::Object();
    click_handler = std::shared_ptr<ClickHandler>(new ClickHandler(*this));
    rootNode->addClickableCallback(click_handler);
    nodeCallback = new CallbackAdapter(this);
    rootNode->setUpdateCallback(nodeCallback);
    vizNode = new osg::PositionAttitudeTransform();
    rootNode->addChild(vizNode);
    oldNodes = new osg::Group();
    rootNode->addChild(oldNodes);
}

VizPluginBase::~VizPluginBase()
{
    osg::Node::ParentList parents = rootNode->getParents();
    for (std::size_t i = 0; i < parents.size(); ++i)
        parents[i]->removeChild(rootNode);
}

osg::ref_ptr<osg::Group> VizPluginBase::getVizNode() const 
{
    return vizNode;
}

void VizPluginBase::setScale(double scale)
{
    vizNode->setScale(osg::Vec3d(scale,scale,scale));
}

double VizPluginBase::getScale() const
{
    osg::Vec3d scale = vizNode->getScale();
    return (scale.x()+scale.y()+scale.y())/3;
}

osg::ref_ptr<osg::Group> VizPluginBase::getRootNode() const 
{
    return rootNode;
}

void VizPluginBase::click(float x,float y, int buttonMask, int modifierMask)
{
    QWidget *osg_widget = dynamic_cast<QWidget*>(parent()); // widget displaying the osg scene.
    
    if(!osg_widget)
        return;
    
    QWidget *container = osg_widget; // will point to Vizkit3DWidget if there is one. contains property browser and osg widget.

    // Find the container widget in the plugin's parents.
    while(container)
    {
        if(container->objectName().toStdString().compare("vizkit3d::Vizkit3DWidget") == 0)
        {
            // Container found.
            //std::cout << "grandparent: " << container->objectName().toStdString() << std::endl;
            QPoint container_coords = osg_widget->mapTo(container, QPoint(x,y));
            //std::cout << "grandparent coords: (" << container_coords.x() << "," << container_coords.y() << ")" << std::endl;
            emit clicked(container_coords.x(), container_coords.y());
            emit clicked(container_coords.x(), container_coords.y(), buttonMask, modifierMask);
            break;
        }
        else
        {
            // Try next parent.
            container = container->parentWidget();
        }
    }
}

void VizPluginBase::pick(float x, float y, float z, int buttonMask, int modifierMask)
{
    emit picked(x, y, z);
    emit picked(x, y, z, buttonMask, modifierMask);
  
    for(std::function<void(float, float, float)> f : pickCallbacks)
    {
	f(x, y, z);
    }    
}

void VizPluginBase::addPickHandler(std::function<void(float, float, float)> f)
{
  pickCallbacks.push_back(f);
}


void VizPluginBase::setPose(const QVector3D &position, const QQuaternion &orientation)
{
    boost::mutex::scoped_lock lock(updateMutex);
    this->position = position;
    this->orientation = orientation;
    setDirty();
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
    if (!mainNode)
    {
        mainNode = createMainNode();
        vizNode->addChild(mainNode);
        isAttached = true;
    }

    if(isDirty())
    {
        boost::mutex::scoped_lock lockit(updateMutex);
        vizNode->setPosition(osg::Vec3d(position.x(), position.y(), position.z()));
        vizNode->setAttitude(osg::Quat(orientation.x(), orientation.y(), orientation.z(), orientation.scalar()));

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

QObject* vizkit3d::VizPluginBase::getRubyAdapterCollection()
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

Vizkit3DWidget* VizPluginBase::getWidget() const
{
    return dynamic_cast<Vizkit3DWidget*>(this->parent());
}

QStringList VizPluginBase::getVisualizationFrames() const
{
    if (!getWidget())
        return QStringList();

    QStringList list = getWidget()->getVisualizationFrames();
    if(!current_frame.isEmpty() && !list.isEmpty())
    {
        list.removeOne(current_frame);
        list.prepend(current_frame);
    }
    return list;
}

QString VizPluginBase::getVisualizationFrame() const
{
    return current_frame;
}

// same as setVisualizationFrame but is not emitting a signal because
// this is called from the property browser
void VizPluginBase::setVisualizationFrameFromList(const QStringList &frames)
{
    if (frames.empty())
        return;
    if (!getWidget())
        return; 
    getWidget()->setPluginDataFrameIntern(frames.front(),this);
    current_frame = frames.front();
}

QVariant VizPluginBase::_invalidate()const
{
    return QVariant();
}

void VizPluginBase::setVisualizationFrame(const QString &frame)
{
    if (!getWidget())
        return;
    getWidget()->setPluginDataFrameIntern(frame,this);
    current_frame = frame;
    emit propertyChanged("frame");
}

bool VizPluginBase::getEvaluatesClicks() const{
    return click_handler->isEnabled();
}

void VizPluginBase::setEvaluatesClicks (const bool &value){
    click_handler->enable(value);
}

