# tt-engine

tt-engine は以下の特徴を持つアニメーションシステム（のリファレンス実装）です。

-	既存のあらゆる描画サブシステムと共存可能
-	データドリブン
-	小さなコードセットと少ない学習コスト

詳しくは「C++ で挑む 2D アニメーションシステム制作演習」（すらりんラボ発行、技術書展５にて頒布）をご覧ください。

## ビルド手順

### 準備

Windows 10 におけるビルド環境

-	git をインストールする
-	Visual Studio 2015 もしくは 2017 をインストールする
-	Visual Studio Install で以下の拡張をインストールする
	-	ワークロード「C++ によるデスクトップ開発」
	-	個別のコンポーネント「CMake の Visual C++ ツール」

### 手順

依存ライブラリのインストール

1.	[vcpkg](https://github.com/Microsoft/vcpkg) をインストールする
	*	$ git clone git@github.com:Microsoft/vcpkg.git
2.	vcpkg で [Boost](https://www.boost.org/) をインストールする
	*	$ .\vcpkg install boost
3.	vcpkg で [SDL2](https://www.libsdl.org/index.php) をインストールする
	*	$ .\vcpkg install sdl2
4.	vcpkg で [SDL2_image](https://www.libsdl.org/projects/SDL_image/) をインストールする
	*	$ .\vcpkg install sdl2-image

ソースコードのビルド

1.	[tt-engine](https://github.com/abechanta/tt-engine) をインストールする
	*	$ git clone git@github.com:abechanta/tt-engine.git
2.	Visual Studio 上の「ファイル ＞ 開く ＞ フォルダー...」でダイアログを開き、ローカルに展開した tt-engine を指定する
3.	Visual Studio 上の「CMake ＞ ビルドのみ ＞ tutorial.exe」を選択する

※ Windows 10 & Visual Studio 以外の環境では未確認
