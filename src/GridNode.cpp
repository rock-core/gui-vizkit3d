#include "GridNode.hpp"

#include <osg/PositionAttitudeTransform>
#include <osg/Point>

namespace vizkit
{
    ::osg::Node* GridNode::create(int rows,int cols,float dx, float dy,const ::osg::Vec4 &color)
    {
        float size_x = cols*dx;
        float size_y = rows*dy;
        float size = std::min(size_x,size_y);
        float interval = std::min(dx,dy);

        ::osg::PositionAttitudeTransform *transform = new ::osg::PositionAttitudeTransform();
        ::osg::Geode *geode = new ::osg::Geode;
        transform->addChild(geode);

        ::osg::Geometry* geom = new ::osg::Geometry;
        ::osg::Vec3Array* v = new ::osg::Vec3Array;
        geom->setVertexArray(v);

        // draw grid lines
        for(float x = - size_x*0.5f; x <= size_x*0.5f; x += dx)
        {
            v->push_back( ::osg::Vec3(x, -size_y*0.5f, 0.01f));
            v->push_back( ::osg::Vec3(x, size_y*0.5f, 0.01f));
        }
        for(float y = - size_y*0.5f; y <= size_y*0.5f; y += dy)
        {
            v->push_back( ::osg::Vec3(-size_x*0.5f, y, 0.01f));
            v->push_back( ::osg::Vec3(size_x*0.5f, y, 0.01f));
        }

        // draw concentric circles
        for(float r=0; r<size*0.5f; r+=interval)
        {
            float xp = (2.0*M_PI)/(r*100);
            for(float x=0; x<2.0*M_PI; x+=2*xp)
            {
                v->push_back( ::osg::Vec3(cos(x)*r, sin(x)*r, 0.01f) );
                v->push_back( ::osg::Vec3(cos(x+xp)*r, sin(x+xp)*r, 0.01f) );
            }
        }

        // set colors
        ::osg::Vec4Array* c = new ::osg::Vec4Array;
        geom->setColorArray(c);
        geom->setColorBinding( ::osg::Geometry::BIND_OVERALL );
        c->push_back(color);

        // Draw a four-vertex quad from the stored data.
        geom->addPrimitiveSet(new ::osg::DrawArrays(::osg::PrimitiveSet::LINES,0,v->size()));

        ::osg::StateSet* stategeode = geode->getOrCreateStateSet();
        stategeode->setMode( GL_LIGHTING, ::osg::StateAttribute::OFF );
        geode->addDrawable(geom);
        return (::osg::Node*)transform;
    }
}
