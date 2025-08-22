
rock_find_qt4(OPTIONAL)
rock_find_qt5(OPTIONAL Core)

set(VIZKIT3D_VIZ_SOURCES
    PluginLoader.cpp
    GridVisualization.cpp
    TextureBoxVisualization.cpp
    ModelVisualization.cpp
    )

set(VIZKIT3D_VIZ_MOC
    GridVisualization.hpp
    TextureBoxVisualization.hpp
    ModelVisualization.hpp
    PluginLoader.hpp
    )

set(VIZKIT3D_VIZ_HEADERS
    GridVisualization.hpp
    TextureBoxVisualization.hpp
    ModelVisualization.hpp
    )

rock_qt_vizkit_plugin(
    TARGETPREFIX vizkit3d-viz
    QT4_SUFFIX ""
    SOURCES ${VIZKIT3D_VIZ_SOURCES}
    MOC ${VIZKIT3D_VIZ_MOC}
    DEPS_QT vizkit3d
    HEADERS ${VIZKIT3D_VIZ_HEADERS}
)
