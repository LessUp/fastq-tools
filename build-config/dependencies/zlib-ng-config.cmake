########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(zlib-ng_FIND_QUIETLY)
    set(zlib-ng_MESSAGE_MODE VERBOSE)
else()
    set(zlib-ng_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/zlib-ngTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${zlib-ng_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(zlib-ng_VERSION_STRING "2.3.3")
set(zlib-ng_INCLUDE_DIRS ${zlib-ng_INCLUDE_DIRS_RELEASE} )
set(zlib-ng_INCLUDE_DIR ${zlib-ng_INCLUDE_DIRS_RELEASE} )
set(zlib-ng_LIBRARIES ${zlib-ng_LIBRARIES_RELEASE} )
set(zlib-ng_DEFINITIONS ${zlib-ng_DEFINITIONS_RELEASE} )


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${zlib-ng_BUILD_MODULES_PATHS_RELEASE} )
    message(${zlib-ng_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


