#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#endif

#include "zstddeclib.c"

#define BUF_SIZE (16*1024)

static int
write_all(const unsigned char *data, size_t size)
{
    while (size > 0) {
        ssize_t n = write(1, data, size);

        if (n <= 0)
            return 1;

        data += n;
        size -= n;
    }
    return 0;
}

int
main(void)
{
#ifdef _WIN32
    _setmode(0, _O_BINARY);
    _setmode(1, _O_BINARY);
#endif

    ZSTD_DStream *dstream = ZSTD_createDStream();

    if (!dstream)
        return 1;

    size_t ret = ZSTD_initDStream(dstream);

    if (ZSTD_isError(ret))
        return 2;

    _Alignas(16) unsigned char in[BUF_SIZE];
    _Alignas(16) unsigned char out[BUF_SIZE];

    for (;;) {
        ssize_t r = read(0, in, BUF_SIZE);

        if (!r)
            break;

        if (r <= 0)
            return 3;

        ZSTD_inBuffer input = {in, (size_t)r, 0};

        while (input.pos < input.size) {
            ZSTD_outBuffer output = {out, BUF_SIZE, 0};

            size_t ret = ZSTD_decompressStream(dstream, &output, &input);

            if (ZSTD_isError(ret))
                return 4;

            if (write_all(out, output.pos))
                return 5;
        }
    }
    ZSTD_freeDStream(dstream);
    return 0;
}
