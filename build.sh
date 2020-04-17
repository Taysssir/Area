#!/bin/sh

rm -rf build
rm -rf deploy
mkdir build
cd build
qmake ..
make -j 5
cd ..
mkdir deploy
cp build/sentinel-admin deploy
cp sentinel-admin.sh deploy
cd deploy
ldd sentinel-admin | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' .
cp -r /usr/lib/x86_64-linux-gnu/qt5/plugins .
cd ..

branch=$(git rev-parse --abbrev-ref HEAD)
commit=$(git rev-parse --short HEAD)
filename="sentinel-admin-$branch.$commit.tar.gz"
tar -cvzf $filename deploy

