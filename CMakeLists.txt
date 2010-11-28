PROJECT(osg_kinect)

set(CMAKE_BUILD_TYPE Release)

ADD_EXECUTABLE(osg_kinect main.cpp)

target_link_libraries(osg_kinect freenect OpenThreads osg osgSim osgViewer)