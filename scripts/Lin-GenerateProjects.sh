#!/bin/bash
pushd "$(dirname "$0")/.."
./vendor/bin/premake/premake5 codelite
popd
read -p "Press any key to continue..."
