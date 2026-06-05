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
    cx, cy, t, L = ox + 32, oy + 99, 6, 15
    plus = [(cx-t,cy-L),(cx+t,cy-L),(cx+t,cy-t),(cx+L,cy-t),(cx+L,cy+t),
            (cx+t,cy+t),(cx+t,cy+L),(cx-t,cy+L),(cx-t,cy+t),(cx-L,cy+t),
            (cx-L,cy-t),(cx-t,cy-t)]
    d.polygon(plus, outline=BLACK)
    # A / B ボタン（右側・横並び・同じ高さ。公式は文字なしの丸）
    for bx in (ox + 84, ox + 106):
        d.ellipse([bx - 9, oy + 80, bx + 9, oy + 98], outline=BLACK, width=2)
    # 四隅のネジ
    for sx, sy in [(ox+11, oy+11), (ox+bw-11, oy+11), (ox+11, oy+bh-11), (ox+bw-11, oy+bh-11)]:
        d.ellipse([sx-3, sy-3, sx+3, sy+3], outline=BLACK, width=1)
        d.ellipse([sx-1, sy-1, sx+1, sy+1], fill=BLACK)
    # スピーカー（本体右上の細いスリット。公式にある小ディテール）
    d.rounded_rectangle([ox + bw - 11, oy + 18, ox + bw - 7, oy + 44], radius=2, outline=BLACK, width=2)
    # クランク（公式準拠）：本体から少し離れた位置に「縦長アーム＋上端の横向き取っ手」のL字。
    # 「黒で塗る → 内側を白で抜く」で1本の綺麗な輪郭にする（継ぎ目の不具合を出さない）。
    arm = [ox + bw + 12, oy + 36, ox + bw + 20, oy + 100]   # アーム（縦長カプセル）
    hnd = [ox + bw + 14, oy + 24, ox + bw + 38, oy + 44]    # 取っ手（横長の箱・上端、右へ突き出す）
    # 1) 黒シルエット
    d.rounded_rectangle(arm, radius=4, fill=BLACK)
    d.rounded_rectangle(hnd, radius=4, fill=BLACK)
    # 2) 内側を白で抜く（白同士を重ねて継ぎ目を消す）
    d.rounded_rectangle([arm[0] + 2, arm[1] + 2, arm[2] - 2, arm[3] - 2], radius=2, fill=255)
    d.rounded_rectangle([hnd[0] + 2, hnd[1] + 2, hnd[2] - 2, hnd[3] - 2], radius=2, fill=255)

draw_device((W - 128) // 2 - 7, 80)   # クランク分すこし左寄せして中央に見せる

im.convert("1").save(OUT)
print("saved", OUT)
