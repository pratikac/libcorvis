local ffi = require("ffi")

ffi.cdef[[
  typedef long time_t;
  typedef struct timeval {
    time_t tv_sec;
    time_t tv_usec;
  } timeval;

  int gettimeofday(timeval* t, void* tzp);
]]

function bot_timestamp_now()
  local t = ffi.new("timeval")
  ffi.C.gettimeofday(t, nil)
  return tonumber(t.tv_sec*1000000 + t.tv_usec)
end
