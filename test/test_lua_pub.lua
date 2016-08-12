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


local lcm = require "lcm"
local pointcloud_t = require "corvis.pointcloud_t"
local pointcloud_type_t = require "corvis.pointcloud_type_t"
local image_t = require 'corvis.image_t'

lc = lcm.lcm.new()

for i=1,10000000 do
    sys.sleep(0.03)

    --[[
    local p = pointcloud_t:new()
    p.utime = bot_timestamp_now()
    p.size = 500000
    for i=1,p.size do
        p.points[i] = {0,1,2}
    end
    p.encoding = pointcloud_type_t:new().xyz
    lc:publish('POINTCLOUD', p:encode())
    --]]
    
    local img = image_t:new()
    img.utime = bot_timestamp_now()
    img.size = 7999 + 1
    img.data = {}
    for i=1,img.size do
        img.data[i] = 1
    end
    lc:publish('CAMERA', img:encode())

    print('Sent one!')
end
