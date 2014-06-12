#!/bin/sh
GAMBIT_BIN="`echo "$0" | sed -e 's/MacOS\/Gambit/Resources\/bin/'`"
export "PATH=$GAMBIT_BIN:$PATH"
exec "$GAMBIT_BIN/gambit"
