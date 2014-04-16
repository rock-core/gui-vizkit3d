#ifndef __VIZKIT_TEXTUREBOX_HPP__
#define __VIZKIT_TEXTUREBOX_HPP__

#include <osg/Geode>
#include <osg/Geometry>

namespace vizkit3d
{
    class TextureBox
    {
        public:
            /**
              Creates a new sub scene graph for a textured box.  

              @param px the origin of the box (x)
              @param py the origin of the box (y) 
              @param sx The size of the box in x direction
              @param sy The size of the box in y direction
              @returns The root node of the created sub graph
              */
            static ::osg::Node* create(float px, float py, float sx, float sy,  std::string filename="scene.png");
    };
}

#endif
