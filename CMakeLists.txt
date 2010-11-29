PROJECT(angkor)

set(CMAKE_BUILD_TYPE Release)

ADD_EXECUTABLE(angkor tweakbargui.cpp main.cpp)

target_link_libraries(angkor freenect OpenThreads osg osgSim osgViewer AntTweakBar)