import "CoreLibs/graphics"
import "CoreLibs/sprites"

local gfx <const> = playdate.graphics

-- crank_loading_sheet_32x32_8frames.png を source/images/ に配置する想定
local crankImage = gfx.image.new("images/crank_loading_sheet_32x32_8frames")
local frameWidth <const> = 32
local frameHeight <const> = 32
local frameCount <const> = 8

local frame = 1
local frameTimer = 0

function drawLoadingCrank(x, y)
    frameTimer += 1

    -- 数値を小さくすると速く回る
    if frameTimer % 3 == 0 then
        frame += 1
        if frame > frameCount then
            frame = 1
        end
    end

    local sx = (frame - 1) * frameWidth
    crankImage:draw(x, y, playdate.geometry.rect.new(sx, 0, frameWidth, frameHeight))
end

function playdate.update()
    gfx.clear(gfx.kColorWhite)

    -- 小さめローディング表示
    drawLoadingCrank(184, 104)

    gfx.drawText("Loading...", 168, 140)
end
