#include "QVizkitWidgetLoader.hpp"
#include <QtPlugin>

QVizkitWidgetLoader::QVizkitWidgetLoader(QObject* parent): QObject(parent)
{
    initialized = false;
}

void QVizkitWidgetLoader::initialize(QDesignerFormEditorInterface* core)
{
    if (initialized) return;

    initialized = true;
}

bool QVizkitWidgetLoader::isInitialized() const
{
    return initialized;
}

QWidget* QVizkitWidgetLoader::createWidget(QWidget* parent)
{
    return new vizkit::QVizkitWidget(parent, Qt::Widget);
}

QString QVizkitWidgetLoader::domXml() const
{
    return  "<ui language=\"c++\">\n"
            " <widget class=\"vizkit::QVizkitWidget\" name=\"QVizkitWidget\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "    <width>640</width>\n"
            "    <height>480</height>\n"
            "   </rect>\n"
            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
}

bool QVizkitWidgetLoader::isContainer() const
{
    return false;
}

QIcon QVizkitWidgetLoader::icon() const
{
    return QIcon();
}

QString QVizkitWidgetLoader::includeFile() const
{
    return "QVizkitWidget.hpp";
}

QString QVizkitWidgetLoader::name() const
{
    return "vizkit::QVizkitWidget";
}

QString QVizkitWidgetLoader::group() const
{
    return "Vizkit Widgets";
}

QString QVizkitWidgetLoader::toolTip() const
{
    return "";
}

QString QVizkitWidgetLoader::whatsThis() const
{
    return "";
}

Q_EXPORT_PLUGIN2(QVizkitWidgetLoader, QVizkitWidgetLoader)