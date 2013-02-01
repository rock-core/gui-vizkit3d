
Vizkit::UiLoader::extend_cplusplus_widget_class "TestVizkitWidget" do

    #called when the widget is created
    def initialize_vizkit_extension
        #activate Typelib transport via qt slots
        extend Vizkit::QtTypelibExtension
    end

    #called each time vizkit wants to display a new 
    #port with this widget
    def config(value,options)

    end

    #called each time new data are available on the 
    #orocos port connected to the widget the name is
    #custom and can be set via register_widget_for
    def update(sample,port_name)
        #mySlot(sample)
    end
end

# register widget for a specific Typelib type to be 
# accessible via rock tooling (rock-replay,...)
# multiple register_widget_for are allowed for each widget
# Vizkit::UiLoader.register_widget_for("TestVizkitWidget","/base/Angle",:update)
