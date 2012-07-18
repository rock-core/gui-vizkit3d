#include "QtThreadedWidget.hpp"
#include <QWidget>

#include <boost/bind.hpp>
    
QtThreadedWidgetBase::QtThreadedWidgetBase()
    : argc(0), argv(NULL), running(false),restart(true),destroy(false),widget(NULL)
{
}

QtThreadedWidgetBase::~QtThreadedWidgetBase()
{
    {
        boost::lock_guard<boost::mutex> lock(mut);
        destroy = true;
    }

    if( isRunning() )
        stop();


    cond.notify_one();
    gui_thread.join();
}

void QtThreadedWidgetBase::start()
{
    if(isRunning())
        return;

    if(!widget)
    {
        gui_thread = boost::thread( boost::bind( &QtThreadedWidgetBase::run, this ) );
        boost::mutex::scoped_lock lock(mut);
        while(!running)
        {
            cond.wait(lock);
        }
    }
    else
    {
        boost::mutex::scoped_lock lock(mut);
        restart = true;
        cond.notify_one();
        while(!running)
        {
            cond.wait(lock);
        }
    }
}

void QtThreadedWidgetBase::stop()
{
    if(!isRunning())
        return;
    app->closeAllWindows();
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
    {
        boost::lock_guard<boost::mutex> lock( mut );
        app = boost::shared_ptr<QApplication>( new QApplication( argc, argv ) );
        widget = createWidget();
    }
    app->connect( app.get(), SIGNAL(lastWindowClosed()), app.get(), SLOT(quit()) );    

    while(!destroy)
    {
        while( app->startingUp()){usleep(100);};
        widget->show();

        {
            boost::lock_guard<boost::mutex> lock( mut );
            running = true;
        }

        cond.notify_one();
        app->exec();

        //cleanup
        //in some cases the main loop is exit without 
        //processing all events
        //this will result in some strange behaivor if 
        //the event loop is restarted
        app->exit(0);
        while(app->hasPendingEvents())
        {
            usleep(200);
            app->processEvents();
        }
        {
            boost::mutex::scoped_lock lock(mut);
            running = false;
            restart = false;
            cond.notify_one();
            while(!restart && !destroy)
                cond.wait(lock);
        }
    }

    //delete all objects from the same thread they were created
    delete widget;
    widget = NULL;
    app.reset();
}

