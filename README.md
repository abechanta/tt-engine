# tt-engine

[tt-engine](https://github.com/abechanta/tt-engine) は以下の特徴を持つアニメーションシステム（のリファレンス実装）です。

-	既存のあらゆる描画サブシステムと共存可能（※１）
-	データドリブン
-	小さなコードセットと少ない学習コスト

詳しくは「C++ で挑む 2D アニメーションシステム制作演習」（すらりんラボ発行、技術書展・BOOTH 等にて頒布）をご覧ください。

### ※１: 標準提供される描画サブシステム用アダプター

-	[SDL2](https://www.libsdl.org/index.php) 

## 使用例

-	[sdl_app::transition::beginMenu() in tt-engine/examples/sdl_app.cpp](https://github.com/abechanta/tt-engine/blob/revise-for-2nd-edition/examples/sdl_app.cpp#L291)

## ビルド環境の準備

### Windows 10

-	git for Windows をインストールする
-	Visual Studio 2019 か Visual Studio 2022 をインストールする
-	Visual Studio Installer で以下の拡張をインストールする
	-	ワークロード「C++ によるデスクトップ開発」
	-	個別のコンポーネント「CMake の Visual C++ ツール」
-	[vcpkg](https://github.com/Microsoft/vcpkg) をインストールする
	-	$ git clone https://github.com/microsoft/vcpkg.git
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
	-	$ vcpkg install boost --triplet=x64-windows
1.	vcpkg で [SDL2](https://www.libsdl.org/index.php) をインストールする
	-	$ vcpkg install sdl2 --triplet=x64-windows
1.	vcpkg で [SDL2_image](https://www.libsdl.org/projects/SDL_image/) をインストールする
	-	$ vcpkg install sdl2-image --triplet=x64-windows

ソースコードのビルド

1.	[tt-engine](https://github.com/abechanta/tt-engine) をローカルに展開する
	-	$ git clone https://github.com/abechanta/tt-engine.git
1.	Visual Studio 上の「ファイル ＞ 開く ＞ フォルダー...」でダイアログを開き、ローカルに展開したフォルダーを指定する
1.	Visual Studio 上の「CMake ＞ CMake の設定を変更 ＞ tt-engine」で CMakeSettings.json を開き、キー名 CMAKE_TOOLCHAIN_FILE に対応する文字列中の value （計 2 か所）を vcpkg のインストールパスに変更する
1.	Visual Studio 上の「CMake ＞ ビルドのみ ＞ tutorial.exe」を実行する

### Linux

※ 未確認

### Mac OS X

※ 未確認

## 参考情報: ビルド確認済みの環境

### Windows 10

    Visual Studio Professional 2019 Version 16.11.5
    Vcpkg package management program version 2021-10-25-b4bff9afacafb0a234a8f20fd6451b2362c0f9a0
    cmake version 3.20.21032501-MSVC_2

    boost-accumulators:x64-windows                     1.77.0           Boost accumulators module
    boost-algorithm:x64-windows                        1.77.0           Boost algorithm module
    boost-align:x64-windows                            1.77.0           Boost align module
    boost-any:x64-windows                              1.77.0           Boost any module
    boost-array:x64-windows                            1.77.0           Boost array module
    boost-asio:x64-windows                             1.77.0           Boost asio module
    boost-assert:x64-windows                           1.77.0           Boost assert module
    boost-assign:x64-windows                           1.77.0           Boost assign module
    boost-atomic:x64-windows                           1.77.0           Boost atomic module
    boost-beast:x64-windows                            1.77.0           Boost beast module
    boost-bimap:x64-windows                            1.77.0           Boost bimap module
    boost-bind:x64-windows                             1.77.0           Boost bind module
    boost-build:x64-windows                            1.77.0           Boost.Build
    boost-callable-traits:x64-windows                  1.77.0           Boost callable_traits module
    boost-chrono:x64-windows                           1.77.0           Boost chrono module
    boost-circular-buffer:x64-windows                  1.77.0           Boost circular_buffer module
    boost-compatibility:x64-windows                    1.77.0           Boost compatibility module
    boost-compute:x64-windows                          1.77.0           Boost compute module
    boost-concept-check:x64-windows                    1.77.0           Boost concept_check module
    boost-config:x64-windows                           1.77.0#2         Boost config module
    boost-container-hash:x64-windows                   1.77.0           Boost container_hash module
    boost-container:x64-windows                        1.77.0           Boost container module
    boost-context:x64-windows                          1.77.0           Boost context module
    boost-contract:x64-windows                         1.77.0           Boost contract module
    boost-conversion:x64-windows                       1.77.0           Boost conversion module
    boost-convert:x64-windows                          1.77.0           Boost convert module
    boost-core:x64-windows                             1.77.0           Boost core module
    boost-coroutine2:x64-windows                       1.77.0           Boost coroutine2 module
    boost-coroutine:x64-windows                        1.77.0           Boost coroutine module
    boost-crc:x64-windows                              1.77.0           Boost crc module
    boost-date-time:x64-windows                        1.77.0           Boost date_time module
    boost-describe:x64-windows                         1.77.0           Boost describe module
    boost-detail:x64-windows                           1.77.0           Boost detail module
    boost-dll:x64-windows                              1.77.0           Boost dll module
    boost-dynamic-bitset:x64-windows                   1.77.0           Boost dynamic_bitset module
    boost-endian:x64-windows                           1.77.0           Boost endian module
    boost-exception:x64-windows                        1.77.0           Boost exception module
    boost-fiber:x64-windows                            1.77.0           Boost fiber module
    boost-filesystem:x64-windows                       1.77.0           Boost filesystem module
    boost-flyweight:x64-windows                        1.77.0           Boost flyweight module
    boost-foreach:x64-windows                          1.77.0           Boost foreach module
    boost-format:x64-windows                           1.77.0           Boost format module
    boost-function-types:x64-windows                   1.77.0           Boost function_types module
    boost-function:x64-windows                         1.77.0           Boost function module
    boost-functional:x64-windows                       1.77.0           Boost functional module
    boost-fusion:x64-windows                           1.77.0           Boost fusion module
    boost-geometry:x64-windows                         1.77.0           Boost geometry module
    boost-gil:x64-windows                              1.77.0#1         Boost gil module
    boost-graph:x64-windows                            1.77.0           Boost graph module
    boost-hana:x64-windows                             1.77.0           Boost hana module
    boost-heap:x64-windows                             1.77.0           Boost heap module
    boost-histogram:x64-windows                        1.77.0           Boost histogram module
    boost-hof:x64-windows                              1.77.0           Boost hof module
    boost-icl:x64-windows                              1.77.0           Boost icl module
    boost-integer:x64-windows                          1.77.0           Boost integer module
    boost-interprocess:x64-windows                     1.77.0           Boost interprocess module
    boost-interval:x64-windows                         1.77.0           Boost interval module
    boost-intrusive:x64-windows                        1.77.0           Boost intrusive module
    boost-io:x64-windows                               1.77.0           Boost io module
    boost-iostreams:x64-windows                        1.77.0#1         Boost iostreams module
    boost-iostreams[bzip2]:x64-windows                                  Support bzip2 filters
    boost-iostreams[lzma]:x64-windows                                   Support LZMA/xz filters
    boost-iostreams[zlib]:x64-windows                                   Support zlib filters
    boost-iostreams[zstd]:x64-windows                                   Support zstd filters
    boost-iterator:x64-windows                         1.77.0           Boost iterator module
    boost-json:x64-windows                             1.77.0           Boost json module
    boost-lambda2:x64-windows                          1.77.0           Boost lambda2 module
    boost-lambda:x64-windows                           1.77.0           Boost lambda module
    boost-leaf:x64-windows                             1.77.0           Boost leaf module
    boost-lexical-cast:x64-windows                     1.77.0           Boost lexical_cast module
    boost-local-function:x64-windows                   1.77.0           Boost local_function module
    boost-locale:x64-windows                           1.77.0           Boost locale module
    boost-lockfree:x64-windows                         1.77.0           Boost lockfree module
    boost-log:x64-windows                              1.77.0           Boost log module
    boost-logic:x64-windows                            1.77.0           Boost logic module
    boost-math:x64-windows                             1.77.0           Boost math module
    boost-metaparse:x64-windows                        1.77.0           Boost metaparse module
    boost-modular-build-helper:x64-windows             1.77.0#4         Internal vcpkg port used to build Boost libraries
    boost-move:x64-windows                             1.77.0           Boost move module
    boost-mp11:x64-windows                             1.77.0           Boost mp11 module
    boost-mpl:x64-windows                              1.77.0           Boost mpl module
    boost-msm:x64-windows                              1.77.0           Boost msm module
    boost-multi-array:x64-windows                      1.77.0           Boost multi_array module
    boost-multi-index:x64-windows                      1.77.0           Boost multi_index module
    boost-multiprecision:x64-windows                   1.77.0           Boost multiprecision module
    boost-nowide:x64-windows                           1.77.0           Boost nowide module
    boost-numeric-conversion:x64-windows               1.77.0           Boost numeric_conversion module
    boost-odeint:x64-windows                           1.77.0#1         Boost odeint module
    boost-optional:x64-windows                         1.77.0           Boost optional module
    boost-outcome:x64-windows                          1.77.0           Boost outcome module
    boost-parameter-python:x64-windows                 1.77.0           Boost parameter_python module
    boost-parameter:x64-windows                        1.77.0           Boost parameter module
    boost-pfr:x64-windows                              1.77.0           Boost pfr module
    boost-phoenix:x64-windows                          1.77.0           Boost phoenix module
    boost-poly-collection:x64-windows                  1.77.0           Boost poly_collection module
    boost-polygon:x64-windows                          1.77.0           Boost polygon module
    boost-pool:x64-windows                             1.77.0           Boost pool module
    boost-predef:x64-windows                           1.77.0           Boost predef module
    boost-preprocessor:x64-windows                     1.77.0           Boost preprocessor module
    boost-process:x64-windows                          1.77.0#2         Boost process module
    boost-program-options:x64-windows                  1.77.0           Boost program_options module
    boost-property-map:x64-windows                     1.77.0           Boost property_map module
    boost-property-tree:x64-windows                    1.77.0           Boost property_tree module
    boost-proto:x64-windows                            1.77.0           Boost proto module
    boost-ptr-container:x64-windows                    1.77.0           Boost ptr_container module
    boost-python:x64-windows                           1.77.0#1         Boost python module
    boost-python[python3]:x64-windows                                   Build with Python3 support
    boost-qvm:x64-windows                              1.77.0           Boost qvm module
    boost-random:x64-windows                           1.77.0           Boost random module
    boost-range:x64-windows                            1.77.0           Boost range module
    boost-ratio:x64-windows                            1.77.0           Boost ratio module
    boost-rational:x64-windows                         1.77.0           Boost rational module
    boost-regex:x64-windows                            1.77.0           Boost regex module
    boost-safe-numerics:x64-windows                    1.77.0           Boost safe_numerics module
    boost-scope-exit:x64-windows                       1.77.0           Boost scope_exit module
    boost-serialization:x64-windows                    1.77.0           Boost serialization module
    boost-signals2:x64-windows                         1.77.0           Boost signals2 module
    boost-smart-ptr:x64-windows                        1.77.0           Boost smart_ptr module
    boost-sort:x64-windows                             1.77.0           Boost sort module
    boost-spirit:x64-windows                           1.77.0           Boost spirit module
    boost-stacktrace:x64-windows                       1.77.0           Boost stacktrace module
    boost-statechart:x64-windows                       1.77.0           Boost statechart module
    boost-static-assert:x64-windows                    1.77.0           Boost static_assert module
    boost-static-string:x64-windows                    1.77.0           Boost static_string module
    boost-stl-interfaces:x64-windows                   1.77.0           Boost stl_interfaces module
    boost-system:x64-windows                           1.77.0           Boost system module
    boost-test:x64-windows                             1.77.0           Boost test module
    boost-thread:x64-windows                           1.77.0           Boost thread module
    boost-throw-exception:x64-windows                  1.77.0           Boost throw_exception module
    boost-timer:x64-windows                            1.77.0           Boost timer module
    boost-tokenizer:x64-windows                        1.77.0           Boost tokenizer module
    boost-tti:x64-windows                              1.77.0           Boost tti module
    boost-tuple:x64-windows                            1.77.0           Boost tuple module
    boost-type-erasure:x64-windows                     1.77.0           Boost type_erasure module
    boost-type-index:x64-windows                       1.77.0           Boost type_index module
    boost-type-traits:x64-windows                      1.77.0           Boost type_traits module
    boost-typeof:x64-windows                           1.77.0           Boost typeof module
    boost-ublas:x64-windows                            1.77.0           Boost ublas module
    boost-uninstall:x64-windows                        1.77.0           Internal vcpkg port used to uninstall Boost
    boost-units:x64-windows                            1.77.0           Boost units module
    boost-unordered:x64-windows                        1.77.0           Boost unordered module
    boost-utility:x64-windows                          1.77.0           Boost utility module
    boost-uuid:x64-windows                             1.77.0           Boost uuid module
    boost-variant2:x64-windows                         1.77.0           Boost variant2 module
    boost-variant:x64-windows                          1.77.0           Boost variant module
    boost-vcpkg-helpers:x64-windows                    1.77.0           Internal vcpkg port used to modularize Boost
    boost-vmd:x64-windows                              1.77.0           Boost vmd module
    boost-wave:x64-windows                             1.77.0           Boost wave module
    boost-winapi:x64-windows                           1.77.0           Boost winapi module
    boost-xpressive:x64-windows                        1.77.0           Boost xpressive module
    boost-yap:x64-windows                              1.77.0           Boost yap module
    boost:x64-windows                                  1.77.0#2         Peer-reviewed portable C++ source libraries
    bzip2:x64-windows                                  1.0.8#2          bzip2 is a freely available, patent free, high-q...
    expat:x64-windows                                  2.4.1            XML parser library written in C
    libffi:x64-windows                                 3.4.2#2          Portable, high level programming interface to va...
    liblzma:x64-windows                                5.2.5#4          Compression library with an API similar to that ...
    libpng:x64-windows                                 1.6.37#15        libpng is a library implementing an interface fo...
    openssl:x64-windows                                1.1.1l#3         OpenSSL is an open source project that provides ...
    python3:x64-windows                                3.10.0           The Python programming language
    sdl2-image:x64-windows                             2.0.5#2          SDL_image is an image file loading library. It l...
    sdl2:x64-windows                                   2.0.16#1         Simple DirectMedia Layer is a cross-platform dev...
    sqlite3:x64-windows                                3.36.0#1         SQLite is a software library that implements a s...
    vcpkg-cmake-config:x64-windows                     2021-09-27       
    vcpkg-cmake:x64-windows                            2021-09-13       
    zlib:x64-windows                                   1.2.11#13        A compression library
    zstd:x64-windows                                   1.5.0#1          Zstandard - Fast real-time compression algorithm
