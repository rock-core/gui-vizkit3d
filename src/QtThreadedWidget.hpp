#ifndef __ENVIEW_QTTHREADEDWIDGET__
#define __ENVIEW_QTTHREADEDWIDGET__

#include <QApplication>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

class QtThreadedWidgetBase
{
    int argc;
    char **argv;
    boost::shared_ptr<QApplication> app;

    boost::mutex mut;
    boost::condition_variable cond;
    boost::thread gui_thread;
    bool running;
    bool restart;
    bool destroy;

public:
    QtThreadedWidgetBase();
    virtual ~QtThreadedWidgetBase();

    virtual void start();
    virtual void stop();
    bool isRunning();
    QWidget* getWidget();

protected:
    QWidget *widget;
    void run();
    virtual QWidget* createWidget() = 0;
};

template <class T>
class QtThreadedWidget : public QtThreadedWidgetBase
{
public:
    QWidget* createWidget()
    {
	return new T();
    }

    T *getWidget()
    {
	if(!isRunning())
	{
	    throw std::runtime_error("Tried to access widget before thread was started");
	}
	
        return dynamic_cast<T*>(QtThreadedWidgetBase::getWidget());
    }
};


#endif
