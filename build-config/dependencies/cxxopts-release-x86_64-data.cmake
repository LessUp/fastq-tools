########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(cxxopts_COMPONENT_NAMES "")
if(DEFINED cxxopts_FIND_DEPENDENCY_NAMES)
  list(APPEND cxxopts_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES cxxopts_FIND_DEPENDENCY_NAMES)
else()
  set(cxxopts_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(cxxopts_PACKAGE_FOLDER_RELEASE "/home/shane/.conan2/p/cxxop824b9f422471c/p")
set(cxxopts_BUILD_MODULES_PATHS_RELEASE )


set(cxxopts_INCLUDE_DIRS_RELEASE "${cxxopts_PACKAGE_FOLDER_RELEASE}/include")
set(cxxopts_RES_DIRS_RELEASE )
set(cxxopts_DEFINITIONS_RELEASE )
set(cxxopts_SHARED_LINK_FLAGS_RELEASE )
set(cxxopts_EXE_LINK_FLAGS_RELEASE )
set(cxxopts_OBJECTS_RELEASE )
set(cxxopts_COMPILE_DEFINITIONS_RELEASE )
set(cxxopts_COMPILE_OPTIONS_C_RELEASE )
set(cxxopts_COMPILE_OPTIONS_CXX_RELEASE )
set(cxxopts_LIB_DIRS_RELEASE )
set(cxxopts_BIN_DIRS_RELEASE )
set(cxxopts_LIBRARY_TYPE_RELEASE UNKNOWN)
set(cxxopts_IS_HOST_WINDOWS_RELEASE 0)
set(cxxopts_LIBS_RELEASE )
set(cxxopts_SYSTEM_LIBS_RELEASE )
set(cxxopts_FRAMEWORK_DIRS_RELEASE )
set(cxxopts_FRAMEWORKS_RELEASE )
set(cxxopts_BUILD_DIRS_RELEASE )
set(cxxopts_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(cxxopts_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${cxxopts_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${cxxopts_COMPILE_OPTIONS_C_RELEASE}>")
set(cxxopts_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${cxxopts_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${cxxopts_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${cxxopts_EXE_LINK_FLAGS_RELEASE}>")


set(cxxopts_COMPONENTS_RELEASE )