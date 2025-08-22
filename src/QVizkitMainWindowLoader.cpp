#include "QVizkitMainWindowLoader.hpp"
#include <QtPlugin>

QVizkitMainWindowLoader::QVizkitMainWindowLoader(QObject* parent): QObject(parent)
{
    initialized = false;
}

void QVizkitMainWindowLoader::initialize(QDesignerFormEditorInterface* core)
{
    if (initialized) return;

    initialized = true;
}

bool QVizkitMainWindowLoader::isInitialized() const
{
    return initialized;
}

QWidget* QVizkitMainWindowLoader::createWidget(QWidget* parent)
{
    return new vizkit3d::QVizkitMainWindow(parent, Qt::Widget);
}

QString QVizkitMainWindowLoader::domXml() const
{
    return  "<ui language=\"c++\">\n"
            " <widget class=\"vizkit3d::QVizkitMainWindow\" name=\"QVizkitMainWindow\">\n"
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

bool QVizkitMainWindowLoader::isContainer() const
{
    return false;
}

QIcon QVizkitMainWindowLoader::icon() const
{
    return QIcon();
}

QString QVizkitMainWindowLoader::includeFile() const
{
    return "QVizkitMainWindow.hpp";
}

QString QVizkitMainWindowLoader::name() const
{
    return "vizkit3d::QVizkitMainWindow";
}

QString QVizkitMainWindowLoader::group() const
{
    return "Vizkit Widgets";
}

QString QVizkitMainWindowLoader::toolTip() const
{
    return "";
}

QString QVizkitMainWindowLoader::whatsThis() const
{
    return "";
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(QVizkitMainWindowLoader, QVizkitMainWindowLoader)
#endif
