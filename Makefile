NAME := unzstd
CFLAGS = -O2 -ffunction-sections -fdata-sections -fno-unwind-tables
LDFLAGS = -Wl,--gc-sections -Wl,--strip-all

.PHONY: all
all: aarch64-macos-$(NAME) x86_64-macos-$(NAME) \
     aarch64-linux-$(NAME) x86_64-linux-$(NAME)

zstddeclib.c:
	git clone https://github.com/facebook/zstd -b release
	(cd zstd/build/single_file_libs && sh create_single_file_decoder.sh)
	cp zstd/build/single_file_libs/zstddeclib.c .
	rm -rf zstd

%-linux-$(NAME): LDFLAGS += -static

%-linux-$(NAME): unzstd.c zstddeclib.c
	zig cc -target $*-linux-musl $(CFLAGS) $< -o $@ $(LDFLAGS)

%-macos-$(NAME): unzstd.c zstddeclib.c
	zig cc -target $*-macos-none $(CFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *-$(NAME)
