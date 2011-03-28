#ifndef __ENVIEW_QTTHREADEDWIDGET__
#define __ENVIEW_QTTHREADEDWIDGET__

#include <QApplication>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

class QtThreadedWidgetBase
{
    int argc;
    char **argv;
    boost::shared_ptr<QApplication> app;

    boost::mutex mut;
    boost::condition_variable cond;
    bool running;

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
};

template <class T>
class QtThreadedWidget : public QtThreadedWidgetBase
{
public:
    void start()
    {
        widget = new T();
        QtThreadedWidgetBase::start();
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
