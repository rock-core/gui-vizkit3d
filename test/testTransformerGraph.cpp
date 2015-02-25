#include <vizkit3d/TransformerGraph.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

typedef osg::ref_ptr<osg::Node> NodePtr;
using namespace vizkit3d;
using namespace std;

// Helper function, defined at the bottom
static void __require_same_graph(char const** expected_graph, size_t expected_graph_size, NodePtr root_node);
#define REQUIRE_SAME_GRAPH(expected_graph, root_node) \
    __require_same_graph(expected_graph, sizeof(expected_graph) / sizeof(*expected_graph) / 2, root_node)

BOOST_AUTO_TEST_SUITE(vizkit3d_TransformerGraph)

static const osg::Quat Identity(0, 0, 0, 1);
static const osg::Vec3d Zero(0, 0, 0);

BOOST_AUTO_TEST_CASE(it_appends_new_edges_to_the_existing_graph)
{
    NodePtr root(TransformerGraph::create("root"));

    TransformerGraph::setTransformation(*root, "frame1", "frame2", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame3", "frame2", Identity, Zero);

    char const* expected_graph[] = {
        "root", "frame1",
        "frame1", "frame2",
        "frame2", "frame3"
    };
    REQUIRE_SAME_GRAPH(expected_graph, root);
}

BOOST_AUTO_TEST_CASE(it_keeps_existing_links_even_if_updates_are_provided_reversed)
{
    NodePtr root(TransformerGraph::create("root"));

    TransformerGraph::setTransformation(*root, "frame1", "frame2", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame2", "frame3", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame3", "frame2", Identity, Zero);

    char const* expected_graph[] = {
        "root", "frame1",
        "frame1", "frame2",
        "frame2", "frame3"
    };
    REQUIRE_SAME_GRAPH(expected_graph, root);
}

BOOST_AUTO_TEST_CASE(it_allows_to_make_a_frame_root)
{
    NodePtr root(TransformerGraph::create("root"));

    TransformerGraph::setTransformation(*root, "frame1", "frame2", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame1", "frame3", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame2", "frame4", Identity, Zero);
    TransformerGraph::makeRoot(*root, "frame2");

    char const* expected_graph[] = {
        "root", "frame2",
        "frame2", "frame1",
        "frame1", "frame3",
        "frame2", "frame4"
    };
    REQUIRE_SAME_GRAPH(expected_graph, root);
}

BOOST_AUTO_TEST_CASE(it_automatically_changes_the_node_shape_when_needed)
{
    NodePtr root(TransformerGraph::create("root"));

    TransformerGraph::setTransformation(*root, "frame1", "frame2", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame3", "frame4", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame2", "frame4", Identity, Zero);

    char const* expected_graph[] = {
        "root", "frame1",
        "frame1", "frame2",
        "frame2", "frame4",
        "frame4", "frame3"
    };
    REQUIRE_SAME_GRAPH(expected_graph, root);
}

BOOST_AUTO_TEST_CASE(it_handles_loops_gracefully_when_reshaping)
{
    NodePtr root(TransformerGraph::create("root"));

    TransformerGraph::setTransformation(*root, "frame1", "frame2", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame2", "frame3", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame3", "frame4", Identity, Zero);
    TransformerGraph::setTransformation(*root, "frame4", "frame2", Identity, Zero);

    char const* expected_graph[] = {
        "root", "frame3",
        "frame3", "frame4",
        "frame4", "frame2",
        "frame2", "frame1"
    };
    REQUIRE_SAME_GRAPH(expected_graph, root);
}

BOOST_AUTO_TEST_SUITE_END();


/** NOTE: Helper functions */
static void __require_same_graph(char const** expected_graph, size_t expected_graph_size, NodePtr root_node)
{
    TransformerGraph::GraphDescription description = TransformerGraph::getGraphDescription(*root_node);
    BOOST_TEST_MESSAGE("Checking equality on graph");
    for (TransformerGraph::GraphDescription::const_iterator it = description.begin();
            it != description.end(); ++it)
    {
        BOOST_TEST_MESSAGE("  " << it->first << " " << it->second);
    }

    BOOST_REQUIRE_EQUAL(description.size(), expected_graph_size);

    for (size_t i = 0; i < expected_graph_size; ++i)
    {
        TransformerGraph::EdgeDescription edge =
            make_pair<string>(expected_graph[i * 2], expected_graph[i * 2 + 1]);

        TransformerGraph::GraphDescription::const_iterator it =
            find(description.begin(), description.end(), edge);

        BOOST_TEST_MESSAGE("expected edge " << edge.first << " -> " << edge.second);
        BOOST_REQUIRE( it != description.end() );
    }
}
