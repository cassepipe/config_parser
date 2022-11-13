set_config("buildir", ".")

target("exe")            
    set_kind("binary")                                                             
    add_files("**.cpp")                                                       
	add_defines("DEBUG")
    add_cxflags("-Wall", "-Wextra", "-g3")                            
    set_languages("c++98")     
