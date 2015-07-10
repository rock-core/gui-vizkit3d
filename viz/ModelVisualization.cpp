#include "ModelVisualization.hpp"
#include <vizkit3d/OsgVisitors.hpp>
#include <osg/Geode>
#include <osg/Switch>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>

#include <osg/MatrixTransform>


using namespace osg;
namespace vizkit3d
{
    ModelVisualization::ModelVisualization(QObject* parent):
        vizkit3d::VizPluginBase(parent)
    {
        setDirty();
    }

    ModelVisualization::~ModelVisualization()
    {
    }

    ref_ptr<Node> ModelVisualization::createMainNode()
    {
        Group* group = new Switch();
        ref_ptr<Geode> geode = new Geode();
        ref_ptr<Sphere> sp = new Sphere(Vec3f(0,0,0), 0.3);
        ref_ptr<ShapeDrawable> spd = new ShapeDrawable(sp);
        spd->setColor(Vec4f(0,1,0, 1.0));
        geode->addDrawable(spd);
        group->addChild(geode);
        return group;
    }

    void ModelVisualization::updateMainNode(::osg::Node* node)
    {
        osg::Group *group = node->asGroup();
        if(!group || group->getNumChildren() == 0)
            return;
        if(model)
        {
            osg::Switch *sgroup= group->asSwitch();
            if(!sgroup)
                return;
            sgroup->setValue(0,false);
            if(sgroup->getNumChildren() == 2)
            {
                sgroup->setChild(1,model);
                sgroup->setValue(1,true);
            }
            else
                sgroup->addChild(model,true);
        }
        else
        {
            osg::Geode *geode = group->getChild(0)->asGeode();
            if(!geode || geode->getNumDrawables() == 0)
                return;
            osg::Switch *sgroup= group->asSwitch();
            if(sgroup)
            {
                sgroup->setValue(0,true);
                sgroup->setValue(1,true);
                if(sgroup->getNumChildren() > 1)
                    sgroup->removeChildren(1,1);
            }
        }

        // update all joints
        JointMap::const_iterator iter = joint_map.begin();
        for(;iter != joint_map.end();++iter)
        {
            osg::Matrix mat = iter->second.orig_mat;
            mat.preMult(osg::Matrix::rotate(iter->second.quat));
            iter->second.trans->setMatrix(mat);
        }
    }

    void ModelVisualization::resetModel()
    {
        boost::mutex::scoped_lock lock(updateMutex);
        JointMap::iterator iter = joint_map.begin();
        for(;iter != joint_map.end();++iter)
            iter->second.quat = osg::Quat(0,0,0,1);
        setDirty();
    }

    QString ModelVisualization::getModelPath() const
    {
        return model_path;
    }

    void ModelVisualization::setJointRotation(QString const& name,QQuaternion quat)
    {
        std::string str = name.toStdString();
        JointMap::iterator iter = joint_map.find(str);
        if(iter == joint_map.end())
        {
            osg::Node *joint = FindNode::find(*model,name.toStdString());
            if(joint)
            {
                osg::Transform *trans = joint->asTransform();
                if(trans)
                {
                    osg::MatrixTransform *trans2 = trans->asMatrixTransform();
                    if(trans2)
                    {
                        Joint joint;
                        joint.orig_mat = trans2->getMatrix();
                        joint.trans = trans2;
                        boost::mutex::scoped_lock lock(updateMutex);
                        joint_map[str] = joint;
                        iter = joint_map.find(str);
                    }
                }
            }
        }
        if(iter != joint_map.end())
        {
            boost::mutex::scoped_lock lock(updateMutex);
            iter->second.quat = osg::Quat(quat.x(),quat.y(),quat.z(),quat.scalar());
            setDirty();
        }
    }

    void ModelVisualization::setModelPath(QString const& path)
    {
        osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(path.toStdString());
        {
            boost::mutex::scoped_lock lock(updateMutex);
            model = node;

            if(model)
                model_path = path;
            else
                model_path = "";
        }
        if(vizkit3d_plugin_name.isEmpty())
            setPluginName(QFileInfo(model_path).baseName());
        setDirty();
        emit propertyChanged("model_path");
    }
}
