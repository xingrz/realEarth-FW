set(COMPONENT_SRCDIRS
    "."
    "display"
    "earth"
    "network"
    "tasks"
)

set(COMPONENT_ADD_INCLUDEDIRS "." "include")

set(COMPONENT_EMBED_FILES
    "images/pic_offline.jpg"
    "images/pic_loading.jpg"
    "images/pic_qrcode.jpg"
)

register_component()

if(NOT $ENV{EARTH_URL} STREQUAL "")
    target_compile_options(${COMPONENT_LIB} PRIVATE
        -DEARTH_URL="$ENV{EARTH_URL}"
    )
endif()

execute_process(COMMAND git describe --tags --abbrev=0 OUTPUT_VARIABLE GIT_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-list --count HEAD OUTPUT_VARIABLE GIT_INCREMENT OUTPUT_STRIP_TRAILING_WHITESPACE)

target_compile_options(
    ${COMPONENT_LIB} PRIVATE
    -DGIT_TAG="${GIT_TAG}"
    -DGIT_COMMIT="${GIT_COMMIT}"
    -DGIT_INCREMENT=${GIT_INCREMENT}
)
