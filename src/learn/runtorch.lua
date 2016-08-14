require 'torch'
require 'image'
require 'qtwidget'
local dbg = require 'debugger'

local lcm = require 'lcm'
local image_t = require 'corvis.image_t'

local lc = lcm.lcm.new()

local function handler(c, d)
    local msg = image_t.decode(d)

    if not win then
        win = qtwidget.newwindow(msg.width, msg.height)
    end
    local a = torch.ByteTensor(msg.data)
    local tmp = image.decompressJPG(a)
    image.display{image=tmp, win=win}
end

local sub = lc:subscribe('CAMERA.*', handler)

while true do
    lc:handle()
end
