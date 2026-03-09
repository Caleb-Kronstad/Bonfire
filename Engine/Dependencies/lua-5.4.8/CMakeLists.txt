file(GLOB LUA_SOURCES "src/*.c" "src/*.h")
list(REMOVE_ITEM LUA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lua.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/luac.c"
)

add_library(LUA STATIC ${LUA_SOURCES})

target_include_directories(LUA PUBLIC src)

target_compile_definitions(LUA PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:Dist>:DIST>
    $<$<BOOL:${WIN32}>:LUA_USE_WINDOWS>
    $<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS>
    $<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:LUA_USE_LINUX>
)

if(UNIX AND NOT APPLE)
    target_link_libraries(LUA PUBLIC dl m)
endif()

set_target_properties(LUA PROPERTIES POSITION_INDEPENDENT_CODE ON)
