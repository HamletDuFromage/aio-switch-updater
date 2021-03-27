#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR
rm -r switch/aio-switch-updater/
mkdir -p switch/aio-switch-updater/
cp aio-switch-updater.nro switch/aio-switch-updater/
#VERSION=$(grep "APP_VERSION :=" Makefile | cut -d' ' -f4)
#cp aiosu-forwarder/aiosu-forwarder.nro switch/aio-switch-updater/aio-switch-updater-v$VERSION.nro
zip -FSr aio-switch-updater.zip switch/