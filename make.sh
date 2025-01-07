# Download openssl 1.1
cd gloo_test_lib
wget https://www.openssl.org/source/openssl-1.1.1t.tar.gz
tar -xvzf openssl-1.1.1t.tar.gz
cd openssl-1.1.1t

# build script for gloo_test
mkdir -p build
cd build
cmake ../ -DCMAKE_CXX_FLAGS="-static-libstdc++" -DCMAKE_CXX_STANDARD=14 -DBUILD_TEST=1  -DOPENSSL_ROOT_DIR=../gloo_test_lib/ssl_install -DOPENSSL_LIBRARIES=../ssl_install/lib/libssl.so.1.1
make -j16
