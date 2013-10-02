#include "TransformerGraph.hpp"
#include "AxesNode.hpp"
#include "OsgVisitors.hpp"
#include "NodeLink.hpp"

#include <osg/PositionAttitudeTransform>
#include <osg/NodeVisitor>
#include <osg/Switch>
#include <osgText/Text>
#include <stdexcept>
#include <assert.h>
#include <iostream>

using namespace vizkit3d;

//local helper methods://
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

osg::PositionAttitudeTransform *getTransform(osg::Node *node,bool raise=true)
{
    assert(node);
    osg::Transform *trans = node->asTransform();
    if(!trans)
    {
        if(raise)
            throw std::runtime_error("given node is not a osg::Transform");
        return NULL;
    }

    osg::PositionAttitudeTransform *pos = trans->asPositionAttitudeTransform();
    if(!pos && raise)
        throw std::runtime_error("given node is not a osg::PositionAttitudeTransform");
    return pos;
}

osg::PositionAttitudeTransform *createFrame(const std::string &name)
{
    osg::PositionAttitudeTransform* node = new osg::PositionAttitudeTransform();
    node->setName(name.c_str());

    // always add a group as first child which will used to hold custom nodes
    node->addChild(new osg::Group());

    // add a switch as second child which is used to display frame annotations
    osg::Switch *switch_node = new osg::Switch();
    osg::Node *axes = AxesNode::create(0.2);
    switch_node->addChild(axes,true);
    
    osg::Geode *text_geode = new osg::Geode;
    osgText::Text *text= new osgText::Text;
    text->setText(name);
    text->setCharacterSize(0.1);
    text->setPosition(osg::Vec3d(0.05,0.05,0));
    text_geode->addDrawable(text);
    switch_node->addChild(text_geode,true);

    osg::Node *link = new osg::Node;
    link->setName("link");
    switch_node->addChild(link,true);

    node->addChild(switch_node);
    return node;
}

osg::PositionAttitudeTransform *getParentTransform(osg::Node *transformer)
{
    assert(transformer);
    osg::PositionAttitudeTransform *trans= getTransform(transformer);
    osg::Node *parent_node = trans->getParent(0);
    if(!parent_node)
        return NULL;
    return getTransform(parent_node,false);
}

osg::Group *getFrameGroup(osg::Node *transformer)
{
    assert(transformer);
    osg::PositionAttitudeTransform *trans= getTransform(transformer);
    osg::Node *node = trans->getChild(0);
    assert(node);
    osg::Group *group= node->asGroup();
    assert(group);
    return group;
}

osg::Switch *getFrameSwitch(osg::Node *transformer)
{
    assert(transformer);
    osg::PositionAttitudeTransform *trans= getTransform(transformer);
    osg::Node *node = trans->getChild(1);
    assert(node);
    osg::Switch *switch_node= node->asSwitch();
    assert(switch_node);
    return switch_node;
}

class FindFrame: public ::osg::NodeVisitor
{
    public:
        static ::osg::PositionAttitudeTransform *find(::osg::Node &node,const std::string &name)
        {
            FindFrame finder(name);
            node.accept(finder);
            return finder.getNode();
        }

        static ::osg::PositionAttitudeTransform *find(::osg::Node &node,osg::Node *payload_node)
        {
            FindFrame finder(payload_node);
            node.accept(finder);
            return finder.getNode();
        }

        FindFrame() :
            ::osg::NodeVisitor(::osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN),payload(NULL) {}

        FindFrame(const std::string &name) :
            osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN), name(name),payload(NULL) { }

        FindFrame(osg::Node *payload) :
            osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN), payload(payload) { }

        ::osg::PositionAttitudeTransform *getNode() { return node.get(); }

        void apply(::osg::Node &node)
        {
            // Just stop if the node is not a PositionAttitudeTransform to prevent
            // traversing the hole graph.
            // The assumption here is that all custom nodes are always added
            // to a group child node of the transformation nodes.
            osg::PositionAttitudeTransform *trans = getTransform(&node,false);
            if(!trans)
                return;

            if(payload)
            {
                if(getFrameGroup(trans)->containsNode(payload))
                    this->node = trans;
                else
                    traverse(node);
            }
            else if(!name.empty())
            {
                if(trans->getName() == name)
                    this->node = trans;
                else
                    traverse(node);
            }
        }
    private:
        ::osg::ref_ptr< ::osg::PositionAttitudeTransform> node;
        std::string name;
        ::osg::ref_ptr< ::osg::Node> payload;
};

class FindFrameNames: public ::osg::NodeVisitor
{
    public:
        static std::vector<std::string> find(::osg::Node &node)
        {
            FindFrameNames finder;
            node.accept(finder);
            return finder.getNames();
        }

        FindFrameNames() :
            ::osg::NodeVisitor(::osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN){}

        std::vector<std::string> getNames() {return names;}

        void apply(::osg::Node &node)
        {
            // Just stop if the node is not a PositionAttitudeTransform to prevent
            // traversing the hole graph.
            // The assumption here is that all custom nodes are always added
            // to a group child node of the transformation nodes.
            if(!getTransform(&node,false))
                return;
            names.push_back(std::string(node.getName()));
            traverse(node);
        }
    private:
        std::vector<std::string> names;
};

class AnnotationSetter: public ::osg::NodeVisitor
{
    public:
        static void set(::osg::Node &node,bool active)
        {
            AnnotationSetter setter(active);
            node.accept(setter);
        }

        AnnotationSetter(bool active) :
            ::osg::NodeVisitor(::osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN),active(active){}

