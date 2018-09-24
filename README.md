# tt-engine

[tt-engine](https://github.com/abechanta/tt-engine) は以下の特徴を持つアニメーションシステム（のリファレンス実装）です。

-	既存のあらゆる描画サブシステムと共存可能（※１）
-	データドリブン
-	小さなコードセットと少ない学習コスト

詳しくは「C++ で挑む 2D アニメーションシステム制作演習」（すらりんラボ発行、技術書展５にて頒布）をご覧ください。

### ※１: 標準提供される描画サブシステム用アダプター

-	[SDL2](https://www.libsdl.org/index.php) 

## 使用例

-	[Showcase1::initialize() in tt-engine/examples/showcase1.cpp](https://github.com/abechanta/tt-engine/blob/revise-examples/examples/showcase1.cpp#L51)

## ビルド環境の準備

### Windows 10

-	git for Windows をインストールする
-	Visual Studio 2017 をインストールする
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
1.	Visual Studio 上の「CMake ＞ CMake の設定を変更 ＞ tt-engine」で CMakeSettings.json を開き、キー名 CMAKE_TOOLCHAIN_FILE に対応する文字列中の &lt;installed-folder-path&gt; を vcpkg のインストールパスに変更する
1.	Visual Studio 上の「CMake ＞ ビルドのみ ＞ tutorial.exe」を実行する

### Linux

※ 未確認

### Mac OS X

※ 未確認

## 参考情報: ビルド確認済みの環境

### Windows 10

	boost-accumulators:x64-windows                     1.67.0           Boost accumulators module
	boost-algorithm:x64-windows                        1.67.0           Boost algorithm module
	boost-align:x64-windows                            1.67.0           Boost align module
	boost-any:x64-windows                              1.67.0           Boost any module
	boost-array:x64-windows                            1.67.0           Boost array module
	boost-asio:x64-windows                             1.67.0-1         Boost asio module
	boost-assert:x64-windows                           1.67.0           Boost assert module
	boost-assign:x64-windows                           1.67.0           Boost assign module
	boost-atomic:x64-windows                           1.67.0           Boost atomic module
	boost-beast:x64-windows                            1.67.0           Boost beast module
	boost-bimap:x64-windows                            1.67.0           Boost bimap module
	boost-bind:x64-windows                             1.67.0           Boost bind module
	boost-build:x64-windows                            1.67.0           Boost.Build
	boost-callable-traits:x64-windows                  1.67.0           Boost callable_traits module
	boost-chrono:x64-windows                           1.67.0           Boost chrono module
	boost-circular-buffer:x64-windows                  1.67.0           Boost circular_buffer module
	boost-compatibility:x64-windows                    1.67.0           Boost compatibility module
	boost-compute:x64-windows                          1.67.0           Boost compute module
	boost-concept-check:x64-windows                    1.67.0           Boost concept_check module
	boost-config:x64-windows                           1.67.0           Boost config module
	boost-container-hash:x64-windows                   1.67.0           Boost container_hash module
	boost-container:x64-windows                        1.67.0           Boost container module
	boost-context:x64-windows                          1.67.0           Boost context module
	boost-contract:x64-windows                         1.67.0           Boost contract module
	boost-conversion:x64-windows                       1.67.0           Boost conversion module
	boost-convert:x64-windows                          1.67.0           Boost convert module
	boost-core:x64-windows                             1.67.0           Boost core module
	boost-coroutine2:x64-windows                       1.67.0           Boost coroutine2 module
	boost-coroutine:x64-windows                        1.67.0           Boost coroutine module
	boost-crc:x64-windows                              1.67.0           Boost crc module
	boost-date-time:x64-windows                        1.67.0           Boost date_time module
	boost-detail:x64-windows                           1.67.0           Boost detail module
	boost-disjoint-sets:x64-windows                    1.67.0           Boost disjoint_sets module
	boost-dll:x64-windows                              1.67.0           Boost dll module
	boost-dynamic-bitset:x64-windows                   1.67.0           Boost dynamic_bitset module
	boost-endian:x64-windows                           1.67.0           Boost endian module
	boost-exception:x64-windows                        1.67.0           Boost exception module
	boost-fiber:x64-windows                            1.67.0           Boost fiber module
	boost-filesystem:x64-windows                       1.67.0           Boost filesystem module
	boost-flyweight:x64-windows                        1.67.0           Boost flyweight module
	boost-foreach:x64-windows                          1.67.0           Boost foreach module
	boost-format:x64-windows                           1.67.0           Boost format module
	boost-function-types:x64-windows                   1.67.0           Boost function_types module
	boost-function:x64-windows                         1.67.0           Boost function module
	boost-functional:x64-windows                       1.67.0           Boost functional module
	boost-fusion:x64-windows                           1.67.0           Boost fusion module
	boost-geometry:x64-windows                         1.67.0           Boost geometry module
	boost-gil:x64-windows                              1.67.0           Boost gil module
	boost-graph-parallel:x64-windows                   1.67.0           Boost graph_parallel module
	boost-graph:x64-windows                            1.67.0           Boost graph module
	boost-hana-msvc:x64-windows                        1.67.0-1         Boost hana module
	boost-hana:x64-windows                             1.67.0-1         Boost hana module
	boost-heap:x64-windows                             1.67.0           Boost heap module
	boost-hof:x64-windows                              1.67.0           Boost hof module
	boost-icl:x64-windows                              1.67.0           Boost icl module
	boost-integer:x64-windows                          1.67.0           Boost integer module
	boost-interprocess:x64-windows                     1.67.0           Boost interprocess module
	boost-interval:x64-windows                         1.67.0           Boost interval module
	boost-intrusive:x64-windows                        1.67.0           Boost intrusive module
	boost-io:x64-windows                               1.67.0           Boost io module
	boost-iostreams:x64-windows                        1.67.0-1         Boost iostreams module
	boost-iterator:x64-windows                         1.67.0           Boost iterator module
	boost-lambda:x64-windows                           1.67.0           Boost lambda module
	boost-lexical-cast:x64-windows                     1.67.0           Boost lexical_cast module
	boost-local-function:x64-windows                   1.67.0           Boost local_function module
	boost-locale:x64-windows                           1.67.0           Boost locale module
	boost-lockfree:x64-windows                         1.67.0           Boost lockfree module
	boost-log:x64-windows                              1.67.0           Boost log module
	boost-logic:x64-windows                            1.67.0           Boost logic module
	boost-math:x64-windows                             1.67.0           Boost math module
	boost-metaparse:x64-windows                        1.67.0           Boost metaparse module
	boost-modular-build-helper:x64-windows             2018-05-14       
	boost-move:x64-windows                             1.67.0           Boost move module
	boost-mp11:x64-windows                             1.67.0           Boost mp11 module
	boost-mpl:x64-windows                              1.67.0           Boost mpl module
	boost-msm:x64-windows                              1.67.0           Boost msm module
	boost-multi-array:x64-windows                      1.67.0           Boost multi_array module
	boost-multi-index:x64-windows                      1.67.0           Boost multi_index module
	boost-multiprecision:x64-windows                   1.67.0           Boost multiprecision module
	boost-numeric-conversion:x64-windows               1.67.0           Boost numeric_conversion module
	boost-odeint:x64-windows                           1.67.0           Boost odeint module
	boost-optional:x64-windows                         1.67.0           Boost optional module
	boost-parameter:x64-windows                        1.67.0           Boost parameter module
	boost-phoenix:x64-windows                          1.67.0           Boost phoenix module
	boost-poly-collection:x64-windows                  1.67.0           Boost poly_collection module
	boost-polygon:x64-windows                          1.67.0           Boost polygon module
	boost-pool:x64-windows                             1.67.0           Boost pool module
	boost-predef:x64-windows                           1.67.0           Boost predef module
	boost-preprocessor:x64-windows                     1.67.0           Boost preprocessor module
	boost-process:x64-windows                          1.67.0           Boost process module
	boost-program-options:x64-windows                  1.67.0           Boost program_options module
	boost-property-map:x64-windows                     1.67.0           Boost property_map module
	boost-property-tree:x64-windows                    1.67.0           Boost property_tree module
	boost-proto:x64-windows                            1.67.0           Boost proto module
	boost-ptr-container:x64-windows                    1.67.0           Boost ptr_container module
	boost-python:x64-windows                           1.67.0-1         Boost python module
	boost-qvm:x64-windows                              1.67.0           Boost qvm module
	boost-random:x64-windows                           1.67.0           Boost random module
	boost-range:x64-windows                            1.67.0           Boost range module
	boost-ratio:x64-windows                            1.67.0           Boost ratio module
	boost-rational:x64-windows                         1.67.0           Boost rational module
	boost-regex:x64-windows                            1.67.0           Boost regex module
	boost-scope-exit:x64-windows                       1.67.0           Boost scope_exit module
	boost-serialization:x64-windows                    1.67.0           Boost serialization module
	boost-signals2:x64-windows                         1.67.0           Boost signals2 module
	boost-signals:x64-windows                          1.67.0           Boost signals module
	boost-smart-ptr:x64-windows                        1.67.0           Boost smart_ptr module
	boost-sort:x64-windows                             1.67.0           Boost sort module
	boost-spirit:x64-windows                           1.67.0           Boost spirit module
	boost-stacktrace:x64-windows                       1.67.0           Boost stacktrace module
	boost-statechart:x64-windows                       1.67.0           Boost statechart module
	boost-static-assert:x64-windows                    1.67.0           Boost static_assert module
	boost-system:x64-windows                           1.67.0           Boost system module
	boost-test:x64-windows                             1.67.0-2         Boost test module
	boost-thread:x64-windows                           1.67.0           Boost thread module
	boost-throw-exception:x64-windows                  1.67.0           Boost throw_exception module
	boost-timer:x64-windows                            1.67.0           Boost timer module
	boost-tokenizer:x64-windows                        1.67.0           Boost tokenizer module
	boost-tti:x64-windows                              1.67.0           Boost tti module
	boost-tuple:x64-windows                            1.67.0           Boost tuple module
	boost-type-erasure:x64-windows                     1.67.0           Boost type_erasure module
	boost-type-index:x64-windows                       1.67.0           Boost type_index module
	boost-type-traits:x64-windows                      1.67.0           Boost type_traits module
	boost-typeof:x64-windows                           1.67.0           Boost typeof module
	boost-ublas:x64-windows                            1.67.0           Boost ublas module
	boost-units:x64-windows                            1.67.0           Boost units module
	boost-unordered:x64-windows                        1.67.0           Boost unordered module
	boost-utility:x64-windows                          1.67.0           Boost utility module
	boost-uuid:x64-windows                             1.67.0           Boost uuid module
	boost-variant:x64-windows                          1.67.0           Boost variant module
	boost-vcpkg-helpers:x64-windows                    4                a set of vcpkg-internal scripts used to modulari...
	boost-vmd:x64-windows                              1.67.0           Boost vmd module
	boost-wave:x64-windows                             1.67.0           Boost wave module
	boost-winapi:x64-windows                           1.67.0           Boost winapi module
	boost-xpressive:x64-windows                        1.67.0           Boost xpressive module
	boost:x64-windows                                  1.67.0           Peer-reviewed portable C++ source libraries
	bzip2:x64-windows                                  1.0.6-2          High-quality data compressor.
	liblzma:x64-windows                                5.2.3-2          Compression library with an API similar to that ...
	libpng:x64-windows                                 1.6.35           libpng is a library implementing an interface fo...
	openssl-windows:x64-windows                        1.0.2o           OpenSSL is an open source project that provides ...
	openssl:x64-windows                                0                OpenSSL is an open source project that provides ...
	python3:x64-windows                                3.6.4-2          The Python programming language as an embeddable...
	sdl2-image:x64-windows                             2.0.2-3          SDL_image is an image file loading library. It l...
	sdl2:x64-windows                                   2.0.8-1          Simple DirectMedia Layer is a cross-platform dev...
	zlib:x64-windows                                   1.2.11-3         A compression library
