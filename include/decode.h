#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_SECRET_BUF_SIZE 50
#define MAX_IMAGE_BUF_SIZE  8
#define MAX_FILE_SUFFIX 5
#define MAX_PWD_SIZE 10

/* Structure to store information required for decoding stego image to secret file
 * Info about output and intermediate data is
 * also decoded */

typedef struct DecodeInfo
{
    /* Image buffer info */
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
	char pass_secret_file[MAX_PWD_SIZE];
    char secret_data[MAX_SECRET_BUF_SIZE];
	int size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* decode data from image */
Status decode_data_from_image(char *string, int size, DecodeInfo *decInfo);

/* decode Magic String and Password String */
Status decode_string(const char *string, DecodeInfo *decInfo);

/* Decode password length and password */
Status decode_password(DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file's size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* decode stego image to secret file */
Status decode_lsb_to_byte(char *data, char *image_buffer);

/* decode stego image to integer */
Status decode_lsb_to_int(int *data, DecodeInfo *decInfo);

#endif
