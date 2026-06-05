#!/usr/bin/env python3
"""
タイトル画像（テキスト主体・クリーン）を生成して Source/images/title.png に置く。
「Press A」はコード側(scene_title)で点滅描画するので画像には入れない。
本番アート（デバイス線画など）を使いたい場合は、この PNG を自前画像に差し替えるだけ。
  python3 tools/gen_title_placeholder.py
"""
import os
from PIL import Image, ImageFont, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TTF  = os.path.join(ROOT, "assets/fonts-src/PixelMplus12-Bold.ttf")
OUT  = os.path.join(ROOT, "Source/images/title.png")
BLACK = 0
W, H = 400, 240

im = Image.new("L", (W, H), 255)
d  = ImageDraw.Draw(im)
font = ImageFont.truetype(TTF, 12)

def stamp(text, scale, cy):
    """12px で描いてから2値化→NEAREST拡大（クッキリ）。描画幅を返す。"""
    w = int(d.textlength(text, font=font))
    t = Image.new("L", (w, 12), 255)
    ImageDraw.Draw(t).text((0, 0), text, font=font, fill=BLACK)
    t = t.point(lambda p: 0 if p < 128 else 255).resize((w * scale, 12 * scale), Image.NEAREST)
    im.paste(t, ((W - w * scale) // 2, cy))
    return w * scale

# 大タイトル
tw = stamp("GAME TEMPLATE", 4, 78)
# 下線（タイトル幅に合わせる）
ux0 = (W - tw) // 2
d.rectangle([ux0, 78 + 48 + 8, ux0 + tw, 78 + 48 + 11], fill=BLACK)
# サブタイトル
stamp("Playdate / C", 2, 150)

im.convert("1").save(OUT)
print("saved", OUT)
