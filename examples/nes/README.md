# NES emulator "Cycloa" on Spresense

This code is for running Cycloa on Spresense. Refer [Cycloa](https://github.com/ledyba/Cycloa) to lean more about Cycloa.
The performance is not good enough at the moment. The frame rate is only about 75ms, so further improvement is needed.
In addition, I cannot cope with sound yet. If the frame rate is not improved, it seems that a decent sound cannot be produced, so it is postponed.
(Fog: It's going to be hard from here..)

Note: Changes to Cycloa for this are in [here](https://github.com/tttk-prj/Cycloa) by forked from the original Cycloa.


このコードはオープンソースのNESエミュレータ「Cycloa」をSpresense上で動かすようにしたものです。Cycloaの詳しい内容は、[こちら](https://github.com/ledyba/Cycloa)を参照してください。
現時点では、まだパフォーマンスが良くありません。フレームレートが75ms程度しか出ておらず、更なる改善が必要です。
また、まだ音の出力には対応できていません。フレームレートが改善されないことには、まともな音は出ないと思われるため、後回しにしています。
（ぼやき：ここからが大変そう。。）

注：Cycloaに対する変更は、オリジナルのCycloaのリポジトリをフォークして、[こちら](https://github.com/tttk-prj/Cycloa)に入れています。


# Hardware requirement

Spresense Main Board : https://www.switch-science.com/catalog/3900/

Spresense Extension Board : https://www.switch-science.com/catalog/3901/

Arduino UNO LCD Connector board : https://www.switch-science.com/catalog/5728/

ILI9341 2.2inch LCD : https://www.switch-science.com/catalog/5902/

Game pad : https://www.amazon.co.jp/snesゲーム対応-ゲームパッド-ゲームハンドル-USB接続-アナログコントローラー-高耐久ボタン-レトロクラシック-ホワイト-２個セット-Iseebiz/dp/B07YWF6RX9/ref=asc_df_B07YWF6RX9

Note: The gamepad can not be used as it is, it is modified inside and connected to Spresense. I think it is good to show other appropriate buttons.

注：ゲームパッドは、そのままでは使えず、中を改造して Spresenseに接続しています。
　　他の適当なボタンなどを組み合わせるのも良いかと思います。

# How to build

It is assumed that the development environment of Spresense has been constructed. Refer to [here](https://developer.sony.com/ja/develop/spresense/) for details on environment construction.

The following is from source code clone to build.


Spresenseの開発環境の構築をされていることを前提にします。環境構築について詳しくは[こちら](https://developer.sony.com/ja/develop/spresense/)を参照してください。

以下、ソースコードのcloneからビルドまでです。


```
git clone --recursive https://github.com/tttk-prj/spresense.git
cd spresense/sdk
./tools/config.py -k release
make buildkernel
./tools/config.py nes
make
```

Note: Cycloa is cloned in this step.

注：Cycloaはこのステップの中でgit cloneされます。


# How to flash it to spresense board

The method of writing to the board after building is as follows.
The point is to install not only "nuttx.spk" but also the "renderer" of the worker.

ビルド後の実機のボードへの書き込み方法は以下です。
ポイントはnuttx.spkだけではなく、workerのrendererもインストールするところです。

```
./tools/flash.sh -c <Your devide port name> -w ../examples/nes/video_worker/renderer/renderer
./tools/flash.sh -c <Your devide port name> nuttx.spk
```

Note: Of course, the Spresense boot loader must be pre-installed on the board. (I am addicted to this.)

注：当然ですが、Spresenseのブートローダーがボードに予めインストールされている必要があります。（私はこれにハマりました。。）


# How to play

Two startup modes are implemented, one is a pattern that starts automatically when the power is turned on, and the other is started by inputting a command from NuttShell.
Note that Cycloa supports iNES format ROM images, so iNES format images are required.


電源を入れると自動的に  起動するパターンとNuttShellからコマンドと入力して起動する、２つの起動モードを実装しています。
なお、CycloaはiNESフォーマットのROMイメージに対応しているので、iNES形式のイメージが必要です。

## Run automatically

Save the rom image you want to run with the file name game.nes on the SD card, stab it on the Spresense Extension board, and turn on the power.
The emulator starts automatically by loading game.nes.

SDカードに、game.nes というファイル名で動かしたいromイメージを保存して、Spresense Extensionボードに刺して電源を入れると、
自動的に、game.nesを読み込んでエミュレータが起動します。

## Run manually

If there is no game.nes file on the SD card, NuttShell will start.
When you connect to the spresense board from terminal software such as minicom, the prompt "nsh>" will appear,
If you enter the command "nes" and the iNES image file name you want to execute in its argument, the image specified by the argument will be loaded and started.

SDカードに、game.nesというファイルが存在しないと、NuttShellが起動するようになっています。
minicomなどのターミナルソフトから実機に繋ぐと、nsh>というプロンプトが出てくるので、
"nes"というコマンドと、その引数に実行したいiNESイメージファイル名を入れれば、引数で指定されたイメージをロードして起動します。

```
nsh> nes <iNES file name>
```

Note: When specifying a file on the SD card, it seems that it is necessary to always specify it with the full path, and it must be "/mnt/sd0/<file name on the SD card>".

注：SDカードにあるファイルを指定する場合、必ずフルパスで指定する必要があるようで、"/mnt/sd0/<SDカードにあるファイル名>"としなくてはいけないようです。


# Game pad Key assign

The gamepad should be prepared by yourself.
Connect to Spresense pin header and read as GPIO Input.
In my case, I purchased the game pad described in "Hardware Requirement" and remodeled the inside and used it.
The relationship between the position of the pin header and the button is defined in examples/nes/spresense_gamepad_fairy.h.
Please change according to your environment.


ゲームパッドは、自前で適当なものを準備してもらえば良いです。
Spresenseのピンヘッダに接続して、GPIOのInputとして読み込んでます。
私の場合、Hardware Requirementに記載したゲームパッドを購入して中を改造して利用しました。
ピンヘッダの位置とボタンの関係は、examples/nes/spresense_gamepad_fairy.hにある、

```
# define BUTTON_SELECT PIN_I2S0_BCK
# define BUTTON_START  PIN_I2S0_LRCK
# define BUTTON_A      PIN_UART2_RTS
# define BUTTON_B      PIN_UART2_CTS

# define BUTTON_DOWN   PIN_EMMC_DATA2
# define BUTTON_LEFT   PIN_I2S0_DATA_IN
# define BUTTON_UP     PIN_I2S0_DATA_OUT
# define BUTTON_RIGHT  PIN_EMMC_DATA3
```

に定義しています。
環境に合わせて変更してください。




