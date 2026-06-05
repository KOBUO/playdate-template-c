#!/usr/bin/env python3
"""
タイトル画面のプレースホルダ画像を生成する（Playdate本体のライン画 + テンプレート名）。
本番アートができたら Source/images/title.png を差し替えればよい。
  python3 tools/gen_title_placeholder.py

ポイント: ピクセルフォントは 12px で描いてから純白黒に2値化 → NEAREST拡大でクッキリ。
「Press A」はコード側(scene_title)で点滅描画するので画像には入れない。
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

# 二重枠
d.rectangle([6, 6, W - 7, H - 7], outline=BLACK)
d.rectangle([10, 10, W - 11, H - 11], outline=BLACK)

# --- テキスト（拡大前に2値化してクッキリ）---
font = ImageFont.truetype(TTF, 12)
def stamp(text, scale, cy):
    w = int(d.textlength(text, font=font))
    t = Image.new("L", (w, 12), 255)
    ImageDraw.Draw(t).text((0, 0), text, font=font, fill=BLACK)
    t = t.point(lambda p: 0 if p < 128 else 255)
    t = t.resize((w * scale, 12 * scale), Image.NEAREST)
    im.paste(t, ((W - w * scale) // 2, cy))

stamp("GAME TEMPLATE", 3, 24)     # テンプレートと分かるタイトル
stamp("Playdate / C", 1, 64)

# --- Playdate本体のライン画 ---
def draw_device(ox, oy):
    bw, bh = 128, 132
    d.rounded_rectangle([ox, oy, ox + bw, oy + bh], radius=12, outline=BLACK, width=3)
    # 画面（横長）
    d.rectangle([ox + 12, oy + 12, ox + bw - 12, oy + 58], outline=BLACK, width=2)
    # 十字キー（左下）
    cx, cy, t, L = ox + 34, oy + 96, 6, 15
    plus = [(cx-t,cy-L),(cx+t,cy-L),(cx+t,cy-t),(cx+L,cy-t),(cx+L,cy+t),
            (cx+t,cy+t),(cx+t,cy+L),(cx-t,cy+L),(cx-t,cy+t),(cx-L,cy+t),
            (cx-L,cy-t),(cx-t,cy-t)]
    d.polygon(plus, outline=BLACK)
    # A / B ボタン（右下）
    for bx, by, ch in [(ox+86, oy+102, "B"), (ox+106, oy+88, "A")]:
        d.ellipse([bx-9, by-9, bx+9, by+9], outline=BLACK, width=2)
        lw = int(d.textlength(ch, font=font))
        d.text((bx - lw // 2, by - 6), ch, font=font, fill=BLACK)
    # クランク：バー(腕/シャフト)と 掴むところ(握り) は別部品。
    # それぞれ本体と同じ線画(白塗り+黒フチ)で描き、重ねて継ぎ目(関節)を見せる。
    # 上ヒンジ → バー(細め) → 握り(太め) の順。
    d.line([ox + bw - 1, oy + 27, ox + bw + 6, oy + 27], fill=BLACK, width=3)        # 上ヒンジ
    # バー（腕）：細く短い角丸
    d.rounded_rectangle([ox + bw + 5, oy + 22, ox + bw + 13, oy + 52],
                        radius=3, fill=255, outline=BLACK, width=2)
    # 握り（掴むところ）：太く長い角丸。バー下端に少し重ねて接合
    d.rounded_rectangle([ox + bw + 8, oy + 48, ox + bw + 22, oy + 98],
                        radius=6, fill=255, outline=BLACK, width=2)

draw_device((W - 128) // 2 - 7, 80)   # クランク分すこし左寄せして中央に見せる

im.convert("1").save(OUT)
print("saved", OUT)
