yum -y install zlib-devel openssl-devel curl-devel libuuid-devel mysql-devel libevent-devel gcc gcc-c++ make cmake

cd deps
cd hiredis
cmake .
make
make install
cd ..
cd ..
cmake .
make
make install
cd libs
cmake .
make
cd ..