add_executable(testsUtilityGLSLPreproc src/main.cpp)
add_dependencies(testsUtilityGLSLPreproc cm3dcore)
add_dependencies(testsUtilityGLSLPreproc cm3dutil_gl)

set_target_properties(testsUtilityGLSLPreproc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Utility/GL/GLSLPreprocessor
    RUNTIME_OUTPUT_NAME "glslpp"
)
target_include_directories(testsUtilityGLSLPreproc PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsUtilityGLSLPreproc "$<TARGET_FILE:cm3dcore>" "$<TARGET_FILE:cm3dutil_gl>")
add_custom_command(TARGET testsUtilityGLSLPreproc POST_BUILD
    COMMAND cp -rf "${CMAKE_CURRENT_SOURCE_DIR}/res" "$<TARGET_FILE_DIR:testsUtilityGLSLPreproc>")
