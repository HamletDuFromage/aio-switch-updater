#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR
rm -r switch/aio-switch-updater/
mkdir -p switch/aio-switch-updater/
mv *.nro switch/aio-switch-updater/aio-switch-updater.nro
zip -r aio-switch-updater.zip switch/