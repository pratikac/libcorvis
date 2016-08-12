local lcm = require 'lcm'
local pointcloud_t = require 'corvis.pointcloud_t'
local image_t = require 'corvis.image_t'

lc = lcm.lcm.new()

local function on_pointcloud(c, d)
    local msg = pointcloud_t.decode(d)
    print(msg.utime)
end

local function on_image(c, d)
    local msg = image_t.decode(d)
    print(msg.utime)
end

lc:subscribe('CAMERA.*', on_image)
lc:subscribe('POINTCLOUD', on_pointcloud)

while true do
    lc:handle()
end

