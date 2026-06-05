# Playdate ゲームテンプレート (C)

シーン管理（タイトル / メニュー / 本編…）を備えた、Playdate ゲームの C 製テンプレート。
新しいゲームを作るときの雛形として使う。

## ビルド & 実行（シミュレータ）

```bash
export PLAYDATE_SDK_PATH="$HOME/Developer/PlaydateSDK"   # 未設定なら

# 初回のみ
cmake -B build -S .

# 毎回（コード編集後）
cmake --build build
open -a "$PLAYDATE_SDK_PATH/bin/Playdate Simulator.app" myfirstgame.pdx
```

## 実機ビルド（要 ARM toolchain）

```bash
brew install --cask gcc-arm-embedded   # 一度だけ
cmake -B build-device -DCMAKE_TOOLCHAIN_FILE="$PLAYDATE_SDK_PATH/C_API/buildsupport/arm.cmake"
cmake --build build-device
```

## 構成

```
src/
├── main.c            エントリ(eventHandler)＋メインループ
├── globals.h/.c      pd / フォント / 画面定数 / 描画・メモリヘルパー
├── input.h/.c        入力ヘルパー(btn_pressed/down/released, crank)
├── scene.h/.c        シーン管理(スタック+遷移演出)
├── scenes.h          各シーンの宣言
└── scenes/
    ├── scene_title.c タイトル
    ├── scene_menu.c  メニュー(難易度を本編に渡す例)
    ├── scene_game.c    本編(ポーズを重ねる例 / ⊙メニュー登録例)
    ├── scene_pause.c   ポーズ(オーバーレイ。結果を本編に返す例)
    ├── scene_settings.c 設定(Sound / Language 切替)
    ├── scene_credits.c クレジット
    ├── i18n.h/.c       多言語化(文字列テーブル + 言語別フォント)
    └── ...
```

## シーンの仕組み

シーンは「スタック」で管理する。トップのシーンだけが `update`（入力処理）され、描画はフラグ次第で下から重ねられる。

各シーンが実装する関数（不要なら NULL）:

| 関数 | 呼ばれるタイミング |
|------|----|
| `enter(self, arg)` | 生成時に1回。`arg` は呼び出し側が渡した引数 |
| `update(self, dt)` | 毎フレーム（トップのみ）。ロジック・入力 |
| `draw(self)` | 毎フレーム。描画 |
| `resume(self, result)` | 上に積んだ画面が pop し、再びトップに戻った時 |
| `leave(self)` | 破棄時に1回 |

シーン固有のデータは `self->state` に持つ（`enter` で `mem_alloc`、`leave` で `mem_free`）。
`draw_below = 1` にすると、自分の下のシーンも描画される（ポーズ・会話などのオーバーレイ用）。

## 画面操作 API（どこからでも呼べる。次フレーム頭で安全に適用）

```c
scene_switch(SCENE_GAME, arg, TRANSITION_FADE); // スタックを空にして1枚に差し替え（通常の遷移）
scene_push(SCENE_PAUSE, NULL);                  // 今の画面の上に重ねる（オーバーレイ）
scene_pop(result);                              // トップを外して下に戻る。result は下の resume へ
```

遷移演出は `TRANSITION_NONE`（即時）/ `TRANSITION_FADE`（黒フェード）。

### データの受け渡し

`arg` / `result` は `void*`。小さな整数なら詰めるマクロを使う:

```c
scene_switch(SCENE_GAME, INT_TO_ARG(difficulty), TRANSITION_FADE);
int difficulty = ARG_TO_INT(arg);   // enter / resume 側で取り出す
```

構造体など大きいデータを渡す場合は、寿命に注意（適用は次フレームなので、スタック上の一時変数を指すポインタは渡さない。static か mem_alloc したものを渡す）。

## ⊙ システムメニュー連携

⊙(Menu)ボタンで開くのは **OS のメニュー**で、ゲームは項目を最大3つまで追加できる（OS の項目を置き換えるのではなく "追加"）。

各シーンは `build_menu(self)` を実装すると、**そのシーンがトップになるたび**にマネージャが
`removeAllMenuItems()` してから呼び出す → シーンごとに ⊙ メニューが自動で付け替わる。

```c
static void on_settings(void* ud) { scene_push(SCENE_SETTINGS, NULL); } // 自作の設定画面へ
static void build_menu(Scene* self) {
    pd->system->addMenuItem("Settings", on_settings, NULL);
    pd->system->addMenuItem("Quit to Title", on_quit, NULL);
}
// 生成関数で .build_menu = build_menu を指定
```

項目の種類: `addMenuItem`(ボタン) / `addCheckmarkMenuItem`(ON/OFF) / `addOptionsMenuItem`(複数択)。
コールバックはメニューを閉じた時に呼ばれる。中から `scene_push`/`scene_switch` を呼べば自作シーンへ遷移できる。

使い分け：表向きのタイトル/モード選択は**自作シーン**、プレイ中のオプション(設定/リスタート/タイトルへ)は**⊙メニュー**。

