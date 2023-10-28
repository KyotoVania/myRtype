# Create an executable for the client component
add_executable(client
    client_tcp_udp.cpp
    # Add any other client-specific source files here
)

# Link the client executable with the common library
target_link_libraries(client common_lib)
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})