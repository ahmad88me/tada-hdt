apt update
apt install cmake g++ -y
apt install zip wget -y
wget https://github.com/rdfhdt/hdt-cpp/archive/v1.3.2.zip
unzip v1.3.2.zip
rm v1.3.2.zip 

echo "Installing hdt-cpp..."
apt install vim -y 
apt install autoconf -y
apt install libtool -y
apt install pkg-config -y
apt install libserd-0-0 libserd-dev -y
apt install zlib1g-dev -y
cd hdt-cpp-1.3.2;./autogen.sh;./configure;make -j2;make install; cd ..;rm -Rf hdt-cpp-1.3.2

echo "Installing googletest suit (gtest)..."
wget https://github.com/google/googletest/archive/release-1.8.1.zip
unzip release-1.8.1.zip
rm release-1.8.1.zip
cd googletest-release-1.8.1; cmake -G "Unix Makefiles"; make; make install; cd ..; rm -Rf googletest-release-1.8.1

echo "Install coverage..."
apt install gcovr -y
