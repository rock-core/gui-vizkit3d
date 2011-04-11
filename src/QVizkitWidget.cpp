#include "QVizkitWidget.hpp"
#include <QVBoxLayout>
#include <GridNode.hpp>

using namespace vizkit;

QVizkitWidget::QVizkitWidget( QWidget* parent, Qt::WindowFlags f )
    : CompositeViewerQOSG( parent, f )
{
    createSceneGraph();

    QWidget* viewWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( viewWidget );
    this->setLayout( layout );

    view = new ViewQOSG( viewWidget );
    view->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    view->setData( root );
    addView( view );

    // pickhandler is for selecting objects in the opengl view
    pickHandler = new PickHandler();
    view->addEventHandler( pickHandler );
    
    // add visualization of ground grid 
    GridNode *gn = new GridNode();
    root->addChild(gn);
}

QSize QVizkitWidget::sizeHint() const
{
    return QSize( 800, 600 );
}

osg::ref_ptr<osg::Group> QVizkitWidget::getRootNode() const
{
    return root;
}

void QVizkitWidget::setTrackedNode( VizPluginBase* plugin )
{
    view->setTrackedNode(plugin->getVizNode());
}

void QVizkitWidget::createSceneGraph() 
{
    //create root node that holds all other nodes
    root = new osg::Group;
    
    osg::ref_ptr<osg::StateSet> state = root->getOrCreateStateSet();
    state->setGlobalDefaults();
    state->setMode( GL_LINE_SMOOTH, osg::StateAttribute::ON );
    state->setMode( GL_POINT_SMOOTH, osg::StateAttribute::ON );
    state->setMode( GL_BLEND, osg::StateAttribute::ON );    
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON);
    state->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    state->setMode( GL_LIGHT0, osg::StateAttribute::ON );
    state->setMode( GL_LIGHT1, osg::StateAttribute::ON );
	
    root->setDataVariance(osg::Object::DYNAMIC);

    // Add the Light to a LightSource. Add the LightSource and
    //   MatrixTransform to the scene graph.
    for(size_t i=0;i<2;i++)
    {
	osg::ref_ptr<osg::Light> light = new osg::Light;
	light->setLightNum(i);
	switch(i) {
	    case 0:
		light->setAmbient( osg::Vec4( .1f, .1f, .1f, 1.f ));
		light->setDiffuse( osg::Vec4( .8f, .8f, .8f, 1.f ));
		light->setSpecular( osg::Vec4( .8f, .8f, .8f, 1.f ));
		light->setPosition( osg::Vec4( 1.f, 1.5f, 2.f, 0.f ));
		break;
	    case 1:
		light->setAmbient( osg::Vec4( .1f, .1f, .1f, 1.f ));
		light->setDiffuse( osg::Vec4( .1f, .3f, .1f, 1.f ));
		light->setSpecular( osg::Vec4( .1f, .3f, .1f, 1.f ));
		light->setPosition( osg::Vec4( -1.f, -3.f, 1.f, 0.f ));
	}

	osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
	ls->setLight( light.get() );
	//ls->setStateSetModes(*state, osg::StateAttribute::ON);
	root->addChild( ls.get() );
    }
}

void QVizkitWidget::addDataHandler(VizPluginBase *viz)
{
    root->addChild( viz->getVizNode() );
}

void QVizkitWidget::removeDataHandler(VizPluginBase *viz)
{
    root->removeChild( viz->getVizNode() );
}

void QVizkitWidget::changeCameraView(const osg::Vec3& lookAtPos)
{
    changeCameraView(&lookAtPos, 0, 0);
}

void QVizkitWidget::changeCameraView(const osg::Vec3& lookAtPos, const osg::Vec3& eyePos)
{
    changeCameraView(&lookAtPos, &eyePos, 0);
}

void QVizkitWidget::setCameraLookAt(double x, double y, double z)
{
    osg::Vec3 lookAt(x, y, z);
    changeCameraView(&lookAt, 0, 0);
}
void QVizkitWidget::setCameraEye(double x, double y, double z)
{
    osg::Vec3 eye(x, y, z);
    changeCameraView(0, &eye, 0);
}
void QVizkitWidget::setCameraUp(double x, double y, double z)
{
    osg::Vec3 up(x, y, z);
    changeCameraView(0, 0, &up);
}

void QVizkitWidget::changeCameraView(const osg::Vec3* lookAtPos, const osg::Vec3* eyePos, const osg::Vec3* upVector)
{
    osgGA::KeySwitchMatrixManipulator* switchMatrixManipulator = dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(view->getCameraManipulator());
    if (!switchMatrixManipulator) return;
    //select TerrainManipulator
    switchMatrixManipulator->selectMatrixManipulator(3);
    
    //get last values of eye, center and up
    osg::Vec3d eye, center, up;
    switchMatrixManipulator->getHomePosition(eye, center, up);

    if (lookAtPos)
        center = *lookAtPos;
    if (eyePos)
        eye = *eyePos;
    if (upVector)
        up = *upVector;

    //set new values
    switchMatrixManipulator->setHomePosition(eye, center, up);
    view->home();
}

