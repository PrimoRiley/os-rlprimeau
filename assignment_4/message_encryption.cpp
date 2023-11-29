#include <iostream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

void handleErrors() {
    ERR_print_errors_fp(stderr); // prints the OpenSSL error messages
    abort(); // stops running
}

// using AES-256-CBC
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx; // container SSL uses to keep track of decryption encryption info

    int len;
    int ciphertext_len;

    // create a new encryption
    if(!(ctx = EVP_CIPHER_CTX_new())) 
        handleErrors();

    // initialize encryption with AES-256-CBC
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    // encrypt text
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    // adds padding to complete the block
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) 
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx); // clean encryption

    return ciphertext_len; 
}

// Function to decrypt ciphertext using AES-256-CBC
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx; // 'container' for decryption

    int len;
    int plaintext_len;

    if(!(ctx = EVP_CIPHER_CTX_new())) 
        handleErrors();

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) 
        handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx); // clean decryption container

    return plaintext_len;
}


int main (void) {
    // hardcoded 32-byte key and 16-byte IV 
    unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
    unsigned char *iv = (unsigned char *)"0123456789012345";

    // text to be encrypted
    unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog";
    unsigned char ciphertext[128]; // Buffer for the ciphertext
    unsigned char decryptedtext[128]; // Buffer for the decrypted text

    int decryptedtext_len, ciphertext_len;

    // Load error strings and algorithms for OpenSSL
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    // encrypt the text
    ciphertext_len = encrypt(plaintext, strlen ((char *)plaintext), key, iv, ciphertext);

    std::cout << "Ciphertext is:\n";
    BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

    // decrypt the ciphertext 
    decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);
    decryptedtext[decryptedtext_len] = '\0'; // Null-terminate the decrypted text

    std::cout << "Decrypted text is:\n";
    std::cout << decryptedtext << std::endl;

    // Cleanup OpenSSL
    EVP_cleanup(); // Cleans up all algorithms
    ERR_free_strings(); // Frees all error strings

    return 0; 
}
