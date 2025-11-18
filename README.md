# 使い方
PATH に /opt/share/mncl-kobe/bin されており、
clang と clojure が使えることが前提です。

```
jc03z1cp01:cl2vsm$ PATH=/opt/share/mncl-kobe/bin:$PATH
jc03z1cp01:cl2vsm$ clang --version
clang version 16.0.6
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /opt/share/mncl-kobe/bin
jc03z1cp01:cl2vsm$ clojure --version
Clojure CLI version 1.11.1.1413
```

build は ninja を使います。
最初に build_cl2vsm.ninja を生成します。

```
jc03z1cp01:cl2vsm$ sh bin/mk_build_cl2vsm.sh  > build_cl2vsm.ninja
```

その後、単純に ninja で build できます。
```
jc03z1cp01:cl2vsm$ ninja
[2/7] Compiling OpenCL C for mncore ob..._build/double-vecadd._vsm to emit vsm.
Function name: main_kernel0
Arg#0 address space: 1
Arg#1 address space: 1
Arg#2 address space: 1
[5/7] Compiling OpenCL C for mncore object _build/nop._vsm to emit vsm.
Function name: my_kernel
[7/7] java -jar /opt/share/mncl-kobe/b...vecadd._vsm > _build/double-vecadd.vsm
```

生成物は _build の下に展開されます。
```
jc03z1cp01:cl2vsm$ ls _build/
double-vecadd._vsm   double-vecadd.vsm    nop.vsm
double-vecadd.ll     nop._vsm
double-vecadd.param  nop.ll
jc03z1cp01:cl2vsm$ ls _build/*.vsm
_build/double-vecadd.vsm  _build/nop.vsm
```

cl-src の下にもとの OpenCL のソースがあります。
```
ls cl-src/
double-vecadd.cl  double-vecadd.param  list.txt  nop.cl
```

## vecadd

このサンプルには以下の 2 ファイルがあります:

- [`cl-src/double-vecadd.cl`](cl-src/double-vecadd.cl)  
  OpenCL のソースコードです。

- [`cl-src/double-vecadd.param`](cl-src/double-vecadd.param)  
  実行時のパラメータ (PDM のアドレスなど) を記述しています。

# 仕組み
double-veadd.cl が clang でコンパイルされて double-veadd._vsm になります。
この時点では PDM とかのアドレスを決定できないので、アセンブラとして
疑似コードが一部入っています。

その後、clojure のスクリプトで、同名の param ファイル(この場合は double-veadd.param) と double-veadd._vsm をみて、アセンブル可能な double-veadd.vsm を
出力します。

# ホスト側のソース

## vsm-golden
変更してほしくないゴールデンのデータを用意してある。
まずはこれを clone する。

```
$ git clone /opt/share/repos/vsm-golden.git/
```

## 単純に build 
build すれば _build/vecadd ができるはず。

```
jc03z1cp01:cl2vsm$ ninja
[2/2] Linking target _build/vecadd.
jc03z1cp01:cl2vsm$ ls _build/vecadd*
_build/vecadd*  _build/vecadd.o
```

## 実行前のアセンブル
実行にさきだち vsm をアセンブルする!!

すると、_build/double-vecadd.vsm をアセンブルして _build/double-vecadd.vsm.asm ができます。
これは仕様です。ハイ。

is_vsmwrite OK

と表示されれば、たぶん大丈夫でしょう。

```
jc03z1cp01:cl2vsm$ ./_build/vecadd --vsm-write
./gen_asm_file.rev.sh _build/double-vecadd.vsm; sleep 1
Traceback (most recent call last):
  File "/home/ext-ryos36/codegen/070b786d8/pfcomp/gpfn2/assembler/gpfnasm/main.py", line 202, in <module>
    tc.check()
  File "/home/ext-ryos36/codegen/070b786d8/pfcomp/gpfn2/assembler/gpfnasm/tag_checker.py", line 189, in check
    self.check_balance()
  File "/home/ext-ryos36/codegen/070b786d8/pfcomp/gpfn2/assembler/gpfnasm/tag_checker.py", line 178, in check_balance
    raise InbalancedWaitException(self.tag_balance_wait)
assembler.gpfnasm.tag_checker.InbalancedWaitException: [tag balance] : [0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
[how to read tag balance] : negative values mean too many `wait`, positive values mean too few `wait`. should be all zero.
[more readable error] :
too many wait with i02 (1 more)
is_vsmwrite OK

jc03z1cp01:cl2vsm$ ls _build/double-vecadd.vsm*
_build/double-vecadd.vsm  _build/double-vecadd.vsm.asm
```

