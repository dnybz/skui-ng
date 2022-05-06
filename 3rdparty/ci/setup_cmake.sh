#! /usr/bin/env bash

VERSION=${1}
OS=${2}
if [[ ${OS} == win64 ]]
then
  ARCH=x64
  EXT=zip
  EXTRACT='unzip -q'
else
  ARCH=x86_64
  EXT=tar.gz
  EXTRACT='tar -xf'
fi

MAJOR=`echo ${VERSION} | cut -d. -f1`
MINOR=`echo ${VERSION} | cut -d. -f2`
REVISION=`echo ${VERSION} | cut -d. -f3`

FILE=cmake-${VERSION}-${OS}-${ARCH}.${EXT}
wget https://github.com/Kitware/CMake/releases/download/v${MAJOR}.${MINOR}.${REVISION}/${FILE}
${EXTRACT} ${FILE}
if [[ ${OS} == Darwin ]]
then
  export PATH=${PWD}/cmake-${VERSION}-${OS}-${ARCH}/CMake.app/Contents/bin:${PATH}
else
  export PATH=${PWD}/cmake-${VERSION}-${OS}-${ARCH}/bin:${PATH}
fi
echo "Extracted CMake and set PATH=$PATH"
cmake --version
