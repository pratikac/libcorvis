corvis_pythonpath=${CORVIS}/build/lib/python2.7/dist-packages
export PYTHONPATH=${corvis_pythonpath}:${PYTHONPATH}

export LUA_PATH="${CORVIS}/build/share/lua/?.lua;${CORVIS}/build/share/lua/?/init.lua;"$LUA_PATH
