macro(install_corvis_spy)
    file(WRITE "${CMAKE_BINARY_DIR}/corvis-spy.sh" "java -cp \"$NUCORE/build/lcm/share/java/*\:$NUCORE/build/libbot/share/java/*\:$CORVIS/build/share/java/*\" lcm.spy.Spy $*\n")
    file(INSTALL "${CMAKE_BINARY_DIR}/corvis-spy.sh"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
endmacro()
