# - Set allows giving a value to a variable similar to: UI_FILES = "file1.ui file2.ui file3.ui"
# - A list of .ui Files is made to make it easier to work with them
# - In this case, there are no UI files, this "set" is just used to keep the code consitent
# - If you add UI files to this library please uncomment the qt5_wrap_ui line below or add AUTOUIC
set(UI_FILES
)

# - Set allows giving a value to a variable similar to: H_FILES = "file1.h file2.h file3.h"
# - A list of .h Files is made to make it easier to work with them
# - All the h files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files are parts of it
set(H_FILES
    source_code/CBenchmark.h
    source_code/CDomainCartesian.h
    source_code/CExecutorControlOpenCL.h
    source_code/CLog.h
    source_code/CLoggingInterface.h
    source_code/CModel.h
    source_code/COpenCLSimpleManager.h
    source_code/COCLBuffer.h
    source_code/COCLDevice.h
    source_code/COCLKernel.h
    source_code/COCLProgram.h
    source_code/common.h
    source_code/CProfiler.h
    source_code/CScheme.h
    source_code/CSchemeGodunov.h
    source_code/CSchemeInertial.h
    source_code/CSchemeMUSCLHancock.h
    source_code/CSchemePromaides.h
    source_code/resource.h
    source_code/util.h
)

# - Set allows giving a value to a variable similar to: CPP_FILES = "file1.cpp file2.cpp file3.cpp"
# - A list of h Files is made to make it easier to work with them
# - All the .cpp files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files to compile
set(CPP_FILES           
    source_code/CBenchmark.cpp
    source_code/CDomainCartesian.cpp
    source_code/CExecutorControlOpenCL.cpp
    source_code/CLog.cpp
    source_code/CModel.cpp
    source_code/COpenCLSimpleManager.cpp
    source_code/COCLBuffer.cpp
    source_code/COCLDevice.cpp
    source_code/COCLKernel.cpp
    source_code/COCLProgram.cpp
    source_code/CProfiler.cpp
    source_code/CScheme.cpp
    source_code/CSchemeGodunov.cpp
    source_code/CSchemeInertial.cpp
    source_code/CSchemeMUSCLHancock.cpp
    source_code/CSchemePromaides.cpp
    source_code/util.cpp
    source_code/windows_platform.cpp
)

# - Set allows giving a value to a variable similar to: CPP_FILES = "file1.cpp file2.cpp file3.cpp"
# - A list of OpenCL Files is made to make it easier to work with them
# - All the .clc & .clh files in this library need to be added here MANUALLY.
# - This variable is passed to the library so that they are added to the project
set(OPENCL_FILES
    source_code/opencl/CLBoundaries.clc
    source_code/opencl/CLBoundaries.clh
    source_code/opencl/CLDomainCartesian.clc
    source_code/opencl/CLDomainCartesian.clh
    source_code/opencl/CLDynamicTimestep.clc
    source_code/opencl/CLDynamicTimestep.clh
    source_code/opencl/CLFriction.clc
    source_code/opencl/CLFriction.clh
    source_code/opencl/CLSchemeGodunov.clc
    source_code/opencl/CLSchemeGodunov.clh
    source_code/opencl/CLSchemeInertial.clc
    source_code/opencl/CLSchemeInertial.clh
    source_code/opencl/CLSchemeMUSCLHancock.clc
    source_code/opencl/CLSchemeMUSCLHancock.clh
    source_code/opencl/CLSchemePromaides.clc
    source_code/opencl/CLSchemePromaides.clh
    source_code/opencl/CLSlopeLimiterMINMOD.clc
    source_code/opencl/CLSlopeLimiterMINMOD.clh
    source_code/opencl/CLSolverHLLC.clc
    source_code/opencl/CLSolverHLLC.clh
    source_code/opencl/CLUniversalHeader.clh
)
# - Set allows giving a value to a variable similar to: RCFiles = "Resource1.rc Resource2.rc Resource3.rc"
# - This set is not used.
# - It is left here because the following message are important:
# - THIS NEEDS TO BE INCULDED IN THE EXECUTABLE
# - THIS MEANS CLCode.rc NEEDS TO BE ADDED TO "add_executable(ProMaIDeS)"
# - add_executable(ProMaIDeS) IS FOUND IN THE TOP CMakeLists.txt
# - AFAIK you can't add resources to a shared library 
set(RCFiles
    CLCode.rc
)

# - qt5_wrap_ui: Creates rules for calling the User Interface Compiler (uic) on the given .ui files
# - In case there are any UI files. Uncomment the line below
#qt5_wrap_ui(UI_FILES ${UI_FILES})

# - add_library create the library
# - The add_library is the engine here and is defined as add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] [<source>...])
# - solvergpu is the name given to the library
# - What follows are the source codes
add_library(solvergpu ${UI_FILES} ${H_FILES} ${CPP_FILES} ${OPENCL_FILES})

# - source_group used to group a bunch of files under a folder
# - This is just to get the OpenCL files under a folder in the solution explorer of VS
source_group("OpenCLFiles" FILES ${OPENCL_FILES})

# To allow correct correct recovery of exeptions through the stack
#if(MSVC)
#    target_compile_options(solvergpu PRIVATE /EHsc)
#endif()

# - find_package trys to find the specifieced package using Find<package>.cmake files
# - CMake has some builtin Find<package>.cmake files, one of which is that of OpenCL
# - this functions defines ${OpenCL_INCLUDE_DIRS} as well as OpenCL::OpenCL
find_package(OpenCL REQUIRED)

# - target_include_directories sets the include directories for a specfic target
# - ${OpenCL_INCLUDE_DIRS} is defined from the find_package(OpenCL REQUIRED)
# - In this case the include directories for the solvergpu package are defined
# - It is set to PUBLIC meaning the parent targerts also inherit these directories
# - For example: the HYD Library is a parent of solvergpu and has access to things like: include "GodunovScheme.h"
# - ${OpenCL_INCLUDE_DIRS} is also needed to use things like: #include "CL/Cl.h" or "OpenCl.h"
# - These are headers required for working with the opencl stuff
target_include_directories(solvergpu
    PUBLIC
        source_code/
        ${OpenCL_INCLUDE_DIRS}
)

# - target_link_libraries links libraries to a specfic target
# - OpenCL::OpenCL is defined from the find_package(OpenCL REQUIRED)
# - In this case linking the OpenCL Library to the solvergpu library
# - The headers include the function names but not what they actually do
# - The dlls or libs are what include the functions themselves.
# - Linking the opencl is required so that solvergpu knows where to go when an opencl function is called
target_link_libraries(solvergpu OpenCL::OpenCL)

# - message is just a print command similar to a cout <<
# - The OpenCL library Include folder is printed here for debug purposes
message("Including OpenCL libraries to solvergpu lib, path is:" ${OpenCL_INCLUDE_DIRS})