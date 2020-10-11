#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR
rm -r switch/aio-switch-updater/
mkdir -p switch/aio-switch-updater/
ls -1 *.nro | xargs -L1 -I{} cp {} switch/aio-switch-updater/{}
zip -FSr aio-switch-updater.zip switch/