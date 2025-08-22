#ifndef __VIZKIT_TEXTUREBOXVISUALIZATON_HPP
#define __VIZKIT_TEXTUREBOXVISUALIZATON_HPP

#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <QVector3D>
#include <QVector2D>
#include <QGenericMatrix>

namespace vizkit3d
{
    class TextureBoxVisualization : public VizPluginBase
    {
        Q_OBJECT
        Q_PROPERTY( double px READ getTexBoxPx WRITE setTexBoxPx)
        Q_PROPERTY( double py READ getTexBoxPy WRITE setTexBoxPy)
        Q_PROPERTY( double pz READ getTexBoxPz WRITE setTexBoxPz)
        Q_PROPERTY( double sx READ getTexBoxSx WRITE setTexBoxSx)
        Q_PROPERTY( double sy READ getTexBoxSy WRITE setTexBoxSy)
        Q_PROPERTY( QString filename READ getTexBoxFilename WRITE setTexBoxFilename)

        // invalidate parent properties by setting them to an invalid QVariant
        Q_PROPERTY(QVariant KeepOldData READ _invalidate)
        Q_PROPERTY(QVariant MaxOldData READ _invalidate)

        public:
            TextureBoxVisualization(QObject *parent = NULL);

        public slots:
            void setTexBoxPx(double val);
            double getTexBoxPx();
            void setTexBoxPy(double val);
            double getTexBoxPy();
            void setTexBoxPz(double val);
            double getTexBoxPz();

            void setTexBoxSx(double val);
            double getTexBoxSx();
            void setTexBoxSy(double val);
            double getTexBoxSy();

            void setTexBoxFilename(QString val);
            QString getTexBoxFilename();
        protected:
            virtual void updateMainNode(osg::Node* node);

        private:
            double texbox_px;
            double texbox_py;
            double texbox_pz;
            double texbox_sx;
            double texbox_sy;
            QString texbox_filename;
    };
}

#endif
