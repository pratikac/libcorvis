find_package(GTK2 REQUIRED gtk)

set(_lcm_inc_dir "${LCM_ROOT}/include")
set(_bot_inc_dir "${LIBBOT_ROOT}/include")
set(_lcm_lib_dir "${LCM_ROOT}/lib")
set(_bot_lib_dir "${LIBBOT_ROOT}/lib")

# set the include dir
set(BOTLCM_INCLUDE_DIRS ${_lcm_inc_dir} ${_bot_inc_dir})

find_library(LCM_LIBRARIES lcm PATHS ${_lcm_lib_dir} DOC "The LCM library")

find_library(LIBBOT_CORE_LIBRARY bot2-core PATHS ${_bot_lib_dir} DOC "The bot-core library")
find_library(LIBBOT_FRAMES_LIBRARY bot2-frames PATHS ${_bot_lib_dir} DOC "The bot-frames library")
find_library(LIBBOT_PARAM_CLIENT_LIBRARY bot2-param-client PATHS ${_bot_lib_dir} DOC "The bot-param-client library")
find_library(LIBBOT_LCMGL_CLIENT_LIBRARY bot2-lcmgl-client PATHS ${_bot_lib_dir} DOC "The bot-lcmgl-client library")
find_library(LIBBOT_LCMGL_RENDERER_LIBRARY bot2-lcmgl-renderer PATHS ${_bot_lib_dir} DOC "The bot-lcmgl-renderer library")
find_library(LIBBOT_VIS_LIBRARY bot2-vis PATHS ${_bot_lib_dir} DOC "The bot-vis library")

set(LIBBOT_LIBRARIES ${LIBBOT_CORE_LIBRARY} ${LIBBOT_FRAMES_LIBRARY} ${LIBBOT_PARAM_CLIENT_LIBRARY} ${LIBBOT_LCMGL_CLIENT_LIBRARY} ${LIBBOT_LCMGL_RENDERER_LIBRARY} ${LIBBOT_VIS_LIBRARY} ${GTK2_GLIB_LIBRARY})

message(STATUS "Found LCM at: ${LCM_LIBRARIES}")
message(STATUS "Found Libbot at: ${LIBBOT_LIBRARIES}")
