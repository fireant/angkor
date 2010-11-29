PROJECT(angkor)

set(CMAKE_BUILD_TYPE Release)

ADD_EXECUTABLE(angkor basiceventhandler.cpp main.cpp)

target_link_libraries(angkor freenect OpenThreads osg osgSim osgViewer)