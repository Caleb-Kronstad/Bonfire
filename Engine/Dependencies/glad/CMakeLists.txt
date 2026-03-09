add_library(GLAD STATIC
    include/glad/glad.h
    include/KHR/khrplatform.h
    src/glad.cpp
)

target_include_directories(GLAD PUBLIC include)

target_compile_definitions(GLAD PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:Dist>:DIST>
    $<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS>
)

set_target_properties(GLAD PROPERTIES POSITION_INDEPENDENT_CODE ON)
