add_executable(testsCoreGLSLPreproc src/main.cpp)
add_dependencies(testsCoreGLSLPreproc cm3dcore)

set_target_properties(testsCoreGLSLPreproc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Core/GLSLPreprocessor
    RUNTIME_OUTPUT_NAME "preproc"
)
target_include_directories(testsCoreGLSLPreproc PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsCoreGLSLPreproc "$<TARGET_FILE:cm3dcore>")
add_custom_command(TARGET testsCoreGLSLPreproc POST_BUILD
    COMMAND cp -rf "${CMAKE_CURRENT_SOURCE_DIR}/res" "$<TARGET_FILE_DIR:testsCoreGLSLPreproc>")
