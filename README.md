vizkit3d: a lightweight infrastructure to build data display using C++, OpenSceneGraph and Qt

vizkit3d is a lightweight infrastructure to allow the creation of 3D views to display
data (and in particular data related to robotic sensing) live. It is used as the
core 3D view implementation in [Rock, the Robot Construction Kit](http://rock-robotics.org).
Nonetheless, vizkit3d uses only widely accepted libraries (Qt and OSG) and depends
on the Rock framework in no way. It only uses Rocks cmake macros.

Vizkit3D functionality can also be accessed from Ruby by using
[the vizkit library](https://github.com/rock-core/gui-vizkit).

Qt5 Porting Guide
=================

Different to Qt4, Qt5 uses the meta object compiler to register plugins.
This requires a few changes to the plugin objects(the classes derived from
`vizkit3d::VizkitPluginFactory`) setup:

* The Qt4 macro must be protected against Qt5:
```
#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2(QtPluginVizkitBase, QtPluginVizkitBase)
#endif
```
* The class must use Q_OBJECT and have the line
```
#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID "rock.vizkit3d.VizkitPluginFactory")
#endif
```
* Since the class now needs to be processed by the moc, it is required that the
  class is declared in a header.

If the plugin is instantiated using the `VizkitQtPlugin(name)` macro,
that needs to be replaced by two macro invocations; in a relevant header, use
`VizkitQtPluginHeaderDecls(name)`, and in a compilation object file(.cc/.cpp),
add `VizkitQtPluginImpl(name)` (probably just replacing the
`VizkitQtPlugin(name)` invocation).

It is possible to build both a Qt4 and Qt5 version of the plugins by avoiding
placing the Qt4/Qt5 libraries and includes in the cmake directory scope
variables.
For this, it is easiest to use the Qt4/Qt5 targets provided by
`findPackage(Qt4 COMPONENTE QtCore ...)`/`findPackage(Qt5 COMPONENTS Core ...)`
in `target_link_libraries(viz-qt4 PRIVATE Qt4::QtCore ...)` and
`target_link_libraries(viz-qt5 PRIVATE Qt5::Core ...)`.
