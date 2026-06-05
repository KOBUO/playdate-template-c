#!/usr/bin/env python3
"""
BGM（ループ用）を生成して Source/music/bgm.wav に置く（pdc が取り込む）。
矩形波の簡単なアルペジオ・ループ。本番では自前曲に差し替え可。
  python3 tools/gen_bgm.py
"""
import os, math, struct, wave

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT  = os.path.join(ROOT, "Source/music")
RATE = 44100
os.makedirs(OUT, exist_ok=True)

def note(freq, dur, vol=0.22):
    n = int(RATE * dur)
    out = []
    for i in range(n):
        # 矩形波
        s = 1.0 if (i * freq / RATE) % 1.0 < 0.5 else -1.0
        env = min(1.0, i / 200) * max(0.0, 1.0 - i / n)   # アタック＋減衰
        out.append(vol * env * s)
    return out

# 簡単な進行（C-G-Am-F のアルペジオ）
A = 440.0
def f(semi):  # A4基準の半音
    return A * (2 ** (semi / 12.0))
# 各小節のアルペジオ（半音オフセット）
chords = [
    [-9, -5, -2, -5],   # C  (C E G E)
    [-2,  2,  5,  2],   # G
    [ 0,  3,  7,  3],   # Am
    [-4,  0,  3,  0],   # F
]
seq = []
for ch in chords:
    for semi in ch:
        seq += note(f(semi), 0.22)

samples = seq
frames = b"".join(struct.pack("<h", max(-32767, min(32767, int(s * 32767)))) for s in samples)
with wave.open(os.path.join(OUT, "bgm.wav"), "w") as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(RATE)
    w.writeframes(frames)
print("saved", os.path.join(OUT, "bgm.wav"), "sec", round(len(samples) / RATE, 2))
