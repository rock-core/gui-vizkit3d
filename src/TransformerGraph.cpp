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
using namespace osg;

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

osg::PositionAttitudeTransform *createFrame(const std::string &name,bool root=false,float textSize=0.1)
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
    text->setCharacterSize(textSize);
    if(root)
        text->setPosition(osg::Vec3d(textSize/2,-textSize*1.5,0));
    else
        text->setPosition(osg::Vec3d(textSize/2,textSize/2,0));
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
    if (trans->getNumParents() == 0)
        return NULL;
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

osgText::Text *getFrameText(osg::Node *transform)
{
    osg::Switch* switch_node = getFrameSwitch(transform);
    osg::Geode *text_geode = switch_node->getChild(1)->asGeode();
    assert(text_geode);
    osgText::Text* text = dynamic_cast<osgText::Text*>(text_geode->getDrawable(0));
    assert(text);
    return text;
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

class TextSizeSetter: public ::osg::NodeVisitor
{
    public:
        static void set(::osg::Node &node,float size)
        {
            TextSizeSetter setter(size);
            node.accept(setter);
        }

        TextSizeSetter(float size)
            : ::osg::NodeVisitor(::osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
            , size(size) {}

        void apply(::osg::Node &node)
        {
            // Just stop if the node is not a PositionAttitudeTransform to prevent
            // traversing the hole graph.
            // The assumption here is that all custom nodes are always added
            // to a group child node of the transformation nodes.
            osg::PositionAttitudeTransform *trans = getTransform(&node,false);
            if(!trans)
                return;
            
            osgText::Text* text = getFrameText(trans);
            text->setCharacterSize(size);
            text->setPosition(osg::Vec3d(size / 2, size / 2, size / 2));

            traverse(node);
        }
    private:
        float size;
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

class WorldCoordOfNodeVisitor : public osg::NodeVisitor
{
public:
    WorldCoordOfNodeVisitor():
        osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS){}

    virtual void apply(osg::Node &node)
    {
        if ( 0 == node.getNumParents() ) // no parents
            wcMatrix.set( osg::computeLocalToWorld(this->getNodePath()) );
        traverse(node);
    }
    osg::Matrixd getMatrix()
    {
        return wcMatrix;
    }
private:
   osg::Matrix wcMatrix;
};

class DescriptionVisitor: public ::osg::NodeVisitor
{
    TransformerGraph::GraphDescription& description;

    public:
        DescriptionVisitor(TransformerGraph::GraphDescription& description)
            : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
            , description(description) {}

        void apply(::osg::Node &node)
        {
            osg::PositionAttitudeTransform *trans = getTransform(&node,false);
            if(!trans)
            {
                return;
            }


            osg::PositionAttitudeTransform *parent = getParentTransform(trans);
            if (parent)
            {
                description.push_back(make_pair(parent->getName(), trans->getName()));
            }
            else
            {
            }

            traverse(node);
        }
};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

osg::Node *TransformerGraph::create(const std::string &name)
{
    return createFrame(name,true,0.1);
}

TransformerGraph::GraphDescription TransformerGraph::getGraphDescription(osg::Node& transformer)
{
    GraphDescription result;
    DescriptionVisitor visitor(result);
    transformer.accept(visitor);
    return result;
}

osg::Node* TransformerGraph::addFrame(osg::Node &transformer,const std::string &name)
{
    if(hasFrame(transformer,name))
        return getFrame(transformer,name);

    osg::PositionAttitudeTransform *trans = getTransform(&transformer);
    osg::PositionAttitudeTransform *node = createFrame(name,false,getTextSize(transformer));
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

std::string TransformerGraph::getWorldName(const osg::Node &transformer)
{
    return std::string(transformer.getName());
}

void TransformerGraph::setWorldName(osg::Node &transformer, const std::string &name)
{
    transformer.setName(name);
    osgText::Text* text = getFrameText(&transformer);
    text->setText(name);
}

void TransformerGraph::setTextSize(osg::Node &transformer, float size)
{
    TextSizeSetter::set(transformer, size);
}

float TransformerGraph::getTextSize(osg::Node &transformer)
{
    osgText::Text *text= getFrameText(&transformer);
    return text->getCharacterHeight();
}

bool TransformerGraph::getTransformation(osg::Node &transformer,const std::string &source_frame,const std::string &target_frame,
                                         osg::Quat &_quat, osg::Vec3d &_trans)
{
    osg::PositionAttitudeTransform *source = FindFrame::find(transformer,source_frame);
    osg::PositionAttitudeTransform *target = FindFrame::find(transformer,target_frame);
    if(!source || !target)
        return false;

    if(source == target)
    {
        _quat = osg::Quat();
        _trans = osg::Vec3d();
    }
    else
    {
        WorldCoordOfNodeVisitor visitor;
        source->accept(visitor);
        osg::Matrix mat1 = visitor.getMatrix();
        target->accept(visitor);
        osg::Matrix mat2 = visitor.getMatrix();
        osg::Matrix mat1_inv;
        mat1.inverse(mat1_inv);

        osg::Vec3d scale;
        osg::Quat so;
        (mat1_inv*mat2).decompose(_trans,_quat,scale,so);
    }
    return true;
}

static void invertOSGTransform(osg::Vec3d& trans, osg::Quat& quat,
        osg::PositionAttitudeTransform*& source, osg::PositionAttitudeTransform*& target,
        std::string& source_frame, std::string& target_frame)
{
    quat = quat.inverse();
    trans = -(quat * trans);
    std::swap(source, target);
    std::swap(source_frame, target_frame);
}

static void makeRoot(osg::Node& _transformer,
        osg::ref_ptr<osg::PositionAttitudeTransform> desiredRoot,
        std::set<osg::Node*> ancestors)
{
    PositionAttitudeTransform* transformer = getTransform(&_transformer);

    osg::Vec3d trans = osg::Vec3d(0, 0, 0);
    osg::Quat  rot   = osg::Quat(0, 0, 0, 1);

    ref_ptr<PositionAttitudeTransform> currentNode(desiredRoot);
    ref_ptr<PositionAttitudeTransform> lastNode(transformer);
    while(currentNode != transformer && !ancestors.count(currentNode))
    {
        osg::Vec3d nextTrans = currentNode->getPosition();
        osg::Quat  nextRot   = currentNode->getAttitude();
        currentNode->setPosition(trans);
        currentNode->setAttitude(rot);
        rot   = nextRot.inverse();
        trans = -(rot * nextTrans);

        ref_ptr<PositionAttitudeTransform> parent(getTransform(currentNode->getParent(0)));
        parent->removeChild(currentNode);
        lastNode->addChild(currentNode);
        lastNode = currentNode;
        currentNode = parent;
    }
}

void TransformerGraph::makeRoot(osg::Node& _transformer, std::string const& frame)
{
    ref_ptr<PositionAttitudeTransform> desiredRoot(FindFrame::find(_transformer, frame));
    if (!desiredRoot)
        return;

    return ::makeRoot(_transformer, desiredRoot, std::set<osg::Node*>());
}

bool TransformerGraph::setTransformation(osg::Node &transformer,const std::string &_source_frame,const std::string &_target_frame,
        const osg::Quat &_quat, const osg::Vec3d &_trans)
{
    std::string source_frame = _source_frame;
    std::string target_frame = _target_frame;
    osg::PositionAttitudeTransform *source = FindFrame::find(transformer,source_frame);
    osg::PositionAttitudeTransform *target = FindFrame::find(transformer,target_frame);
    osg::Quat quat = _quat;
    osg::Vec3d trans = _trans;

    if(!source)
        source = getTransform(addFrame(transformer,source_frame));
    if(!target)
        target = getTransform(addFrame(transformer,target_frame));

    if(source == target)
    {
        std::cerr << "cannot set transformation between " << source_frame << " and " << target_frame << ". They are identically" << std::endl;
        return false;
    }
    
    if (source->getParent(0) == target)
    {
        invertOSGTransform(trans, quat, source, target, source_frame, target_frame);
    }
    else if (target->getParent(0) != source)
    {
        if (target == &transformer || (source->getParent(0) == &transformer && target->getParent(0) != &transformer))
        {
            invertOSGTransform(trans, quat, source, target, source_frame, target_frame);
        }

        if (target->getParent(0) != &transformer)
        {
            std::set<osg::Node*> ancestors;
            osg::ref_ptr<osg::Node> sourceAncestor = source;
            while (sourceAncestor != &transformer)
            {
                ancestors.insert(sourceAncestor);
                if (sourceAncestor->getParent(0) == target)
                {
                    target->removeChild(sourceAncestor);
                    getTransform(&transformer)->addChild(sourceAncestor);
                    ancestors.clear();
                    break;
                }
                else
                    sourceAncestor = sourceAncestor->getParent(0);
            }

            ::makeRoot(transformer, getTransform(target), ancestors);

        }

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

