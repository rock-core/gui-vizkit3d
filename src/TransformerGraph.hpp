
#ifndef __VIZKIT3D_TRANSFORMERGRAPH_HPP__
#define __VIZKIT3D_TRANSFORMERGRAPH_HPP__

#include <osg/Node>
#include <osg/Geometry>

#include <vector>
#include <string>

namespace vizkit3d
{
    /**
     * The class TransfromerGraph is a collection of methods for creating and
     * manipulating an osg sub scene graph used to maintain osg nodes living in
     * different relative coordinate frames.
     */
    class TransformerGraph
    {
        public:
        /**
         * Creates a new sub scene graph for maintaining a transformation stack
         *
         * Do not directly add nodes to this node. Use getGroupNode instead!
         *
         * @param name The name of the wold coordinate frame
         * @returns The transformer node of the created sub graph
         */
        static osg::Node *create(const std::string &name);


        /**
         * Returns the name of the wold (root) frame.
         */
        static std::string getWorldName(const osg::Node &transformer);

        /**
         * Adds a new coordinate frame to the graph. Use setTranformation to
         * change its location in the graph.
         *
         * @param transformer The osg node of the transformer graph 
         * @param name The name of the new coordinate frame
         */
        static osg::Node *addFrame(osg::Node &transformer,const std::string &name);

        /**
         * Returns the osg node for the given frame name. Returns NULL if the frame
         * name does not exist.
         *
         * Do not directly add nodes to this node. Use getGroupNode instead!
         *
         * @param transformer The osg node of the transformer graph
         * @param name The name of coordinate frame
         */
        static osg::Node *getFrame(osg::Node &transformer,const std::string &name);

        /**
         * Returns the osg frame node for the given custom node. Returns NULL if the given node
         * is not port of the sub scene graph
         *
         * Do not directly add nodes to this node. Use getGroupNode instead!
         *
         * @param transformer The osg node of the transformer graph
         * @param node The node
         */
        static osg::Node *getFrame(osg::Node &transformer,osg::Node *node);

        /**
         * Returns the osg group node for the given frame. Use this node if 
         * you want to attach custom nodes to the frame
         *
         * if no name is given it will return the group of the top level
         *
         * @param transformer The osg node of the transformer graph
         * @param node The node
         */
        static osg::Group *getFrameGroup(osg::Node &transformer,const std::string &frame="");

        /**
         * Returns the name of the osg frame node for the given custom node. Returns an empty string if the given node
         * is not port of the sub scene graph
         *
         * @param transformer The osg node of the transformer graph
         * @param node The node
         */
        static std::string getFrameName(osg::Node &transformer,osg::Node *node);

        /**
         * Sets the transformation between two subsequently coordinate frames.
         *
         * @param transformer The osg node of the transformer graph
         * @param source_frame The name of the source coordinate frame
         * @param target_frame The name of the target coordinate frame
         * @param quat The rotation between source and target frame
         * @param trans The translation between source and target frame
         */
        static bool setTransformation(osg::Node &transformer,const std::string &source_frame,const std::string &target_frame,
                                      const osg::Quat &quat, const osg::Vec3d &trans);

        /**
         * Gets the transformation between two coordinate frames. Thereby unlike setTransformation the frames do not have to be subsequently.
         *
         * @param transformer The osg node of the transformer graph
         * @param source_frame The name of the source coordinate frame
         * @param target_frame The name of the target coordinate frame
         * @param quat The rotation between source and target frame
         * @param trans The translation between source and target frame
         */
        static bool getTransformation(osg::Node &transformer,const std::string &source_frame,const std::string &target_frame,
                                      osg::Quat &quat, osg::Vec3d &trans);

        /**
         * Removes the osg node for the given frame name. Returns false if the frame
         * name does not exist.
         *
         * @param transformer The osg node of the transformer graph
         * @param name The name of coordinate frame
         */
        static bool removeFrame(osg::Node &transformer,const std::string &name);

        /**
         * Returns true if the given frame name exists.
         *
         * @param transformer The osg node of the transformer graph
         * @param name The name of coordinate frame
         */
        static bool hasFrame(osg::Node &transformer,const std::string &name);

        /**
         * Returns the names of all added coordinate frames.
         *
         * @param transformer The osg node of the transformer graph
         */
        static std::vector<std::string> getFrameNames(osg::Node &transformer);

        /**
         * Activates or deactivates the annotation of the frame nodes
         *
         * @param transformer The osg node of the transformer graph
         * @param value
         */
        static void showFrameAnnotation(osg::Node &transformer,bool value);

        /**
         * Returns true if the frame annotations are activated
         *
         * @param transformer The osg node of the transformer graph
         */
        static bool areFrameAnnotationVisible(osg::Node &transformer);

        /**
         * Attaches the given node to the given coordinate frame.
         * If the node is already attached it will be removed first.
         *
         * @param transformer The osg node of the transformer graph
         * @param frame The name of the coordinate frame.
         * @param node The node which shall be attached.
         */
        static void attachNode(osg::Node &transformer,const std::string &frame,osg::Node &node);

        /**
         * Detaches the given node from the transformer graph
         *
         * @param transformer The osg node of the transformer graph
         * @param node The node which shall be detached.
         */
        static void detachNode(osg::Node &transformer,osg::Node &node);

    };

}

#endif
