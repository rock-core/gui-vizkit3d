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

widget.setTransformation("rover","camera",Qt::Vector3D.new(0,0,1),Qt::Quaternion.new)
widget.setTransformation("rover","laser",Qt::Vector3D.new(0,1,1),Qt::Quaternion::fromAxisAndAngle(1,0,0,-20))
widget.setTransformation("laser","camera2",Qt::Vector3D.new(0,0,0.5),Qt::Quaternion.new)

timer = Qt::Timer.new
angle = 0
timer.connect SIGNAL(:timeout) do
    x = -3+2.0 *Math::cos(angle)
    y = -1+4.0 *Math::sin(angle)
    quat = Qt::Quaternion::fromAxisAndAngle(0,0,1,angle/Math::PI*180)
    widget.setTransformation("world","rover",Qt::Vector3D.new(x,y,0.5),quat)
    angle +=0.005
end
timer.start(10)

widget.show
app.exec


