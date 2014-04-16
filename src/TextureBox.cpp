#include "TextureBox.hpp"

#include <osg/PositionAttitudeTransform>
#include <osg/Point>
#include <osgText/Text>
#include <osgDB/ReadFile>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace vizkit3d
{
    ::osg::Node* TextureBox::create(float px, float py, float sx, float sy, std::string filename)
    {
        ::osg::PositionAttitudeTransform *transform = new ::osg::PositionAttitudeTransform();
        ::osg::Geode *geode = new ::osg::Geode;
        transform->addChild(geode);

        ::osg::Geometry* geom = new ::osg::Geometry;
        ::osg::Vec3Array* v = new ::osg::Vec3Array;
        geom->setVertexArray(v);

        // create the box
        v->push_back( ::osg::Vec3(px, py, 0.01f));
        v->push_back( ::osg::Vec3(px + sx, py, 0.01f));
        v->push_back( ::osg::Vec3(px, py + sy, 0.01f));
        v->push_back( ::osg::Vec3(px + sx, py + sy, 0.01f));

        // Draw a four-vertex quad from the stored data.
        geom->addPrimitiveSet(new ::osg::DrawArrays(::osg::PrimitiveSet::TRIANGLE_STRIP,0,v->size()));

        osg::Vec2Array* texcoords = new osg::Vec2Array(4);
        (*texcoords)[0].set(0.0f, 0.0f); 
        (*texcoords)[1].set(1.0f, 0.0f); 
        (*texcoords)[2].set(0.0f, 1.0f); 
        (*texcoords)[3].set(1.0f, 1.0f); 
        geom->setTexCoordArray(0, texcoords);

        osg::Texture2D* texture = new osg::Texture2D;
        texture->setDataVariance(osg::Object::DYNAMIC); 
        // load an image by reading a file: 
        osg::Image* image = osgDB::readImageFile(filename);
        if (!image)
        {
           std::cout << "TextureBox: Could not find texture file!." << std::endl;
        }
        // Assign the texture to the image we read from file: 
        texture->setImage(image);

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 0 red
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 0 red
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 0 red
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 0 red

        geom->setColorArray(colors);
        geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

        ::osg::StateSet* stategeode = geode->getOrCreateStateSet();
        stategeode->setMode( GL_LIGHTING, ::osg::StateAttribute::OFF );
        stategeode->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
        geode->addDrawable(geom);
        return (::osg::Node*)transform;
    }
}
