begin
    require 'pp'
    require 'Qt4'
    require 'qtuitools'
rescue Exception => e
    log = Logger.new(STDOUT)
    log.error "!!! Cannot load Qt4 ruby bindings !!!"
    raise e
end

app = Qt::Application.new(ARGV)
loader = Qt::UiLoader.new()
widget = loader.createWidget("vizkit3d::Vizkit3DWidget")
plugin = widget.loadPlugin("vizkit3d","GridVisualization")
plugin2 = widget.loadPlugin("sfm","")


widget.show

pp widget.getAvailablePlugins
app.exec


