
if(NOT TARGET susbdmx::susbdmx)
    add_library(susbdmx::susbdmx SHARED IMPORTED)
    
    set_target_properties(susbdmx::susbdmx PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../../include"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/../../lib/susbdmx.lib"
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../../bin/susbdmx.dll"
    )

endif()
