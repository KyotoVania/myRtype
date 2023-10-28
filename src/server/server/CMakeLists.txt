# Create an executable for the server component
add_executable(server
    server_tcp.cpp
    # Add any other server-specific source files here
)

# Link the server executable with the common library
target_link_libraries(server common_lib)
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
