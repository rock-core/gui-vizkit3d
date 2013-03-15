require 'vizkit'
Orocos.initialize

## create a widget for 3d display
view3d = Vizkit.vizkit3d_widget

#show it
view3d.show()

## load and add the 3d plugin for the rock
rbs_plugin = Vizkit.default_loader.RigidBodyStateVisualization
rbs_plugin.connect(SIGNAL "clicked(float,float)") do |x, y|
    puts "Ruby Script: Received mouseDownAt #{x},#{y}"
end

rbs = Types::Base::Samples::RigidBodyState.new

rbs.position = Eigen::Vector3.new(0, 0, 0)
rbs.orientation = Eigen::Quaternion.new(0, 0, 0, 1)

rbs_plugin.updateRigidBodyState(rbs)

#execute the main GUI loop
Vizkit.exec()
