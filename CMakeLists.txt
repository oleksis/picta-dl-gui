cmake_minimum_required(VERSION 3.1.0)
project(Picta-dl_GUI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(TARGET ${CMAKE_PROJECT_NAME})

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

include_directories(${Qt5Widgets_INCLUDES} ${Qt5Network_INCLUDES})

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Network_DEFINITIONS})

set(QT5_LIBRARIES Qt5::Widgets Qt5::Network)

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

set(SOURCES 
    main.cpp 
    RunGuard.cpp 
    mainwindow.cpp 
    configuration.cpp 
    simplecrypt.cpp 
    information.cpp 
)

set(HEADERS 
    mainwindow.h 
    RunGuard.h 
    configuration.h 
    simplecrypt.h 
    information.h
)

set(FORMS 
    mainwindow.ui 
    configuration.ui 
    information.ui
)

set(RESOURCES 
    Resources/images.qrc 
    Resources/styles.qrc
)

set(RES_MANIFEST 
    Resources/resManifest.rc
)

if(WIN32)
    set_source_files_properties(${RES_MANIFEST} LANGUAGE RC)
    list(APPEND RESOURCES ${RES_MANIFEST})
endif(WIN32)

# include(CTest)
# enable_testing()

add_executable(${TARGET} ${HEADERS} ${FORMS} ${SOURCES} ${RESOURCES})

target_link_libraries(${TARGET} ${QT5_LIBRARIES})

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
