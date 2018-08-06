mkdir build-nosgct
cd build-nosgct
cmake -GNinja -DVISCOM_USE_SGCT=OFF ..

cd ..

mkdir build-sgct
cd build-sgct
cmake -GNinja -DSGCT_DEBUG_LIBRARY=/home/bone/projects/sgct/build/lib/linux_x64/libsgctd.a -DSGCT_RELEASE_LIBRARY=/home/bone/projects/sgct/build/lib/linux_x64/libsgct.a -DSGCT_INCLUDE_DIRECTORY=/home/bone/projects/sgct/include ..
