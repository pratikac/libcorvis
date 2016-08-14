set(BOT_VIS_PACKAGES glib-2.0 gthread-2.0 gtk+-2.0 gdk-pixbuf-2.0 libpng gl glu)

add_executable(corvis-viewer
    viewer.cpp
    view_menu.cpp
    udp_util.cpp
    renderer_camera.cpp
    renderer_pointcloud.cpp
    )

pods_use_pkg_config_packages(corvis-viewer ${BOT_VIS_PACKAGES})
target_link_libraries(corvis-viewer
    glut
    z
    jpeg)

pods_install_executables(corvis-viewer)
