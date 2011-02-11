#ifndef __ENVIEW_QTTHREADEDWIDGET__
#define __ENVIEW_QTTHREADEDWIDGET__

#include <QApplication>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>

template <class T>
class QtThreadedWidget
{
    int argc;
    char **argv;
    boost::shared_ptr<QApplication> app;

    boost::mutex mut;
    boost::condition_variable cond;
    bool running;

    T *widget;

public:

    T *getWidget()
    {
	if(!running)
	{
	    throw std::runtime_error("Tried to access widget before thread was started");
	}
	
	return widget;
    }
    
    QtThreadedWidget()
	: argc(0), argv(NULL), running(false)
    {
    }

    void start()
    {
	boost::thread( boost::bind( &QtThreadedWidget<T>::run, this ) );
	boost::mutex::scoped_lock lock(mut);
	while(!running)
	{
	    cond.wait(lock);
	}
    }

    void stop()
    {
	app->quit();
	boost::mutex::scoped_lock lock(mut);
	while(running)
	{
	    cond.wait(lock);
	}
    }

    ~QtThreadedWidget()
    {
	if( isRunning() )
	    stop();
    }

    bool isRunning()
    {
	boost::mutex::scoped_lock lock(mut);
	return running;
    }

protected:
    void run()
    {
	app = boost::shared_ptr<QApplication>( new QApplication( argc, argv ) );
	widget = new T();
	app->connect( app.get(), SIGNAL(lastWindowClosed()), app.get(), SLOT(quit()) );    
	widget->show();
	while( app->startingUp() );

	{
	    boost::lock_guard<boost::mutex> lock( mut );
	    running = true;
	}
	cond.notify_one();

	app->exec();

	{
	    boost::lock_guard<boost::mutex> lock( mut );
	    running = false;
	}
	cond.notify_one();
    }
};

#endif
