#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "test"
#define BOOST_AUTO_TEST_MAIN

#include <vizkit/QVizkitMainWindow.hpp>
#include <vizkit/QVizkitWidget.hpp>
#include <vizkit/QtThreadedWidget.hpp>
#include <vizkit/TrajectoryVisualization.hpp>
#include <vizkit/WaypointVisualization.hpp>
#include <vizkit/MotionCommandVisualization.hpp>
#include <boost/test/unit_test.hpp>

namespace vizkit
{
    
template <class T, class D>
class QVisualisationTestWidget : public QVizkitMainWindow
{
    public:
        QVisualisationTestWidget( QWidget* parent = 0, Qt::WindowFlags f = 0 )
            : QVizkitMainWindow(parent, f), viz(new T())
        {
            addPlugin( viz.get() );
        }

        ~QVisualisationTestWidget()
        {
            removePlugin( viz.get() );
        }

        void updateData( const D &data )
        {
            viz->updateData(data);
        }
      
        boost::shared_ptr<T> viz;
};

}

BOOST_AUTO_TEST_SUITE(vizkit_test)

/** Test of the TrajectoryVisualization */
BOOST_AUTO_TEST_CASE(trajectoryVisualization_test) 
{
    std::cout << "Testing TrajectoryVisualization" << std::endl;
    QtThreadedWidget<vizkit::QVisualisationTestWidget<vizkit::TrajectoryVisualization, Eigen::Vector3d> > app;
    app.start();
    std::cout << "Close the visualization window to abort this test." << std::endl;
    for( int i=0; i<1000 && app.isRunning(); i++ )
    {
        double r = i/100.0;
        double s = r/10.0;
        app.getWidget()->updateData( Eigen::Vector3d( cos(r) * s, sin(r) * s, s ) );
	app.getWidget()->viz->setColor( 1.0, 0, 0, 1.0 );
        
        usleep( 10000 );
    }
}

/** Test of the WaypointVisualization */
BOOST_AUTO_TEST_CASE(waypointVisualization_test) 
{
    std::cout << "Testing WaypointVisualization" << std::endl;
    QtThreadedWidget<vizkit::QVisualisationTestWidget<vizkit::WaypointVisualization, base::Waypoint> > app;
    app.start();
    std::cout << "Close the visualization window to abort this test." << std::endl;
    for( int i=0; i<10000 && app.isRunning(); i++ )
    {
        double r = i/1000.0;
        double s = r/10;
        app.getWidget()->updateData( base::Waypoint(Eigen::Vector3d( cos(r) * r, sin(r) * r, s), s, s*r, s ));
        
        usleep( 1000 );
    }
}

/** Test of the MotionCommandVisualization */
BOOST_AUTO_TEST_CASE(motionCommandVisualization_test)
{
    std::cout << "Testing MotionCommandVisualization" << std::endl;
    QtThreadedWidget<vizkit::QVisualisationTestWidget<vizkit::MotionCommandVisualization, std::pair<double, double> > > app;
    app.start();
    std::cout << "Close the visualization window to abort this test." << std::endl;
    for( int i=0; i<10000 && app.isRunning(); i++ )
    {
        double r = i/1000.0;
        double s = r/10;
        app.getWidget()->updateData(std::pair<double, double>(s, sin(r) * r));
        
        usleep( 1000 );
    }
}

/** Test of the changeCameraView method */
BOOST_AUTO_TEST_CASE(changeCameraView_test) 
{
    std::cout << "Testing changeCameraView method" << std::endl;
    QtThreadedWidget<vizkit::QVisualisationTestWidget<vizkit::TrajectoryVisualization, Eigen::Vector3d> > app;
    app.start();
    std::cout << "Close the visualization window to abort this test." << std::endl;
    for( int i=0; i<30000 && app.isRunning(); i++ )
    {
        double r = i/1000.0;
        double s = r/10;
        app.getWidget()->getVizkitWidget()->changeCameraView(osg::Vec3d(cos(r)*s,sin(r)*s,0));
        
        usleep( 500 );
    }
    
    for( int i=0; i<30000 && app.isRunning(); i++ )
    {
        double r = i/1000.0;
        double s = r/10;
        app.getWidget()->getVizkitWidget()->changeCameraView(osg::Vec3d(cos(r)*s,sin(r)*s,0), osg::Vec3d(cos(r)*s,(sin(r)*s)-20,20));
        
        usleep( 500 );
    } 
}

BOOST_AUTO_TEST_SUITE_END();
