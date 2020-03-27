#include "NodeLink.hpp"


#include <osg/Point>
#include <osgViz/Object.h>
#include <assert.h>
#include <iostream>
#include <stdexcept>

namespace vizkit
{
    NodeLink::NodeCallback::NodeCallback(::osg::Node *node1,::osg::Node *node2):
        ::osg::NodeCallback(),node1(node1),node2(node2)
    {}

    void NodeLink::NodeCallback::operator()(::osg::Node* node, ::osg::NodeVisitor* nv)
    {
        osg::ref_ptr<osg::Node> f1 = node1.get();
        osg::ref_ptr<osg::Node> f2 = node2.get();
        if(!f1.get() || !f2.get())
            return;

        //transform local view2 to local view1
        ::osg::Matrix world2local = ::osg::computeWorldToLocal(nv->getNodePath());

        ::osg::Vec3 pos1(0,0,0);
        ::osg::MatrixList list1 = f1->getWorldMatrices();
        assert(!list1.empty());
        pos1 = pos1*list1.front();
        pos1 = pos1*world2local;

        ::osg::Vec3 pos2(0,0,0);
        ::osg::MatrixList list2 = f2->getWorldMatrices();
        assert(!list2.empty());
        pos2 = pos2*list2.front();
        pos2 = pos2*world2local;

        ::osg::Geode *geode = node->asGeode();
        assert(geode);
        assert(1 == geode->getNumDrawables());
        ::osg::Geometry* geometry = geode->getDrawable(0)->asGeometry();
        assert(geometry);
        assert(geometry->getVertexArray());
        assert(geometry->getVertexArray()->getType() == ::osg::Array::Vec3ArrayType);
        ::osg::Vec3Array *osg_vertex= dynamic_cast< ::osg::Vec3Array*>(geometry->getVertexArray());
        assert(osg_vertex);
        (*osg_vertex)[0] = pos1;
        (*osg_vertex)[1] = pos2;
        //re-set vertex array to apply changes
        geometry->setVertexArray(osg_vertex);
        geometry->dirtyBound();
        traverse(node, nv);
    }

    ::osg::Node* NodeLink::create(::osg::Node *node1, ::osg::Node *node2, const ::osg::Vec4 &color)
    {
        assert(node1 && node2);
        NodeLink* object = new NodeLink();
        ::osg::Geode* geode = new ::osg::Geode();
        geode->setName(node2->getName());
        ::osg::Geometry* geometry = new ::osg::Geometry();

        //setup normals
        ::osg::Vec3Array *normals = new ::osg::Vec3Array;
        normals->push_back(::osg::Vec3(-1.0f,0.0f,0.0f));
        geometry->setNormalArray(normals);
        geometry->setNormalBinding(::osg::Geometry::BIND_OVERALL);

        //set size
        ::osg::Point *point = new ::osg::Point();
        point->setSize(5.0);
        point->setDistanceAttenuation(::osg::Vec3(1.0, 1.0, 1.0 ));
        point->setMinSize( 3.0 );
        point->setMaxSize( 5.0 );
        geometry->getOrCreateStateSet()->setAttribute(point,::osg::StateAttribute::ON );

      //turn on transparency
        geode->getOrCreateStateSet()->setRenderingHint(::osg::StateSet::TRANSPARENT_BIN);
        geode->addDrawable(geometry);

        ::osg::Vec4Array *colors = new ::osg::Vec4Array();
        colors->push_back(color);
        geometry->setColorArray(colors);
        geometry->setColorBinding(::osg::Geometry::BIND_OVERALL);

        ::osg::Vec3Array *vertices = new ::osg::Vec3Array();
        vertices->resize(2);
        (*vertices)[0] = osg::Vec3(0,0,0);
        (*vertices)[1] = osg::Vec3(0,0,0);
        geometry->setVertexArray(vertices);
        geometry->addPrimitiveSet(new ::osg::DrawArrays(::osg::PrimitiveSet::LINES,0,vertices->size()));

        NodeCallback *callback = new NodeCallback(node1,node2);
        geode->setUpdateCallback(callback);
        object->addChild(geode);
        return object;
    }
}
