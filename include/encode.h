#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types


#define MAX_SECRET_BUF_SIZE 50
#define MAX_IMAGE_BUF_SIZE  8
#define MAX_FILE_SUFFIX 5
#define MAX_PWD_SIZE 10

/* Structure to store information required for encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored */

typedef struct EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
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

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_encode_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);
 
/* Copy bmp image header */
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String and Password String */
Status encode_string(const char *string, EncodeInfo *encInfo);

/* Store password length and password */
Status encode_password(EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(const char *data, char *image_buffer);

/* Encode a integer into LSB of image data array*/
Status encode_int_to_lsb(int *length, EncodeInfo *encInfo);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
