# Playdate Game Template (C)

Playdate（C）でゲームを作るためのテンプレート兼サンプル集。
**Playdate(C) 開発ガイド本のサンプルコード**として、主要トピックを一通り盛り込んでいる。

> Playdate is a trademark of Panic Inc. 本プロジェクトは非公式で Panic とは無関係です。

## ビルド & 実行（シミュレータ）

```bash
export PLAYDATE_SDK_PATH="$HOME/Developer/PlaydateSDK"   # 未設定なら
cmake -B build -S .        # 初回のみ（ファイルを追加した時も）
cmake --build build        # 毎回
open -a "$PLAYDATE_SDK_PATH/bin/Playdate Simulator.app" myfirstgame.pdx
```

実機ビルドは ARM toolchain が必要（`brew install --cask gcc-arm-embedded`）。

## 画面の流れ

```
Splash(オープニング) → Title → Stage Select → Loading → Stage(ミニゲーム) → クリアで★
```

各ステージは「その操作に適したシンプルなゲーム」：

| ステージ | 操作 | 内容 |
|---|---|---|
| Move    | 十字キー | 歩行アニメでゴールへ |
| Crank   | クランク | 針を的に合わせて A |
| Tilt    | 加速度センサ | 傾けてボールをゴールへ |
| Maze    | 十字キー | タイルマップ迷路を出口へ |

Settings / Credits は **⊙(Menu)ボタン**から。各ステージ開始時に会話ウィンドウで操作説明が出る。

## 構成

```
src/
├── main.c            eventHandler + メインループ + ライフサイクル(自動セーブ)
├── globals.h/.c      pd / フォント / 画面定数 / 描画・画像・メモリヘルパー
├── input.h/.c        ボタン / クランク / 加速度の入力ヘルパー
├── scene.h/.c        シーン管理（スタック: switch/push/pop、フェード遷移）
├── scenes.h          各シーン宣言 + LoadRequest
├── sysmenu.h/.c      ⊙システムメニュー項目の再利用ヘルパー
├── i18n.h/.c         多言語(EN/JA) + 言語別フォント
├── save.h/.c         セーブ/ロード（設定 + クリア状況）
├── audio.h/.c        SE(sampleplayer) + BGM(fileplayer)、サウンド設定連動
├── dialogue.h        会話スクリプト定義（名前 / 選択肢対応）
├── tween.h/.c        補間・イージング
└── scenes/
    ├── scene_splash.c        オープニング(コード演出、Aスキップ)
    ├── scene_title.c         タイトル(背景画像)
    ├── scene_stage_select.c  ステージ選択(トゥイーンするカーソル)
    ├── scene_loading.c       ローディング(擬似待ち + アニメ)
    ├── scene_stage_move.c    ステージ:移動(会話の名前+選択肢デモ)
    ├── scene_stage_crank.c   ステージ:クランク
    ├── scene_stage_tilt.c    ステージ:傾き
    ├── scene_stage_tilemap.c ステージ:タイルマップ迷路
    ├── scene_settings.c      設定(Sound/Language/Credits)
    ├── scene_credits.c       クレジット
    └── scene_dialogue.c      会話ウィンドウ(UTF-8タイプライター/折返し/名前/選択肢)
```

## シーン管理

スタック制。トップのシーンだけが update され、`draw_below=1` で下のシーンも描画（オーバーレイ）。

| 関数 | 呼ばれる時 |
|------|----|
| `enter(self, arg)` | 生成時。`arg` は呼び出し側の引数 |
| `update(self, dt)` | 毎フレーム（トップのみ） |
| `draw(self)` | 毎フレーム |
| `resume(self, result)` | 上に積んだ画面が pop した時（`result` を受け取る） |
| `leave(self)` | 破棄時 |
| `build_menu(self)` | ⊙メニュー項目登録（トップになるたび） |

操作（次フレーム頭で安全に適用）:
`scene_switch(id, arg, transition)` / `scene_push(id, arg)` / `scene_pop(result)`。
`arg`/`result` は `INT_TO_ARG()` / `ARG_TO_INT()` で整数を詰められる。

新しい画面の追加: `scene.h` の enum + `scenes/xxx.c` + `scenes.h` 宣言 + `main.c` で register + `cmake -B build -S .` 再実行。

## 各システム

- **i18n**: 文字列は `tr(STR_*)` で取得。言語追加は `Language` と `TABLE`/`FONT_PATH` に1列。描画は UTF-8。
- **フォント**: 英日共用の PixelMplus12（`tools/gen_jp_font.py` で生成、ASCII+かな+第1水準漢字）。
- **セーブ**: `save_store/load`。設定変更時 + 終了/スリープ(`kEventTerminate/Lock`)で自動保存。クリア状況はビットマスク。
- **オーディオ**: SE は `audio_play(SFX_*)`、BGM はループ。`g_sound_on` で消音（設定で切替→`audio_refresh()`）。
- **⊙メニュー**: `build_menu` で `sysmenu_add_settings/credits/quit_to_title` を組み立て（OS制約で英語固定・最大3項目）。
- **会話**: `DialogueScript{lines,count,name,choices,choice_count}` を `scene_push(SCENE_DIALOGUE, &sc)`。選択結果は pop の result で返る。
- **トゥイーン**: `tween_approach/lerp/ease_*`。
- **ローディング**: 本物の sleep はブロックするため、タイマーで擬似待ち（アニメを回しながら）。

## ツール（アセット生成）

```bash
python3 tools/gen_jp_font.py    # 日本語フォント
python3 tools/gen_title_placeholder.py  # タイトル画像（assets/art/playdate-device.png から）
python3 tools/gen_launcher.py   # ランチャー card / launchImage
python3 tools/gen_sprites.py    # 歩行アニメ imagetable
python3 tools/gen_loading.py    # ローディングアニメ imagetable
python3 tools/gen_sfx.py        # 効果音
python3 tools/gen_bgm.py        # BGM
```

## ライセンス / クレジット

- ソースコードは **MIT**（`LICENSE`）。
- 同梱フォント **PixelMplus**（M+ BITMAP FONTS License）— `NOTICE` 参照。
- タイトル/ローディング等の画像はオリジナル（`assets/art/`）。効果音/BGMは自作生成。
- 「Playdate」は Panic Inc. の商標。本プロジェクトは非公式。SDKは同梱していない。
