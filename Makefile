CFLAGS = -O2 -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections -s

.PHONY: all
all: aarch64-macos-unzstd \
     aarch64-linux-unzstd \
     x86_64-linux-unzstd

zstddeclib.c:
	git clone https://github.com/facebook/zstd -b release
	(cd zstd/build/single_file_libs && sh create_single_file_decoder.sh)
	cp zstd/build/single_file_libs/zstddeclib.c .
	rm -rf zstd

aarch64-macos-unzstd: unzstd.c zstddeclib.c
	zig cc -target aarch64-macos-none $(CFLAGS) $< -o $@ $(LDFLAGS)

aarch64-linux-unzstd: unzstd.c zstddeclib.c
	zig cc -target aarch64-linux-musl $(CFLAGS) $< -o $@ -static $(LDFLAGS)

x86_64-linux-unzstd: unzstd.c zstddeclib.c
	zig cc -target x86_64-linux-musl $(CFLAGS) $< -o $@ -static $(LDFLAGS)

.PHONY: clean
clean:
	git clean -xfd
