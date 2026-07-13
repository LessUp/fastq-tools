########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(zlib-ng_COMPONENT_NAMES "")
if(DEFINED zlib-ng_FIND_DEPENDENCY_NAMES)
  list(APPEND zlib-ng_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES zlib-ng_FIND_DEPENDENCY_NAMES)
else()
  set(zlib-ng_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(zlib-ng_PACKAGE_FOLDER_RELEASE "/home/shane/.conan2/p/b/zlib-7069d364688bf/p")
set(zlib-ng_BUILD_MODULES_PATHS_RELEASE )


set(zlib-ng_INCLUDE_DIRS_RELEASE "${zlib-ng_PACKAGE_FOLDER_RELEASE}/include")
set(zlib-ng_RES_DIRS_RELEASE )
set(zlib-ng_DEFINITIONS_RELEASE "-DWITH_GZFILEOP")
set(zlib-ng_SHARED_LINK_FLAGS_RELEASE )
set(zlib-ng_EXE_LINK_FLAGS_RELEASE )
set(zlib-ng_OBJECTS_RELEASE )
set(zlib-ng_COMPILE_DEFINITIONS_RELEASE "WITH_GZFILEOP")
set(zlib-ng_COMPILE_OPTIONS_C_RELEASE )
set(zlib-ng_COMPILE_OPTIONS_CXX_RELEASE )
set(zlib-ng_LIB_DIRS_RELEASE "${zlib-ng_PACKAGE_FOLDER_RELEASE}/lib")
set(zlib-ng_BIN_DIRS_RELEASE )
set(zlib-ng_LIBRARY_TYPE_RELEASE STATIC)
set(zlib-ng_IS_HOST_WINDOWS_RELEASE 0)
set(zlib-ng_LIBS_RELEASE z-ng)
set(zlib-ng_SYSTEM_LIBS_RELEASE )
set(zlib-ng_FRAMEWORK_DIRS_RELEASE )
set(zlib-ng_FRAMEWORKS_RELEASE )
set(zlib-ng_BUILD_DIRS_RELEASE )
set(zlib-ng_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(zlib-ng_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${zlib-ng_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${zlib-ng_COMPILE_OPTIONS_C_RELEASE}>")
set(zlib-ng_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${zlib-ng_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${zlib-ng_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${zlib-ng_EXE_LINK_FLAGS_RELEASE}>")


set(zlib-ng_COMPONENTS_RELEASE )