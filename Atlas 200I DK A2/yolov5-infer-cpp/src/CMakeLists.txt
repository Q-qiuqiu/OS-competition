find_package(OpenCV REQUIRED)
add_executable(yolov5_infer
        main.cpp
        model.cpp
        yolov5.cpp
)

if (NOT ${DEV_ENV})
    if (NOT DEFINED ENV{ASCEND_TOOLKIT_HOME})
        set(ASCEND_TOOLKIT_HOME "/usr/local/Ascend/ascend-toolkit/latest")
        message(WARNING "ASCEND_TOOLKIT_HOME is not set, use default path: ${ASCEND_TOOLKIT_HOME}")
    else ()
        set(ASCEND_TOOLKIT_HOME $ENV{ASCEND_TOOLKIT_HOME})
        message(STATUS "ASCEND_TOOLKIT_HOME is set to: ${ASCEND_TOOLKIT_HOME}")
    endif ()

    set(LIB_PATH "${ASCEND_TOOLKIT_HOME}/acllib/lib64/stub")
    set(INC_PATH "${ASCEND_TOOLKIT_HOME}/acllib/include")
else ()
    set(ASCEND_TOOLKIT_HOME "/usr/local/Ascend/ascend-toolkit/7.0.RC1")
    set(LIB_PATH "${ASCEND_TOOLKIT_HOME}/aarch64-linux/lib64")
    set(INC_PATH "${ASCEND_TOOLKIT_HOME}/aarch64-linux/include")
endif ()

target_include_directories(yolov5_infer PRIVATE ${INC_PATH})
target_link_directories(yolov5_infer PRIVATE ${LIB_PATH})

target_link_libraries(yolov5_infer PRIVATE ascendcl ${OpenCV_LIBS})
