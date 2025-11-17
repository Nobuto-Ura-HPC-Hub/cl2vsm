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

## ホスト側のソース

TBD
