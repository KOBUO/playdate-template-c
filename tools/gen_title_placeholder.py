#!/usr/bin/env python3
"""
タイトル画像を生成して Source/images/title.png に置く。
デバイス線画（assets/art/playdate-device.png・オリジナル画像）を1bit化して配置し、
上に "PLAYDATE TEMPLATE" を載せ、画像と少し重ねる。「Press A」はコード側で点滅描画。
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

# 黒い線だけを重ねて貼る（白背景で下を消さない＝重なりが見える）
def paste_black(layer, x, y):
    mask = layer.point(lambda p: 255 if p < 128 else 0)   # 黒→不透明
    im.paste(0, (x, y, x + layer.width, y + layer.height), mask)

def render_text(text, scale):
    w = int(d.textlength(text, font=font))
    t = Image.new("L", (w, 12), 255)
    ImageDraw.Draw(t).text((0, 0), text, font=font, fill=BLACK)
    return t.point(lambda p: 0 if p < 128 else 255).resize((w * scale, 12 * scale), Image.NEAREST)

# デバイスの「画面」矩形の中心Yを検出する（外周から届かない＝囲まれた最大の白領域＝画面）。
def screen_center_y(img):
    from collections import deque
    g = img.convert("L"); px = g.load(); w, h = g.size
    ext = [[False] * w for _ in range(h)]
    dq = deque()
    for x in range(w):
        for y in (0, h - 1):
            if px[x, y] >= 128 and not ext[y][x]: ext[y][x] = True; dq.append((x, y))
    for y in range(h):
        for x in (0, w - 1):
            if px[x, y] >= 128 and not ext[y][x]: ext[y][x] = True; dq.append((x, y))
    while dq:
        x, y = dq.popleft()
        for dx2, dy2 in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nx, ny = x + dx2, y + dy2
            if 0 <= nx < w and 0 <= ny < h and px[nx, ny] >= 128 and not ext[ny][nx]:
                ext[ny][nx] = True; dq.append((nx, ny))
    seen = [[False] * w for _ in range(h)]
    best, best_sz = None, 0
    for y0 in range(h):
        for x0 in range(w):
            if px[x0, y0] >= 128 and not ext[y0][x0] and not seen[y0][x0]:
                comp = deque([(x0, y0)]); seen[y0][x0] = True
                minx = maxx = x0; miny = maxy = y0; sz = 0
                while comp:
                    x, y = comp.popleft(); sz += 1
                    minx = min(minx, x); maxx = max(maxx, x)
                    miny = min(miny, y); maxy = max(maxy, y)
                    for dx2, dy2 in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                        nx, ny = x + dx2, y + dy2
                        if 0 <= nx < w and 0 <= ny < h and px[nx, ny] >= 128 and not ext[ny][nx] and not seen[ny][nx]:
                            seen[ny][nx] = True; comp.append((nx, ny))
                # 矩形に近い（bbox充填率が高い）大きな領域＝画面。本体内側の不定形は除外。
                bbox_area = (maxx - minx + 1) * (maxy - miny + 1)
                fill = sz / bbox_area if bbox_area else 0
                if fill >= 0.8 and sz > best_sz:
                    best_sz = sz; best = (miny + maxy) // 2
    return best if best is not None else int(h * 0.32)

dev_l = dev.convert("L")
scy = screen_center_y(dev_l)               # 画面中心Y（デバイス座標）

# タイトル
title = "PLAYDATE TEMPLATE"
tw = int(d.textlength(title, font=font))
tscale = 3
while tw * tscale > W - 20 and tscale > 1:
    tscale -= 1
tl = render_text(title, tscale)

# タイトル中心を画面中心に合わせる。画面中心が来てほしいキャンバスY = TARGET。
TARGET = 78
DDY = TARGET - scy                         # → 画像を上げ、画面中心がTARGETに来る
if DDY < 4:
    DDY = 4
dx = (W - dev_l.width) // 2 + int(dev_l.width * 0.06)
paste_black(dev_l, dx, DDY)
paste_black(tl, (W - tl.width) // 2, TARGET - tl.height // 2)

im.convert("1").save(OUT)
print("saved", OUT, "device", dev.size, "title_scale", tscale)
