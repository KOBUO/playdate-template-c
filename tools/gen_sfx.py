#!/usr/bin/env python3
"""
効果音(WAV)を生成して Source/sounds/ に置く（pdc が取り込む）。
  python3 tools/gen_sfx.py
本番では自前の音源に差し替え可。BGM は別途 fileplayer で。
"""
import os, math, struct, wave

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT  = os.path.join(ROOT, "Source/sounds")
RATE = 44100

def write_wav(name, samples):
    os.makedirs(OUT, exist_ok=True)
    path = os.path.join(OUT, name)
    with wave.open(path, "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(RATE)
        frames = b"".join(struct.pack("<h", max(-32767, min(32767, int(s * 32767)))) for s in samples)
        w.writeframes(frames)
    print("saved", path)

def tone(freq, ms, vol=0.5):
    n = int(RATE * ms / 1000)
    out = []
    for i in range(n):
        t = i / RATE
        env = min(1.0, i / 80) * max(0.0, 1.0 - i / n)   # 軽いアタック＋減衰
        out.append(vol * env * math.sin(2 * math.pi * freq * t))
    return out

# カーソル移動: 短い高め
write_wav("select.wav", tone(880, 45))
# 決定: 2音上昇
write_wav("confirm.wav", tone(660, 55) + tone(990, 75))
