#ifndef TEST_VIZKIT_WIDGET_H
#define TEST_VIZKIT_WIDGET_H

#include <QtGui>
#include <base/samples/frame.h>
#include <base/motion_command.h>
#include <ui_widget.h>

class TestVizkitWidget : public QMainWindow
{
    Q_OBJECT
public:
    TestVizkitWidget(QWidget *parent = 0);
    virtual ~TestVizkitWidget();

public slots:
    void setFrame(const base::samples::frame::Frame &frame);
    void reachable();
    void unreachable();
    const base::samples::frame::Frame getFrame()const;
    base::AUVMotionCommand getMotionCommand() const;
    void callbackMotionCommand(const QString &string);

signals:
    void frameChanged();
    void valueChanged(int value);
    void motionCommandChanged();

private:
    Ui::MainWindow window;
    base::samples::frame::Frame frame;
};

#endif /* TEST_VIZKIT_WIDGET_H */
