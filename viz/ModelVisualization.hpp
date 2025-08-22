#ifndef __ROBOT_VISUALIZATION_HPP__
#define __ROBOT_VISUALIZATION_HPP__

#include <vizkit3d/Vizkit3DPlugin.hpp>

namespace vizkit3d
{

    class ModelVisualization : public VizPluginBase
    {
        Q_OBJECT
        Q_PROPERTY(QString model_path READ getModelPath WRITE setModelPath)

        // invalidate parent properties by setting them to an invalid QVariant
        Q_PROPERTY(QVariant KeepOldData READ _invalidate)
        Q_PROPERTY(QVariant MaxOldData READ _invalidate)

    public:
	ModelVisualization(QObject* parent = NULL);
	virtual ~ModelVisualization();

    public slots:
        /** Sets the path of the robot model (osg,ive,stl,... see openscenegraph doc.) and loads it
         */
        void setModelPath(QString const& path);

        /** Returns the path of the loaded robot model
         */
        QString getModelPath() const;

        /** Resets the loaded robot model to its original state
         */
        void resetModel();

        /** Sets the rotation of a robot joint
         *
         * Certain 3D formats (dae,isg,ode) directly support object graphs
         * including joints which can be loaded by openscengraph. To modifiy
         * a joint its string label has to be known.
         *
         * To generate a compatible 3d model blender can be used together with
         * the collada export plugin. To load the collada model from
         * openscengraph, openscenegraph has to be compiled with collada
         * support. Therefore, the model should be converted to the native
         * openscenegraph format (osg,ive) to be loadable from any viztki3d
         * installation using the openscengraph tool osgconv.
         */
        void setJointRotation(QString const& name,QQuaternion);

    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
	virtual void updateMainNode(osg::Node* node);

    private:
        struct Joint
        {
            osg::Matrix orig_mat;
            osg::ref_ptr<osg::MatrixTransform> trans;
            osg::Quat quat;
        };
        typedef std::map<std::string,Joint> JointMap;

        JointMap joint_map;
        osg::ref_ptr<osg::Node> model;
        QString model_path;
};

}
#endif // ROBOT_H
