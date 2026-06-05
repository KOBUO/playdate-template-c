#!/usr/bin/env python3
"""
タイトル画像を生成して Source/images/title.png に置く。
デバイス線画（assets/art/playdate-device.png・オリジナル画像）を1bit化して配置し、
上に "GAME TEMPLATE" を載せる。「Press A」はコード側(scene_title)で点滅描画。
本番アートに差し替えるなら assets/art/playdate-device.png を置き換えて再実行。
  python3 tools/gen_title_placeholder.py
"""
import os
from PIL import Image, ImageFont, ImageDraw, ImageOps, ImageFilter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TTF  = os.path.join(ROOT, "assets/fonts-src/PixelMplus12-Bold.ttf")
DEV  = os.path.join(ROOT, "assets/art/playdate-device.png")
OUT  = os.path.join(ROOT, "Source/images/title.png")
BLACK = 0
W, H = 400, 240

# --- デバイス線画を 1bit 化 ---
ref = Image.open(DEV).convert("L")
# 黒線だけ抽出（白背景はそのまま白）→ 縮小で途切れないよう少し太らせる
lines = ref.point(lambda p: 0 if p < 120 else 255).filter(ImageFilter.MinFilter(3))
bbox = ImageOps.invert(lines).getbbox()
crop = lines.crop(bbox)
# 表示枠に収める → 2値化
max_w, max_h = 300, 140
s = min(max_w / crop.width, max_h / crop.height)
dev = crop.resize((int(crop.width * s), int(crop.height * s)), Image.LANCZOS)
dev = dev.point(lambda p: 0 if p < 175 else 255).convert("1")

# --- 合成 ---
im = Image.new("L", (W, H), 255)
d  = ImageDraw.Draw(im)
font = ImageFont.truetype(TTF, 12)

def stamp(text, scale, cy):
    w = int(d.textlength(text, font=font))
    t = Image.new("L", (w, 12), 255)
    ImageDraw.Draw(t).text((0, 0), text, font=font, fill=BLACK)
    t = t.point(lambda p: 0 if p < 128 else 255).resize((w * scale, 12 * scale), Image.NEAREST)
    im.paste(t, ((W - w * scale) // 2, cy))

stamp("GAME TEMPLATE", 3, 16)
im.paste(dev.convert("L"), ((W - dev.width) // 2, 64 + (140 - dev.height) // 2))

im.convert("1").save(OUT)
print("saved", OUT, "device", dev.size)