> ⚠ **⊙メニューの項目名は日本語にできない**：OSが内蔵フォント(英字のみ)で描画するため、
> かな・漢字は豆腐(□)になる。項目名は言語に関わらず英語(またはローマ字)固定にする。

## 画像

```c
LCDBitmap* bg = img_load("images/title");  // Source/images/title.png → 拡張子なしで指定
img_draw(bg, 0, 0);
img_free(bg);                              // leave で解放
```

タイトル画面は背景画像方式（`Source/images/title.png`、いまはプレースホルダ）。本番アートに差し替えるだけ。
画面ごとに別画像にしたい場合や言語別タイトルにしたい場合は、ファイルを分けて `img_load` のパスを変える。

### 文字レイアウトの整列

`draw_text`(左寄せ) / `draw_text_right`(右端合わせ) / `draw_text_centered`(中央) / `text_width` を使う。
設定画面はコロン位置を揃える例（ラベルを右寄せ＝コロン揃え、値は左寄せで開始位置統一）。

## 多言語化（i18n）

UI文字列はソースに直書きせず、`tr(STR_xxx)` で取得する。描画は UTF-8（`kUTF8Encoding`）なので日本語もそのまま出せる。

**文字列を足す:** `i18n.h` の `StringID` に ID を追加 → `i18n.c` の各言語テーブルに訳を書く。
**言語を足す:** `i18n.h` の `Language` に追加 → `i18n.c` の `FONT_PATH` と `TABLE` にその言語の列を足すだけ。
（未翻訳の項目は自動で英語にフォールバックする）

```c
draw_text_centered(tr(STR_TITLE), 80);   // 現在の言語で描画
i18n_set_language(LANG_JA);               // 言語切替（フォントも切替）
```

### フォント（英日共用・生成済み）

SDK標準フォントは英字＋かなのみ（漢字なし）。また英語(Asheville 14 Bold)と日本語で
サイズ・太さがバラつくため、**英語・日本語とも同じピクセルフォントに統一**して生成・同梱している:

- 元データ: `assets/fonts-src/PixelMplus12-Bold.ttf`（M+ bitmap fonts ライセンス＝商用配布OK）
- 生成物: `Source/fonts/main.fnt` + `main-table-12-14.png`（ASCII＋かな＋JIS第1水準漢字 約3500字）
- `pdc` がビルド時に `.pft` にコンパイル。`i18n.c` の `FONT_PATH` で全言語がこれを参照

**再生成**（フォント/太さ/収録文字を変えたいとき）:

```bash
python3 tools/gen_jp_font.py    # 要 Pillow
```

- Regular にするなら `tools/gen_jp_font.py` の `TTF` を `PixelMplus12-Regular.ttf` に
- 第2水準漢字が要るなら同スクリプトの `ku` 範囲に `48-84` を追加
- 言語ごとに別フォントにしたいなら `i18n.c` の `FONT_PATH[]` を分ける
- フォントが見つからない場合は英語フォントで代替され、日本語は豆腐□になる

## 新しい画面（シーン）の追加手順

1. `src/scene.h` の `SceneID` enum に ID を1つ足す（例: `SCENE_RESULT`）
2. `src/scenes/scene_result.c` を作る（既存シーンをコピーすると早い）。
   `enter / update / draw / leave` を実装し、末尾に生成関数 `Scene scene_result(void)` を置く
3. `src/scenes.h` に `Scene scene_result(void);` を1行追加
4. `src/main.c` の init に `scene_register(SCENE_RESULT, scene_result);` を追加（関数ポインタを渡す。`()` は付けない）
5. **ファイルを新規追加したので一度** `cmake -B build -S .` **を再実行**（CMake が GLOB で .c を集めるため）

## 入力 API（input.h）

- `btn_pressed(kButtonA)` … 押した瞬間（メニュー決定などに）
- `btn_down(kButtonLeft)` … 押しっぱなし（移動などに）
- `btn_released(kButtonB)` … 離した瞬間
- `crank_angle()` / `crank_change()` / `crank_docked()` … クランク

ボタン: `kButtonA / kButtonB / kButtonUp / kButtonDown / kButtonLeft / kButtonRight`

## ライセンス / クレジット

- 本リポジトリのソースコードは **MIT License**（`LICENSE`）。
- 同梱フォント **PixelMplus**（M+ BITMAP FONTS License）— `assets/fonts-src/PixelMplus-LICENSE_E.txt`、詳細は `NOTICE`。
- 効果音は自作（`tools/gen_sfx.py` 生成、MIT）。
- **「Playdate」は Panic Inc. の商標**です。本プロジェクトは**非公式**で、Panic とは無関係です。Playdate SDK は同梱していません（各自インストール）。
- タイトル/ランチャー画像は本テンプレート用の簡易プレースホルダ（`tools/gen_title_placeholder.py` / `gen_launcher.py` で生成）。配布する自作ゲームでは独自アートに差し替えてください。
