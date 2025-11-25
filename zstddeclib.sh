git clone https://github.com/facebook/zstd -b release &&
(cd zstd/build/single_file_libs && sh create_single_file_decoder.sh) &&
mv zstd/build/single_file_libs/zstddeclib.c .
rm -rf zstd
