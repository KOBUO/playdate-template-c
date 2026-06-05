#!/usr/bin/env python3
"""
スプライト アニメ用の imagetable を生成して Source/images/ に置く。
  出力: Source/images/anim-table-32-32.png  (32x32 を横に並べた 4 フレーム歩行アニメ)
Playdate の imagetable 命名規則: <name>-table-<W>-<H>.png
loadBitmapTable("images/anim") で読み込める。
  python3 tools/gen_sprites.py
本番では自前のキャラ画像に差し替え可。
"""
import os
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT  = os.path.join(ROOT, "Source/images")
os.makedirs(OUT, exist_ok=True)

CELL = 32
FRAMES = 4
BLACK = (0, 0, 0, 255)
WHITE = (255, 255, 255, 255)

sheet = Image.new("RGBA", (CELL * FRAMES, CELL), (0, 0, 0, 0))

# 脚の上下オフセット（歩行サイクル）：左脚/右脚を交互に
leg_phase = [(0, 4), (2, 2), (4, 0), (2, 2)]

for i in range(FRAMES):
    cell = Image.new("RGBA", (CELL, CELL), (0, 0, 0, 0))
    d = ImageDraw.Draw(cell)
    bob = [0, 1, 0, 1][i]                       # 上下の弾み
    cx, cy = 16, 13 + bob
    # 胴体（黒い円）
    d.ellipse([cx - 10, cy - 10, cx + 10, cy + 10], fill=BLACK)
    # 目（白）
    d.ellipse([cx + 1, cy - 5, cx + 6, cy], fill=WHITE)
    d.ellipse([cx + 3, cy - 4, cx + 5, cy - 2], fill=BLACK)
    # 脚（黒い棒）左右で上下交互
    lL, lR = leg_phase[i]
    d.rectangle([cx - 7, cy + 8, cx - 3, cy + 8 + 8 - lL], fill=BLACK)
    d.rectangle([cx + 3, cy + 8, cx + 7, cy + 8 + 8 - lR], fill=BLACK)
    sheet.paste(cell, (i * CELL, 0))

path = os.path.join(OUT, f"anim-table-{CELL}-{CELL}.png")
sheet.save(path)
print("saved", path)
