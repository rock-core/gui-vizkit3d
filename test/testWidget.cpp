#include <vizkit/QVizkitWidget.hpp>
#include <vizkit/QtThreadedWidget.hpp>
#include <vizkit/TrajectoryVisualization.hpp>

namespace vizkit
{
class QTrajectoryWidget : public QVizkitWidget
{
    public:
	QTrajectoryWidget( QWidget* parent = 0, Qt::WindowFlags f = 0 )
	    : QVizkitWidget( parent, f ), trajViz( new TrajectoryVisualization() )
	{
	    addDataHandler( trajViz.get() );
	}

	~QTrajectoryWidget()
	{
	    removeDataHandler( trajViz.get() );
	}

	void addPosition( const Eigen::Vector3d& point )
	{
	    trajViz->updateData( point );
	}

    private:
	boost::shared_ptr<TrajectoryVisualization> trajViz;
};
}

int main( int argc, char **argv )
{
    QtThreadedWidget<vizkit::QTrajectoryWidget> app;
    app.start();

    for( int i=0; i<10000 && app.isRunning(); i++ )
    {
	double r = i/1000.0;
	double s = r/10;
	app.widget->addPosition( Eigen::Vector3d( cos(r) * s, sin(r) * s, s ) );

	usleep( 1000 );
    }
}