## 実行
_build/vecadd で実行です。

err_n が 0 ならエラーがありません。
結果がすべて 8192 なので、あまりインパクトがありませんが。

```
jc03z1cp01:cl2vsm$ ./_build/vecadd
instsize 0 124 _build/double-vecadd.vsm


****************
Result
8192.000000 8192.000000 8192.000000 8192.000000 8192.000000 8192.000000 8192.000000 8192.000000
8192.000000 8192.000000 8192.000000 8192.000000 8192.000000 8192.000000 8192.000000 8192.000000
...

err_n:0
```

## 実行時の注意点
なんかの拍子で、MN-Core がストールすると、初期化しないと
うまくうごかなくなるようです。
本当はライブラリ側で起動時にリセット的な何かをすべきなのでしょう。
他のプログラムはそうなっているみたいです。

ということで、他のプログラムを呼ぶことで、MN-Core は通常の
動きを取り戻します。

これは Docker のコンテナの環境が必要になります。

```
codegen# pwd
/home/ext-ryos36/codegen/070b786d8/pfcomp/codegen
codegen# ./build/tools/run_vsm --gpfn 2 ~/mncl/cl2vsm/_build/nop.vsm
Assembling /home/ext-ryos36/mncl/cl2vsm/_build/nop.vsm ...
Running assembled code 1 times...
Done!
N: 1
M: 10
total time: 0.717ms 322637cyc @449.981MHz
time to run 1 iter (best): 0.0717ms 32263.7cyc @449.981MHz
time to run 1 iter (avg): 0.0717ms 32263.7cyc @449.981MHz
```

あるいは

```
    ./build/mnlinker2/mnlinker2_test
```

この場合は、最後に All test passed と出力されればOK


## 仕組み
実行に先立って、vsm が必要です。もとのライブラリは

```
    gpfn_setvsmfile("_build/double-vecadd.vsm");
```

としたあとに

```
  qvsm(" nop; wait i%02x             \n",1);
  qvsm(" nop/3                       \n");
```

みたいにして、vsm を構築していくものでした。
これだと、すでにある vsm に対応できないので、

```
    gpfn_setvsmfile_ro("_build/double-vecadd.vsm");
```

というインタフェースを"考案"してしまいました。
ro は ReadOnly のニュアンスです。

将来的にはより洗練された API にすべきでしょう。

### アセンブルする
vsm だけあっても実行できないので、アセンブルするのですが、
アセンブルは一度、--vsm-write のオプションをつけて実行することで
実現しています。このときは、実際の実行はしません。

実行時に argc, argv を見ています。次のマクロがそれです。

```
#define CHECK_VSM_WRITE(__argc__, __argv__) \
do { \
    if (((__argc__) >= 2 ) && (strcmp((__argv__)[1], "--vsm-write") == 0)) { \
        gpfn_setvsmwrite(); \
        (__argc__)--; \
        (__argv__)++; \
    } \
} while (0);
```

gpfn_setvsmwrite を呼ぶと、アセンブルするモードになって、
実際の実行をしません。

アセンブル時には
```
gen_asm_file.rev.sh
```
というシェルプロを gpfn_sendvsm の中で呼んでいます。

つまり、アセンブルするモードのときは gpfn_sendvsm は vsm を送信せずに
アセンブルします。通常実行では vsm を送信して、実行をするようになっています。

将来的には build.ninja で、実行とは分離して、
アセンブルするようにすべきでしょう。
