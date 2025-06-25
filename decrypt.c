#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#define SALT_SIZE 16
#define IV_SIZE 16
#define KEY_SIZE 32
#define ITERATIONS 100000

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <encrypted_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char *encrypted_file = argv[1];
    const char *output_file = argv[2];

    char password[128];
    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    FILE *fin = fopen(encrypted_file, "rb");
    if (!fin) { perror("Opening file"); return 1; }

    fseek(fin, 0, SEEK_END);
    long total_size = ftell(fin); rewind(fin);

    unsigned char salt[SALT_SIZE], iv[IV_SIZE], key[KEY_SIZE];
    fread(salt, 1, SALT_SIZE, fin);
    fread(iv, 1, IV_SIZE, fin);

    long ciphertext_len = total_size - SALT_SIZE - IV_SIZE;
    unsigned char *ciphertext = malloc(ciphertext_len);
    fread(ciphertext, 1, ciphertext_len, fin);
    fclose(fin);

    PKCS5_PBKDF2_HMAC(password, strlen(password), salt, SALT_SIZE,
                      ITERATIONS, EVP_sha256(), KEY_SIZE, key);

    unsigned char *plaintext = malloc(ciphertext_len + EVP_MAX_BLOCK_LENGTH);
    int len, plaintext_len;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;
    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) <= 0) {
        printf("Decryption failed: wrong password or corrupted file.\n");
        free(ciphertext); free(plaintext); EVP_CIPHER_CTX_free(ctx);
        return 1;
    }
    plaintext_len += len;

    FILE *fout = fopen(output_file, "wb");
    fwrite(plaintext, 1, plaintext_len, fout);
    fclose(fout);

    EVP_CIPHER_CTX_free(ctx);
    free(ciphertext); free(plaintext);
    printf("File successfully decrypted to %s\n", output_file);
    return 0;
}

