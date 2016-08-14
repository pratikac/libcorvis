require 'lcm'
local pointcloud_t = require 'corvis.pointcloud_t'
local pointcloud_type_t = require 'corvis.pointcloud_type_t'

require 'src/utils/common'
require 'torch'

lc = lcm.lcm.new()

while 1 do
    sys.sleep(0.03)

    local p = pointcloud_t:new()
    p.utime = bot_timestamp_now()
    local r = 10

    for th= -math.pi,math.pi,0.25 do
        for ph=-math.pi,math.pi,0.25 do
            p.points[#p.points+1] = { r*math.cos(ph)*math.cos(th),
                                    -r*math.cos(ph)*math.sin(th),
                                    r*math.sin(ph)}
        end
    end
    p.size = #p.points
    p.encoding = pointcloud_type_t:new().xyz
    
    lc:publish('POINTCLOUD', p:encode())
end
