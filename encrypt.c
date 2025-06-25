#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define SALT_SIZE 16
#define IV_SIZE 16
#define KEY_SIZE 32
#define ITERATIONS 100000

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char *infile = argv[1];
    const char *outfile = argv[2];
    char password[128];

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    unsigned char salt[SALT_SIZE], iv[IV_SIZE], key[KEY_SIZE];
    RAND_bytes(salt, SALT_SIZE);
    RAND_bytes(iv, IV_SIZE);

    PKCS5_PBKDF2_HMAC(password, strlen(password), salt, SALT_SIZE,
                      ITERATIONS, EVP_sha256(), KEY_SIZE, key);

    FILE *fin = fopen(infile, "rb");
    if (!fin) { perror("Opening file"); return 1; }
    fseek(fin, 0, SEEK_END);
    long filesize = ftell(fin); rewind(fin);

    unsigned char *plaintext = malloc(filesize);
    fread(plaintext, 1, filesize, fin);
    fclose(fin);

    unsigned char *ciphertext = malloc(filesize + EVP_MAX_BLOCK_LENGTH);
    int len, ciphertext_len;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, filesize);
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    FILE *fout = fopen(outfile, "wb");
    fwrite(salt, 1, SALT_SIZE, fout);
    fwrite(iv, 1, IV_SIZE, fout);
    fwrite(ciphertext, 1, ciphertext_len, fout);
    fclose(fout);

    EVP_CIPHER_CTX_free(ctx);
    free(plaintext); free(ciphertext);

    printf("File successfully encrypted to %s\n", outfile);
    return 0;
}

