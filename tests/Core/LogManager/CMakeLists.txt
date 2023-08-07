add_executable(testsCoreLogMgr src/main.cpp)
add_dependencies(testsCoreLogMgr cm3dcore)

set_target_properties(testsCoreLogMgr PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Core/LogManager
    RUNTIME_OUTPUT_NAME "logmgr"
)
target_include_directories(testsCoreLogMgr PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsCoreLogMgr "$<TARGET_FILE:cm3dcore>")
