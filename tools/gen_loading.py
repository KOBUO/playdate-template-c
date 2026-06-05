#!/usr/bin/env python3
"""
ローディング用 imagetable を生成。
入力: assets/art/pixel_loading/playdate_pixel_loading_sheet_40x28_8frames_1bit.png（320x28, 8コマ）
出力: Source/images/loading-table-40-28.png（Playdate imagetable 命名規則）
孤立した黒点があれば除去。
  python3 tools/gen_loading.py
"""
import os
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC  = os.path.join(ROOT, "assets/art/pixel_loading/playdate_pixel_loading_sheet_40x28_8frames_1bit.png")
OUT  = os.path.join(ROOT, "Source/images/loading-table-40-28.png")

im = Image.open(SRC).convert("L")
px = im.load()
w, h = im.size
removed = 0
for y in range(h):
    for x in range(w):
        if px[x, y] < 128:
            neigh = 0
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    if dx == 0 and dy == 0:
                        continue
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < w and 0 <= ny < h and px[nx, ny] < 128:
                        neigh += 1
            if neigh == 0:
                px[x, y] = 255
                removed += 1
im.point(lambda p: 0 if p < 128 else 255).convert("1").save(OUT)
print("saved", OUT, "removed", removed, "speckles")
