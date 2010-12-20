#ifndef __ENVIEW_QTTHREADEDWIDGET__
#define __ENVIEW_QTTHREADEDWIDGET__

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

template <class T>
class QtThreadedWidget
{
    int argc;
    char **argv;
    boost::shared_ptr<QApplication> app;
    bool initialized;

public:
    T *widget;

    QtThreadedWidget()
	: argc(0), argv(NULL), initialized(false)
    {
	boost::thread( boost::bind( &QtThreadedWidget<T>::run, this ) );
    }

    ~QtThreadedWidget()
    {
	app->quit();
    }

    bool isInitialized() const { return initialized && !app->startingUp(); }

protected:
    void run()
    {
	app = boost::shared_ptr<QApplication>( new QApplication( argc, argv ) );
	widget = new T();
	app->connect( app.get(), SIGNAL(lastWindowClosed()), app.get(), SLOT(quit()) );    
	widget->show();
	initialized = true;
	app->exec();
    }
};

#endif
