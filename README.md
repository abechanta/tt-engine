# tt-engine

[tt-engine](https://github.com/abechanta/tt-engine) は以下の特徴を持つアニメーションシステム（のリファレンス実装）です。

-	既存のあらゆる描画サブシステムと共存可能（※１）
-	データドリブン
-	小さなコードセットと少ない学習コスト

詳しくは「C++ で挑む 2D アニメーションシステム制作演習」（すらりんラボ発行、技術書展５にて頒布）をご覧ください。

### ※１: 標準提供される描画サブシステム用アダプター

-	[SDL2](https://www.libsdl.org/index.php) 

## 使用例

※ 準備中

## ビルド環境の準備

### Windows 10

-	git for Windows をインストールする
-	Visual Studio 2015 もしくは 2017 をインストールする
-	Visual Studio Installer で以下の拡張をインストールする
	-	ワークロード「C++ によるデスクトップ開発」
	-	個別のコンポーネント「CMake の Visual C++ ツール」
-	[vcpkg](https://github.com/Microsoft/vcpkg) をインストールする
	-	$ git clone git@github.com:Microsoft/vcpkg.git
	-	$ cd vcpkg
	-	$ .\bootstrap-vcpkg.bat
	-	$ .\vcpkg integrate install

### Linux

※ 未確認

### Mac OS X

※ 未確認

## ビルド手順

### Windows 10

依存ライブラリのインストール

1.	vcpkg で [Boost](https://www.boost.org/) をインストールする
	-	$ .\vcpkg install boost
1.	vcpkg で [SDL2](https://www.libsdl.org/index.php) をインストールする
	-	$ .\vcpkg install sdl2
1.	vcpkg で [SDL2_image](https://www.libsdl.org/projects/SDL_image/) をインストールする
	-	$ .\vcpkg install sdl2-image

ソースコードのビルド

1.	[tt-engine](https://github.com/abechanta/tt-engine) をローカルに展開する
	-	$ git clone git@github.com:abechanta/tt-engine.git
1.	Visual Studio 上の「ファイル ＞ 開く ＞ フォルダー...」でダイアログを開き、ローカルに展開したフォルダーを指定する
1.	Visual Studio 上の「CMake ＞ ビルドのみ ＞ tutorial.exe」を実行する

### Linux

※ 未確認

### Mac OS X

※ 未確認

