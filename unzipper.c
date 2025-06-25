#include <stdio.h>
#include <stdlib.h>
#include <zip.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("use: %s <zipdatei>\n", argv[0]);
        return 1;
    }

    int err = 0;
    zip_t *zip = zip_open(argv[1], 0, &err);
    if (!zip) {
        printf("Error bei ZIP.\n");
        return 1;
    }

    zip_int64_t entries = zip_get_num_entries(zip, 0);
    for (zip_uint64_t i = 0; i < entries; ++i) {
        const char *name = zip_get_name(zip, i, 0);
        zip_file_t *zfile = zip_fopen_index(zip, i, 0);

        FILE *out = fopen(name, "wb");
        if (!out) continue;

        char buf[1024];
        zip_int64_t n;
        while ((n = zip_fread(zfile, buf, sizeof(buf))) > 0)
            fwrite(buf, 1, n, out);

        fclose(out);
        zip_fclose(zfile);
    }

    zip_close(zip);
    printf("ZIP unpacked.\n");
    return 0;
}

