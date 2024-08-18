# Learn OS in 3days

## そもそもOSとは？

OS（オペレーティングシステム）は、ハードウェアを制御し、OS以外のソフトウェアに対して基本的な機能を提供するソフトウェアである。代表的なものとして、Linux、Windows、macOSなどがある。ソフトウェアを使う以上は逃れることのできないほどOSというのはとても身近な存在だが、何をもってOSとするかという厳密な定義は存在しない。そもそもOSがなくてもソフトウェアを動かすことはできる。しかし、当たり前だがソフトウェアはハードウェアの上で動作するため、その場合はハードウェアを直接制御するための実装が必要になる。またそのソフトウェアだけではなく、その他のソフトウェアも動作するとなると、複数のソフトウェア間でハードウェアという共通のリソースを共有するための仕組みが必要になる。これらの機能をソフトウェアを作るたびに実装するとなると非常に手間がかかる。そこで、それらの機能をまとめて提供するのがOSである。しかし、どの機能を提供するかはOSによって異なるため、何をもってOSと呼ぶのかという厳密な定義は存在しないわけだが、現代のOSであれば基本的な機能は大体共通しているため、それらを学ぶことでOSについての理解を深めることができる。

## OSの役割

OSの役割には大きく２つあり、ハードウェアリソースの管理と、ソフトウェアの実行の制御を行う。現代のコンピュータには、CPU、メモリ、ストレージ、その他の入出力装置などがあり、それらのハードウェアリソースを管理するために、メモリ管理、ファイル管理、入出力制御を行う。また、ソフトウェアの実行を制御するために、プロセス管理も行う。

- メモリ管理
- ファイル管理
- 入出力制御
- プロセス管理

## コンピュータのハードウェア

マザーボードはコンピュータの中心的な基盤となる回路基盤で、CPU、メモリ、ストレージ、入出力装置などの主要なコンポーネントを接続し、相互に通信させる役割を担っている。CPUとメインメモリはマザーボードを介してメモリバスと呼ばれる高速な信号線で接続される。また、SSDやGPUなどの高速なデバイスもマザーボードを介してPCI express（PCIe）と呼ばれる高速なバスでCPUと接続される。HDDやキーボード、USBなどの入出力装置はそれぞれを担当するデバイスコントローラに接続される。イーサネットやWi-Fiでパケットのやり取りに使用されるNICやWNICは入力装置でもあり出力装置でもあると言える。デバイスコントローラは入出力装置を制御するための制御レジスタを持っており、プロセッサは制御レジスタに命令を書き込んだり、制御レジスタの値を読み出すことで入出力装置を制御する。通常これらのデバイスコントローラは、チップセットと呼ばれるLSIに集約されていて、チップセットはマザーボードを介してCPUと、PCIeベースの通信プロトコルを使用したDMI（Direct Media Interface）と呼ばれる高速なバスで接続される。

## ISA

CPUの機種によって解釈できる機械語は異なる。CPUが解釈できる機械語命令や、使用できるレジスタなど、プログラムから見たときのCPUの使用をISA（命令セットアーキテクチャ）と呼ぶ。詳細は[ARM? RISC-V? x86? 用語がごっちゃになっている人のためのCPUアーキテクチャの全体図](https://qiita.com/rihib/items/6b70361b48c3840d09b0)を参照。

## マルチコア

CPUが機械語命令を順次実行していく流れのことをハードウェアスレッドという。コンピュータの性能を向上させるには大きく、クロック周波数を高くする方法と複数のプロセッサを用いて複数のハードウェアスレッドを並列に動作させる方法がある。複数のプロセッサを持つCPUのことをマルチコアCPUと呼ぶ。前者の方法はクロック周波数が3GHzを超えたあたりから頭打ちになりつつあり、現在は後者の方法が中心となっている。

## 動作モード

一般的にプロセッサはカーネル（特権）モードとユーザーモードと呼ばれる２つの動作モードを持つ。カーネルモードはカーネルを実行するためのモードであり、プロセッサの全ての動作を際限なく行えるのに対し、ユーザーモードはユーザープログラムを実行するためのモードであり、特権命令（システムに影響を与える可能性がある機械語命令）の実行やハードウェアへの直接アクセスが禁止される。ユーザーモードでこれらを実行しようとすると特権違反例外が発生し、カーネルに制御が移る。

## 割り込み

プロセッサは、システムで何らかの急いで処理すべき事象が発生したときに、実行中のプログラムを一時中断し、別のプログラムを実行する、割り込みと呼ばれる機能を持っている。このときに実行するプログラムを割り込みハンドラと呼ぶ。割り込みには、外部割り込みと内部割り込みがある。

外部割り込み（ハードウェア割り込み）は、プロセッサ外部のデバイスで何らかの事象が発生したことをプロセッサに通知するための割り込みである。プロセッサは外部からの割り込みを受け付けるためのIRQ（割り込み信号線）を持つ。

内部割り込みは、プログラムの実行に伴ってプロセッサ内部で発生する割り込みである。内部割り込みにはソフトウェア割り込みとトラップ（例外、フォールト）がある。ソフトウェア割り込みは特別な機械語命令を実行することで発生し、プロセッサのモードをユーザーモードからカーネルモードに変更するために使用される。トラップはプログラムの実行中になんらかの例外的な事象が発生することで引き起こされる。例えばゼロ除算例外、未定義命令違反、特権違反例外、セグメンテーションフォールト（アクセスが許可されていないアドレスを参照）、ページフォールト（物理メモリが割り当てられていない仮想ページを参照しようとしたとき）などがある。

## スタック

実行中に必要なデータを一時的に保持するために、スタックと呼ばれるメモリ領域を使用する。一般的なプロセッサはスタックトップ（最後にpushしたデータ）のアドレスを持つスタックポインタ（SP）というレジスタを持っている。スタックはレジスタに格納できない関数の引数やリターンアドレス、ローカル変数などを保持する。
