#/bin/bash
rm -R build/
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=clang++ ..
mv compile_commands.json ..
