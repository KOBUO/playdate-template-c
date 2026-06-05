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

stamp("GAME TEMPLATE", 3, 16)     # テンプレートと分かるタイトル
stamp("Playdate / C", 1, 50)

# --- Playdate本体のライン画（実機写真の配置に準拠・正面向き）---
def draw_device(ox, oy):
    bw, bh = 126, 132
    # 本体
    d.rounded_rectangle([ox, oy, ox + bw, oy + bh], radius=14, outline=BLACK, width=3)
    # 画面（左上寄り・横長 5:3）
    d.rectangle([ox + 9, oy + 9, ox + 92, oy + 58], outline=BLACK, width=2)
    # スピーカー（画面の右・縦スリット）
    d.rounded_rectangle([ox + 100, oy + 20, ox + 104, oy + 50], radius=2, outline=BLACK, width=1)
    # クランク（右上：横→下の L字。黒塗り→内側白抜きで綺麗な輪郭）
    horz = [ox + bw - 8, oy + 8,  ox + bw + 24, oy + 20]   # 上の横バー
    vert = [ox + bw + 14, oy + 12, ox + bw + 24, oy + 74]  # 右の縦バー（先に取っ手）
    d.rounded_rectangle(horz, radius=5, fill=BLACK)
    d.rounded_rectangle(vert, radius=5, fill=BLACK)
    d.rounded_rectangle([horz[0]+2, horz[1]+2, horz[2]-2, horz[3]-2], radius=3, fill=255)
    d.rounded_rectangle([vert[0]+2, vert[1]+2, vert[2]-2, vert[3]-2], radius=3, fill=255)
    # 十字キー（左下・大きめ）
    cx, cy, t, L = ox + 34, oy + 98, 7, 18
    plus = [(cx-t,cy-L),(cx+t,cy-L),(cx+t,cy-t),(cx+L,cy-t),(cx+L,cy+t),
            (cx+t,cy+t),(cx+t,cy+L),(cx-t,cy+L),(cx-t,cy+t),(cx-L,cy+t),
            (cx-L,cy-t),(cx-t,cy-t)]
    d.polygon(plus, outline=BLACK)
    # B / A ボタン（下・中央右、文字あり。写真どおり B が左・A が右）
    for bx, ch in [(ox + 76, "B"), (ox + 102, "A")]:
        by = oy + 98
        d.ellipse([bx-11, by-11, bx+11, by+11], outline=BLACK, width=2)
        lw = int(d.textlength(ch, font=font))
        d.text((bx - lw//2, by - 6), ch, font=font, fill=BLACK)
    # ネジ（右上・左下・右下）
    for sx, sy in [(ox+bw-12, oy+12), (ox+12, oy+bh-12), (ox+bw-12, oy+bh-12)]:
        d.ellipse([sx-3, sy-3, sx+3, sy+3], outline=BLACK, width=1)
        d.ellipse([sx-1, sy-1, sx+1, sy+1], fill=BLACK)

draw_device((W - 126) // 2 - 12, 74)  # クランク分すこし左へ寄せて中央に見せる

im.convert("1").save(OUT)
print("saved", OUT)
