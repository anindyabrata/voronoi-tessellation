rm -rf tmp/* && cmake -H. -Btmp -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=true -DENABLE_UBSAN=true && cmake --build tmp && ./tmp/devmain < in.txt
