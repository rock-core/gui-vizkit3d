###### Uncomment the following block for Qt setup
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# Add your .ui files ot the following line (and uncomment it)
QT4_WRAP_UI(ui_cpp myproject.ui)
# Add the .h files that define QObject subclasses (that have a Q_OBJECT
# statement)
QT4_WRAP_CPP(moc_cpp dummy.h)


