
ExternalProject_Add(uncrustify
		    DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
		    SOURCE_DIR ${uncrustify}
		    BUILD_IN_SOURCE 1
   		    CONFIGURE_COMMAND ${configure}
   		    BUILD_COMMAND ${make} )
