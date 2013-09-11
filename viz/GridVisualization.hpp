#ifndef __VIZKIT_GRIDVISUALIZATON_HPP
#define __VIZKIT_GRIDVISUALIZATON_HPP

#include "vizkit/Vizkit3DPlugin.hpp"
#include <QVector3D>
#include <QVector2D>
#include <QGenericMatrix>

namespace vizkit
{
    class GridVisualization : public VizPluginBase
    {
        Q_OBJECT
        Q_PROPERTY( int rows READ getGridRows WRITE setGridRows)
        Q_PROPERTY( int cols READ getGridCols WRITE setGridCols)
        Q_PROPERTY( double dx READ getGridDx WRITE setGridDx)
        Q_PROPERTY( double dy READ getGridDy WRITE setGridDy)
        Q_PROPERTY( QColor color READ getGridColor WRITE setGridColor)

        // invalidate parent properties
        Q_PROPERTY(QVariant KeepOldData READ QVariant)
        Q_PROPERTY(QVariant MaxOldData READ QVariant)

        public:
            GridVisualization(QObject *parent = NULL);

        public slots:
            void setGridRows(int val);
            void setGridCols(int val);
            void setGridDx(double val);
            void setGridDy(double val);
            void setGridColor(QColor color);
            int getGridRows();
            int getGridCols();
            double getGridDx();
            double getGridDy();
            QColor getGridColor();

        protected:
            virtual void updateMainNode(osg::Node* node);

        private:
            QColor grid_color;
            int grid_rows;
            int grid_cols;
            double grid_dx;
            double grid_dy;
    };
}

#endif
