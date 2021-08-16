# cmake -H. -Btmp_cmake -DCMAKE_INSTALL_PREFIX=~/id
# cmake --build tmp_cmake --clean-first --target install
# ~/id/bin/generate_input > in.txt
rm -rf tmp/* && cmake -H. -Btmp -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=true -DENABLE_UBSAN=true && cmake --build tmp && ./tmp/demo < in.txt
