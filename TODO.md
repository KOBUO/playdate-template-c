# TODO

このリポジトリの最終目的: **Playdate(C) 開発ガイド本（EPUB/KDP）のサンプルコード**。
テンプレートに各トピックのミニデモを「Demos ハブ」として盛り込み、章立てに対応させる。

Playdate 開発ガイド / テンプレートの未着手・保留タスク。

## 環境

- [ ] **ARM toolchain のインストール**（実機ビルド用。シミュレータには不要）
  - `brew install --cask gcc-arm-embedded` が developer.arm.com からのDL失敗で未完了
  - ダメなら developer.arm.com から手動DL → `.pkg` インストール
  - 入ったら実機ビルド確認: `cmake -B build-device -DCMAKE_TOOLCHAIN_FILE="$PLAYDATE_SDK_PATH/C_API/buildsupport/arm.cmake" && cmake --build build-device`

## 多言語化

- [x] **日本語フォント生成・同梱**（PixelMplus12 / M+ライセンス → `tools/gen_jp_font.py` で生成、ASCII+かな+第1水準漢字 約3500字）
- [ ] 翻訳文言の見直し（i18n.c の TABLE。仮訳のまま）
- [ ] 第2水準漢字が要る固有名詞などが出たら、gen_jp_font.py の ku 範囲に 48-84 を追加して再生成

## アート

- [ ] **タイトル画像を本番アートに差し替え**：今は自作の簡易ライン画プレースホルダ
  （`tools/gen_title_placeholder.py` で生成、`gen_launcher.py` でカード/起動画像も生成）。
  ※公開のため Panic 公式アートは除去済み（自作のみ）
- [ ] 必要なら言語別タイトル画像（title_en / title_ja）

## 構成（Demos廃止 → ステージ＝操作別ミニゲームに集約）

- [x] Demos ハブ廃止・導線整理（splash→title→stage select→loading→stage）
- [x] ステージ＝操作別ミニゲーム：移動(十字)/クランク/傾き、クリアで★保存
- [x] ローディング画面（40×28アニメ・擬似待ち時間）を stage select→stage に挿入
- [x] 各ステージ導入に会話ウィンドウで操作説明
- [x] **README を新構成に更新**
- [x] **オーディオ拡張**：BGM(fileplayer ループ) + g_sound_on 連動（audio_refresh）
- [x] **タイマー/トゥイーン**（tween.{h,c}・ステージ選択カーソルに適用）
- [x] **タイルマップ**ステージ（迷路・グリッド移動）
- [x] **会話ウィンドウ拡張**：名前ボックス + 選択肢（Moveステージで実演）
- [ ] （任意）立ち絵対応・長文の複数ページ送り
- [ ] （任意）再利用リストUI部品（stage select / settings の共通化）
- [ ] （任意）シンセ音（PDSynth）デモ
- [ ] 実機ビルド（ARM toolchain 導入）/ ランチャー本番アート / 翻訳見直し

## 細かい改善

- [ ] **⊙メニューで Sound を直接トグル**（`addCheckmarkMenuItem`）
- [ ] 実際の勝利条件で `save_mark_stage_cleared` を呼ぶ（今はデモで「プレイ=クリア」）
- [ ] `(Menu btn: options)` 表記の記号化検討

## 完了済み（参考）

- [x] SDK / cmake セットアップ、最小Cプロジェクト
- [x] シーン管理（スタック: switch/push/pop、ライフサイクル、状態管理）
- [x] フェード遷移（Bayerディザ）
- [x] オーバーレイ（ポーズ画面）
- [x] シーン間データ受け渡し（arg/result）
- [x] ⊙システムメニュー連携（build_menu フック、Settings→設定シーン）
- [x] 多言語化（i18n: tr()、EN/JA、言語切替、Credits画面）
- [x] 日本語フォント生成・同梱（PixelMplus12）→ 英日共用フォントに統一(サイズ/太さ揃え)
- [x] タイトル画面の画像化（img_load/draw ヘルパー + プレースホルダ画像）
- [x] 設定画面のレイアウト統一（コロン揃え・値整列、Backは中央寄せ）
- [x] タイトルを画像化（GAME TEMPLATE + Playdateデバイス線画、クッキリ2値化）
- [x] タイトル画面の⊙メニュー（Settings/Credits）。⊙項目名はOS制約で英語固定
- [x] タイトルの次=ステージセレクト化（反転バー選択、Stage 1-3、★クリア表示）
- [x] ⊙メニューを全シーンで・各シーンでカスタマイズ可能に（sysmenu 共通部品）
- [x] 自己レビュー：menu表記の修正、SCENE_MENU→SCENE_STAGE_SELECT 改名、デッド文字列削除
- [x] セーブ/ロード（save.{h,c}、Dataフォルダにファイル保存。設定+クリア状況、終了/スリープ/変更時に保存）
- [x] オーディオ（audio.{h,c}、SE=select/confirm、g_sound_on連動。tools/gen_sfx.py）
- [x] ライフサイクル（kEventTerminate/Lock で自動セーブ）
- [x] 会話ウィンドウ（scene_dialogue.c、UTF-8タイプライター+折返し、本編⊙の Talk(demo) から）
- [x] 配布アセット（card.png 350x155 / launchImage.png、pdxinfo整備、tools/gen_launcher.py）
- [x] git init + .gitignore（build/・*.pdx 除外）+ 初回コミット
- [x] Demos ハブ（⊙メニューから）+ クランク/加速度センサ/スプライト デモ
- [x] スプラッシュ(オープニング)シーン：起動→タイトル、Aスキップ（動画.pdvは不採用＝軽量コード演出に）
- [x] GitHub 公開（MIT、Panicアート除去、NOTICE/商標免責）→ KOBUO/playdate-template-c
