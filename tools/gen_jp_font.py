#!/usr/bin/env python3
"""
英日共用 Playdate フォント(.fnt + グリフPNG)生成スクリプト。

  入力 : assets/fonts-src/PixelMplus12-Bold.ttf (M+ bitmap fonts ライセンス)
  出力 : Source/fonts/main.fnt
         Source/fonts/main-table-12-14.png
  収録 : ASCII(英数記号) + JIS X0208 (ku 1-8: 記号/かな/カナ等, ku 16-47: 第1水準漢字)

英語・日本語の見た目(サイズ/太さ)を揃えるため、両言語ともこの1フォントを使う。
Regular に戻すなら TTF を PixelMplus12-Regular.ttf に変更。第2水準漢字が要るなら
下の ku 範囲に 48-84 を足す。変更後は再実行:
  python3 tools/gen_jp_font.py
"""
import os
from PIL import Image, ImageFont, ImageDraw

HERE   = os.path.dirname(os.path.abspath(__file__))
ROOT   = os.path.dirname(HERE)
TTF    = os.path.join(ROOT, "assets/fonts-src/PixelMplus12-Bold.ttf")
OUTDIR = os.path.join(ROOT, "Source/fonts")
BASE   = "main"

SIZE   = 12          # PixelMplus12 は 12px 設計
CELL_W = 12          # 全角の最大送り幅
CELL_H = 14          # ascent(11) + descent(3)
COLS   = 64          # PNGテーブルの列数
THRESH = 128         # アンチエイリアスの2値化しきい値

font = ImageFont.truetype(TTF, SIZE)

# --- 収録文字を構築（重複除去・順序保持。スペースは別扱い）---
seen, chars = set(), []
def add(c):
    if c and c != ' ' and not c.isspace() and c not in seen:
        seen.add(c); chars.append(c)

for cp in range(0x21, 0x7f):          # ASCII 表示可能文字
    add(chr(cp))

for ku in list(range(1, 9)) + list(range(16, 48)):   # JIS: 記号/かな/カナ + 第1水準漢字
    for ten in range(1, 95):
        try:
            ch = bytes([0xA0 + ku, 0xA0 + ten]).decode("euc_jp")
        except Exception:
            continue
        add(ch)

# セル順: [0]=space(空白), [1..]=chars
glyphs = ["space"] + chars
n = len(glyphs)
rows = (n + COLS - 1) // COLS

img = Image.new("RGBA", (COLS * CELL_W, rows * CELL_H), (0, 0, 0, 0))

def render(ch):
    """1グリフを 黒(不透明)/透過 の RGBA セルに描く"""
    l = Image.new("L", (CELL_W, CELL_H), 0)
    ImageDraw.Draw(l).text((0, 0), ch, font=font, fill=255)
    cell = Image.new("RGBA", (CELL_W, CELL_H), (0, 0, 0, 0))
    src, dst = l.load(), cell.load()
    for y in range(CELL_H):
        for x in range(CELL_W):
            if src[x, y] >= THRESH:
                dst[x, y] = (0, 0, 0, 255)
    return cell

for i, g in enumerate(glyphs):
    if g == "space":
        continue
    cx, cy = (i % COLS) * CELL_W, (i // COLS) * CELL_H
    img.paste(render(g), (cx, cy))

os.makedirs(OUTDIR, exist_ok=True)
png = os.path.join(OUTDIR, f"{BASE}-table-{CELL_W}-{CELL_H}.png")
img.save(png)

def advance(ch):
    return max(1, round(font.getlength(ch)))

lines = ["tracking=0", f"space\t{advance(' ')}"]
lines += [f"{ch}\t{advance(ch)}" for ch in chars]
with open(os.path.join(OUTDIR, f"{BASE}.fnt"), "w", encoding="utf-8") as fp:
    fp.write("\n".join(lines) + "\n")

print(f"glyphs={n} image={img.size} -> {png}")
