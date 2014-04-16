#include "../src/TextureBox.hpp"
#include "TextureBoxVisualization.hpp"

namespace vizkit3d
{
    TextureBoxVisualization::TextureBoxVisualization(QObject *parent):
        vizkit3d::VizPluginBase(parent),
            texbox_px(0),
            texbox_py(0),
            texbox_sx(10),
            texbox_sy(10),
            texbox_filename("scene.png")
    {
        this->setPluginEnabled(false);
        setDirty();
    }

    void TextureBoxVisualization::updateMainNode(::osg::Node* node)
    {
        osg::Group *group = node->asGroup();
        if(!group)
            return;

        group->removeChildren(0,group->getNumChildren());

        std::string utf8_filename = texbox_filename.toUtf8().constData();
        ::osg::Node *texbox= TextureBox::create(texbox_px, texbox_py, texbox_sx, texbox_sy, utf8_filename);
        group->addChild(texbox);
    }

    void TextureBoxVisualization::setTexBoxPx(double val)
    {
        if(val <= 0)
            return;
        texbox_px=val;
        setDirty();
    }

    double TextureBoxVisualization::getTexBoxPx()
    {
        return texbox_px;
    }

    void TextureBoxVisualization::setTexBoxPy(double val)
    {
        if(val <= 0)
            return;
        texbox_py=val;
        setDirty();
    }

    double TextureBoxVisualization::getTexBoxPy()
    {
        return texbox_py;
    }

    void TextureBoxVisualization::setTexBoxSx(double val)
    {
        if(val <= 0)
            return;
        texbox_sx=val;
        setDirty();
    }

    double TextureBoxVisualization::getTexBoxSx()
    {
        return texbox_sx;
    }

    void TextureBoxVisualization::setTexBoxSy(double val)
    {
        if(val <= 0)
            return;
        texbox_sy=val;
        setDirty();
    }

    double TextureBoxVisualization::getTexBoxSy()
    {
        return texbox_sy;
    }

    void TextureBoxVisualization::setTexBoxFilename(QString val)
    {
        texbox_filename = val;
        setDirty();
    }

    QString TextureBoxVisualization::getTexBoxFilename()
    {
        return texbox_filename;
    }
}
