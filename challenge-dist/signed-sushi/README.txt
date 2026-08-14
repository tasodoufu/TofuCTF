# Signed Sushi

寿司トレーのラベル更新処理が、負の長さを受け入れてしまう。符号付き整数からサイズへ変換される境界を読み、関数ポインタを秘密の配膳係へ差し替えよう。

Topic: integer conversion / function-pointer overwrite

起動:

    ./run.sh
    nc 127.0.0.1 31337

停止:

    ./run.sh stop

この問題は完全にローカルなamd64 ELFであり、配布物にflagのリテラル値は含まれない。
