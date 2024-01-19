set(SOURCE_FILES
    main.c
    cpu.c
    gb.c
    memory.c
    timer.c
)

add_executable(nyanGBE ${SOURCE_FILES})
target_include_directories(nyanGBE PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(nyanGBE PRIVATE ${SDL2_LIBRARIES})