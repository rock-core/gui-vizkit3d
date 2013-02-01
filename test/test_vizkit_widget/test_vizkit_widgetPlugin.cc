#include "test_vizkit_widgetPlugin.h"
#include "test_vizkit_widget.h"

Q_EXPORT_PLUGIN2(TestVizkitWidget, TestVizkitWidgetPlugin)

TestVizkitWidgetPlugin::TestVizkitWidgetPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

TestVizkitWidgetPlugin::~TestVizkitWidgetPlugin()
{
}

bool TestVizkitWidgetPlugin::isContainer() const
{
    return false;
}

bool TestVizkitWidgetPlugin::isInitialized() const
{
    return initialized;
}

QIcon TestVizkitWidgetPlugin::icon() const
{
    return QIcon("");
}

QString TestVizkitWidgetPlugin::domXml() const
{
        return "<ui language=\"c++\">\n"
            " <widget class=\"TestVizkitWidget\" name=\"TestVizkitWidget\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "     <width>300</width>\n"
            "     <height>120</height>\n"
            "   </rect>\n"
            "  </property>\n"
//            "  <property name=\"toolTip\" >\n"
//            "   <string>TestVizkitWidget</string>\n"
//            "  </property>\n"
//            "  <property name=\"whatsThis\" >\n"
//            "   <string>TestVizkitWidget</string>\n"
//            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
}

QString TestVizkitWidgetPlugin::group() const {
    return "Rock-Robotics";
}

QString TestVizkitWidgetPlugin::includeFile() const {
    return "TestVizkitWidget/TestVizkitWidget.h";
}

QString TestVizkitWidgetPlugin::name() const {
    return "TestVizkitWidget";
}

QString TestVizkitWidgetPlugin::toolTip() const {
    return whatsThis();
}

QString TestVizkitWidgetPlugin::whatsThis() const
{
    return "";
}

QWidget* TestVizkitWidgetPlugin::createWidget(QWidget *parent)
{
    return new TestVizkitWidget(parent);
}

void TestVizkitWidgetPlugin::initialize(QDesignerFormEditorInterface *core)
{
     if (initialized)
         return;
     initialized = true;
}
