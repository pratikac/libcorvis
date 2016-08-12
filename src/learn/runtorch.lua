require 'torch'

local lcm = require 'lcm'
local image_t = require 'corvis.image_t'

local lc = lcm.lcm.new()

local function handler(c, d)
    local msg = image_t.decode(d)
    print(msg.utime)
end

local sub = lc:subscribe('CAMERA.*', handler)

while true do
    lc:handle()
end