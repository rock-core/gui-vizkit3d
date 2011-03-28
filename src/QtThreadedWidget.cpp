#include "QtThreadedWidget.hpp"
#include <QWidget>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
    
QtThreadedWidgetBase::QtThreadedWidgetBase()
    : argc(0), argv(NULL), running(false)
{
}

QtThreadedWidgetBase::~QtThreadedWidgetBase()
{
    if( isRunning() )
        stop();
}

void QtThreadedWidgetBase::start()
{
    boost::thread( boost::bind( &QtThreadedWidgetBase::run, this ) );
    boost::mutex::scoped_lock lock(mut);
    while(!running)
    {
        cond.wait(lock);
    }
}

void QtThreadedWidgetBase::stop()
{
    app->quit();
    boost::mutex::scoped_lock lock(mut);
    while(running)
    {
        cond.wait(lock);
    }
}

bool QtThreadedWidgetBase::isRunning()
{
    boost::mutex::scoped_lock lock(mut);
    return running;
}

QWidget* QtThreadedWidgetBase::getWidget()
{
    return widget;
}

void QtThreadedWidgetBase::run()
{
    app = boost::shared_ptr<QApplication>( new QApplication( argc, argv ) );
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

