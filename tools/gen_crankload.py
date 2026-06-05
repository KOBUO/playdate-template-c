#!/usr/bin/env python3
"""
クランク・ロード用 imagetable を生成。
入力: assets/art/crank_loading/crank_loading_sheet_32x32_8frames.png（256x32, 8コマ）
出力: Source/images/crankload-table-32-32.png（Playdate imagetable 命名規則）
孤立した黒点（生成ノイズ）を除去してクッキリさせる。
  python3 tools/gen_crankload.py
"""
import os
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC  = os.path.join(ROOT, "assets/art/crank_loading/crank_loading_sheet_32x32_8frames.png")
OUT  = os.path.join(ROOT, "Source/images/crankload-table-32-32.png")

im = Image.open(SRC).convert("L")
px = im.load()
w, h = im.size

# 孤立黒点（周囲8近傍がすべて白）を除去
to_white = []
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
                to_white.append((x, y))
for x, y in to_white:
    px[x, y] = 255

im.point(lambda p: 0 if p < 128 else 255).convert("1").save(OUT)
print("saved", OUT, "removed", len(to_white), "speckles")
