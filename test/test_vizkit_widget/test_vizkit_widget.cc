#include "test_vizkit_widget.h"

using namespace base::samples;

TestVizkitWidget::TestVizkitWidget(QWidget *parent)
    : QMainWindow(parent)
{
    window.setupUi(this);
    show();
    connect(window.position_button,SIGNAL(clicked(bool)),this,SIGNAL(motionCommandChanged()));
}

TestVizkitWidget::~TestVizkitWidget()
{
}

void TestVizkitWidget::setFrame(const base::samples::frame::Frame &frame)
{
    this->frame = frame;
    window.frame_time->setText(frame.time.toString().c_str());
}

void TestVizkitWidget::reachable()
{
    window.event_list->addItem("task is reachable");
}

void TestVizkitWidget::set2Int(int i1,int i2)
{
}

void TestVizkitWidget::unreachable()
{
    window.event_list->addItem("task is unreachable");
}

void TestVizkitWidget::callbackMotionCommand(const QString &string)
{
    QMessageBox::about(this,"Info",string);
}

base::AUVMotionCommand TestVizkitWidget::getMotionCommand()const
{
    base::AUVMotionCommand command;
    command.heading = window.command_heading->value();
    command.z= window.command_z->value();
    command.x_speed= window.command_x_speed->value();
    command.y_speed= window.command_y_speed->value();
    return command;
}

const frame::Frame TestVizkitWidget::getFrame()const
{
    return frame;
}
