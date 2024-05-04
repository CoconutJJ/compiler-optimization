cmake CMakeLists.txt -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cd build
make
ctest