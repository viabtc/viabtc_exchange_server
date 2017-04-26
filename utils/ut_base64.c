/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/26, create
 */

# include <stdint.h>
# include <string.h>
# include <openssl/bio.h>
# include <openssl/evp.h>
# include <openssl/buffer.h>

# include "ut_base64.h"

int base64_encode(const unsigned char *buffer, size_t length, sds *b64text)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    *b64text = sdsnewlen(bufferPtr->data, bufferPtr->length);

    return 0;
}

size_t calc_decode_length(const char* b64input)
{
    size_t len = strlen(b64input);
    size_t padding = 0;
    if (b64input[len - 1] == '=' && b64input[len - 2] == '=')
        padding = 2;
    else if (b64input[len - 1] == '=')
        padding = 1;
    return (len * 3) / 4 - padding;
}

int base64_decode(char *b64message, unsigned char **buffer, size_t *length)
{
    BIO *bio, *b64;

    int decodeLen = calc_decode_length(b64message);
    *buffer = (unsigned char*)malloc(decodeLen + 1);
    (*buffer)[decodeLen] = '\0';

    bio = BIO_new_mem_buf(b64message, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    *length = BIO_read(bio, *buffer, strlen(b64message));
    BIO_free_all(bio);

    return 0;
}

