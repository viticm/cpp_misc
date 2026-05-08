# 怎样生成gcm.cache(for gcc 16.0.1)
g++ -std=c++26 -fmodules -fsearch-include-path bits/std.cc module_test.cpp

g++ -std=c++26-fmodules module_test.cpp -o module_test