        void apply(::osg::Node &node)
        {
            // Just stop if the node is not a PositionAttitudeTransform to prevent
            // traversing the hole graph.
            // The assumption here is that all custom nodes are always added
            // to a group child node of the transformation nodes.
            osg::PositionAttitudeTransform *trans = getTransform(&node,false);
            if(!trans)
                return;
            osg::Switch *switch_node = getFrameSwitch(trans);
            if(active)
                switch_node->setAllChildrenOn();
            else
                switch_node->setAllChildrenOff();
            traverse(node);
        }
    private:
        bool active;
};

class NodeRemover: public ::osg::NodeVisitor
{
    public:
        static void remove(::osg::Node &node,osg::Node *remove)
        {
            NodeRemover remover(remove);
            node.accept(remover);
        }

        NodeRemover(osg::Node *remove) :
            ::osg::NodeVisitor(::osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN),node(remove){}

        void apply(::osg::Node &node)
        {
            // Just stop if the node is not a PositionAttitudeTransform to prevent
            // traversing the hole graph.
            // The assumption here is that all custom nodes are always added
            // to a group child node of the transformation nodes.
            osg::PositionAttitudeTransform *trans = getTransform(&node,false);
            if(!trans)
                return;
            osg::Group *group = getFrameGroup(trans);
            group->removeChild(this->node);
        }
    private:
        ::osg::ref_ptr< ::osg::Node> node;
};


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

osg::Node *TransformerGraph::create(const std::string &name)
{
    return createFrame(name);
}

osg::Node* TransformerGraph::addFrame(osg::Node &transformer,const std::string &name)
{
    if(hasFrame(transformer,name))
        return getFrame(transformer,name);

    osg::PositionAttitudeTransform *trans = getTransform(&transformer);
    osg::PositionAttitudeTransform *node = createFrame(name);
    trans->addChild(node);
    return node;
}

osg::Node *TransformerGraph::getFrame(osg::Node &transformer,const std::string &name)
{
    return FindFrame::find(transformer,name);
}

osg::Node *TransformerGraph::getFrame(osg::Node &transformer,osg::Node *node)
{
    return FindFrame::find(transformer,node);
}


osg::Group *TransformerGraph::getFrameGroup(osg::Node &transformer,const std::string &frame)
{
    std::string str = frame;
    if(str.empty())
        return ::getFrameGroup(&transformer);
    osg::Node *node = FindFrame::find(transformer,frame);
    if(!node)
        return NULL;
    return ::getFrameGroup(node);
}

std::string TransformerGraph::getFrameName(osg::Node &transformer,osg::Node *node)
{
    const osg::Node *nod = getFrame(transformer,node);
    if(nod)
        return std::string(nod->getName());
    else
        return "";
}

bool TransformerGraph::setTransformation(osg::Node &transformer,const std::string &source_frame,const std::string target_frame,
        const osg::Quat &quat, const osg::Vec3d &trans)
{
    osg::PositionAttitudeTransform *source = FindFrame::find(transformer,source_frame);
    osg::PositionAttitudeTransform *target = FindFrame::find(transformer,target_frame);

    if(!source)
        source = getTransform(addFrame(transformer,source_frame));
    if(!target)
        target = getTransform(addFrame(transformer,target_frame));

    // if it is not the case attach target to the source node
    if(target->getParent(0) != source)
    {
	osg::ref_ptr<osg::Node> node = target; //insures that node is not deleted
        removeFrame(transformer,target_frame);
        source->addChild(node);
    }

    target->setAttitude(quat);
    target->setPosition(trans);

    osg::Switch *switch_node = getFrameSwitch(target);
    osg::Node *old_node = FindNode::find(*switch_node,"link");
    assert(old_node);
    osg::Node *link = vizkit::NodeLink::create(source,target,osg::Vec4(255,0,0,255));
    link->setName("link");
    switch_node->replaceChild(old_node,link);
    return true;
}

bool TransformerGraph::removeFrame(osg::Node &transformer,const std::string &name)
{
    osg::PositionAttitudeTransform *trans= FindFrame::find(transformer,name);
    if(!trans)
        return false;

    osg::PositionAttitudeTransform *parent_trans =  getParentTransform(trans);
    if(!parent_trans)
        return false;

    osg::Group *group = ::getFrameGroup(trans);
    osg::Group *parent_group = ::getFrameGroup(parent_trans);

    // attach all custom nodes to the parent node
    for(int i=0;i <(int)group->getNumChildren();++i)
        parent_group->addChild(group->getChild(i));

    group->removeChildren(0,group->getNumChildren());
    parent_trans->removeChild(trans);
    return true;
}

bool TransformerGraph::hasFrame(osg::Node &transformer,const std::string &name)
{
    return NULL != FindFrame::find(transformer,name);
}

std::vector<std::string> TransformerGraph::getFrameNames(osg::Node &transformer)
{
    return FindFrameNames::find(transformer);
}

void TransformerGraph::showFrameAnnotation(osg::Node &transformer,bool value)
{
    AnnotationSetter::set(transformer,value);
}

bool TransformerGraph::areFrameAnnotationVisible(osg::Node &transformer)
{
    osg::PositionAttitudeTransform *trans= getTransform(&transformer);
    osg::Switch *switch_node = getFrameSwitch(trans);
    return switch_node->getValue(0);
}

void TransformerGraph::attachNode(osg::Node &transformer,const std::string &frame,osg::Node &node)
{
    detachNode(transformer,node);
    osg::PositionAttitudeTransform *trans= FindFrame::find(transformer,frame);
    if(!trans)
        return;
    osg::Group *group = ::getFrameGroup(trans);
    group->addChild(&node);
}

void TransformerGraph::detachNode(osg::Node &transformer,osg::Node &node)
{
    NodeRemover::remove(transformer,&node);
}

