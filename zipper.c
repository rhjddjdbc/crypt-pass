#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void add_to_zip(zip_t *zip, const char *base, const char *path) {
    struct stat st;
    stat(path, &st);
    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) return;
        struct dirent *entry;
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            char full[1024];
            snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);
            add_to_zip(zip, base, full);
        }
        closedir(dir);
    } else {
        char rel[1024];
        snprintf(rel, sizeof(rel), "%s", path + strlen(base) + 1);
        zip_source_t *src = zip_source_file(zip, path, 0, 0);
        if (src)
            zip_file_add(zip, rel, src, ZIP_FL_OVERWRITE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <file1> [file2 ...] <output.zip>\n", argv[0]);
        return 1;
    }

    const char *zipname = argv[argc - 1];
    int err = 0;
    zip_t *zip = zip_open(zipname, ZIP_CREATE | ZIP_TRUNCATE, &err);
    if (!zip) {
        fprintf(stderr, "Error creating: %s\n", zipname);
        return 1;
    }

    for (int i = 1; i < argc - 1; ++i) {
        add_to_zip(zip, argv[i], argv[i]);
    }

    zip_close(zip);
    printf("ZIP created: %s\n", zipname);
    return 0;
}

