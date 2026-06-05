import "CoreLibs/graphics"

local gfx <const> = playdate.graphics

-- Playdate実機向けには 1bit版を source/images/ に配置する想定
-- playdate_pixel_loading_sheet_48x48_8frames_1bit.png
local loadingImage = gfx.image.new("images/playdate_pixel_loading_sheet_48x48_8frames_1bit")

local frameWidth <const> = 48
local frameHeight <const> = 48
local frameCount <const> = 8

local frame = 1
local frameTimer = 0

function drawPlaydatePixelLoading(x, y)
    frameTimer += 1

    if frameTimer % 4 == 0 then
        frame += 1
        if frame > frameCount then
            frame = 1
        end
    end

    local sx = (frame - 1) * frameWidth
    loadingImage:draw(x, y, playdate.geometry.rect.new(sx, 0, frameWidth, frameHeight))
end

function playdate.update()
    gfx.clear(gfx.kColorWhite)

    drawPlaydatePixelLoading(176, 86)
    gfx.drawText("Loading...", 168, 142)
end
