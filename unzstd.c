#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "zstddeclib.c"

#define BUF_SIZE (16*1024)

static void
die(const char *msg)
{
    if (msg) {
        size_t len = strlen(msg);
        write(2, msg, len);
    }
    exit(1);
}

static void
write_all(unsigned char *data, size_t size)
{
    while (size > 0) {
        ssize_t written = write(1, data, size);

        if (written <= 0)
            die("Couldn't write in stdout");

        data += written;
        size -= written;
    }
}

int
main(void)
{
    ZSTD_DStream *dstream = ZSTD_createDStream();

    if (!dstream)
        die("Couldn't init zstd");

    size_t ret = ZSTD_initDStream(dstream);

    if (ZSTD_isError(ret))
        die(ZSTD_getErrorName(ret));

    _Alignas(16) unsigned char in[BUF_SIZE];
    _Alignas(16) unsigned char out[BUF_SIZE];
    size_t read_size;

    while ((read_size = read(0, in, BUF_SIZE)) > 0) {
        ZSTD_inBuffer input = {in, read_size, 0};

        while (input.pos < input.size) {
            ZSTD_outBuffer output = {out, BUF_SIZE, 0};

            size_t ret = ZSTD_decompressStream(dstream, &output, &input);

            if (ZSTD_isError(ret))
                die(ZSTD_getErrorName(ret));

            write_all(out, output.pos);
        }
    }
    if (read_size < 0)
        die("Couldn't read stdin");

    ZSTD_freeDStream(dstream);
    return 0;
}
