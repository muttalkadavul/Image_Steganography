#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "encode.h"
#include "common.h"

/* Reading and validating arguments */
Status read_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
	int idx = 0;
	char extn_file[4];

	/* Allocating memory to save the file name */
	encInfo->src_image_fname = (char*)malloc(sizeof (char) * 32);
	encInfo->secret_fname = (char*)malloc(sizeof (char) * 32);
	encInfo->stego_image_fname = (char*)malloc(sizeof (char) * 32);
	
	/*reading the file names if no arguments are passed by the user*/
	if (argc != 5)
	{
		printf("Enter the source image file name : ");
		scanf("%s", encInfo->src_image_fname);

		printf("Enter the secret message file name : ");
		scanf("%s", encInfo->secret_fname);
		
		printf("Enter the destination image file name : ");
		scanf("%s", encInfo->stego_image_fname);
	
		while (getchar() != '\n');	
	}
	
	/*reading file names from command line*/
	else
	{
		strcpy(encInfo->src_image_fname, argv[2]);
		strcpy(encInfo->secret_fname, argv[3]);
		strcpy(encInfo->stego_image_fname, argv[4]);
	}

	/* reading source file extension */
	for (idx = 0; encInfo->src_image_fname[idx] != '.'; idx++);
	strcpy(extn_file , &(encInfo->src_image_fname[idx + 1]));
	
	/* validating source file extension */
	if (strcmp(extn_file, "bmp"))
	{
		printf("The Source image file should be a Bitmap file\n");
		return e_failure;
	}
	
	/* reading secret file extension */
	strcpy(encInfo->extn_secret_file , strchr(encInfo->secret_fname, '.'));
	
	/* reading dest file extension */
	strcpy(extn_file , strchr(encInfo->stego_image_fname, '.'));
	
	/* validating dest file extension */
	if (strcmp(extn_file, ".bmp"))
	{
		printf("The Dest image file should be a Bitmap file\n");
		return e_failure;
	}
	
	return e_success;
}

/* opening the files */
Status open_files(EncodeInfo *encInfo)
{
	/* opening the source image file */
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

	/* error handling */
	if (encInfo->fptr_src_image == NULL)
	{
		fprintf(stderr, "Error! The source image file %s doesn't exist\n", encInfo->src_image_fname);
		return e_failure;
	}

	/* opening the secret message file */
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

	/* error handling */
	if (encInfo->fptr_secret == NULL)
	{
		fprintf(stderr, "Error! The secret message file %s doesn't exist\n", encInfo->secret_fname);
		return e_failure;
	}

	/* opening the output file */
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	
	/* error handling */
	if (encInfo->fptr_stego_image == NULL)
	{
		fprintf(stderr, "Error! The output image file %s can't be opened\n", encInfo->stego_image_fname);
		return e_failure;
	}
	
	printf("Files are opened\n");

	return e_success;
}

/* checking if the image is big enough to steg */
Status check_capacity(EncodeInfo *encInfo)
{
	/* getting the size of the file */
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	//printf("Secret File Size = %d Bytes\n", encInfo->size_secret_file);

	/* getting the size of the image */
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
	//printf("Image Size = %d Bytes\n", encInfo->image_capacity);
	
	/* checking if the sizes are valid 
	   262 = 54(header) + 208(to save file details) 
	   8 = to encode 1 byte (8 bits) */
	if ((encInfo->image_capacity - 262) < (encInfo->size_secret_file * 8))
	{
		return e_failure;
	}
	
	printf("Sizes are valid\n");

	return e_success;
}

/* calculating file size */
uint get_file_size(FILE *fptr)
{
	uint fsize;

	/* moving the file pointer to the end */
	fseek(fptr, 0, SEEK_END);

	fsize = ftell(fptr);
	return fsize;
}

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	//printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	//printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* copying the header file to the dest image*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char buff[54];

	/* rewinding to the start */
	rewind(fptr_src_image);
	fread(buff, sizeof (char), 54, fptr_src_image);
	fwrite(buff, sizeof (char), 54, fptr_dest_image);
	printf("Header copied\n");

	return e_success;
}

/* encoding data to the image */
Status encode_data_to_image(const char *data, int size, EncodeInfo *encInfo)
{
	int idx;

	for (idx = 0; idx < size; idx++)
	{
		/* reading 8 bytes from source image */
		fread(encInfo->image_data, sizeof (char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_src_image);

		/* encoding a byte to the image buffer */
		encode_byte_to_lsb(&(data[idx]), encInfo->image_data);

		/* writing to dest image */
		fwrite(encInfo->image_data, sizeof (char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_stego_image);
	}

	return e_success;
}

/* encoding a byte to the image buffer */
Status encode_byte_to_lsb(const char *data, char *image_buffer)
{
	int idx; 

	/* loop to encode byte by byte */
	for (idx = 0; idx < 8; idx++)
	{
		if (*data & (1 << idx))
		{
			image_buffer[idx] |= 1;                         
		}
		else
		{
			image_buffer[idx] &= 0;
		}
	}
	return e_success;
}

/* encoding integer to lsb of image buffer */
Status encode_int_to_lsb(int *data, EncodeInfo *encInfo)
{
	int idx;
	char image_data[32];

	/* reading 32 bytes to save an integer */
	fread(image_data, sizeof (char), 32, encInfo->fptr_src_image);
	
	/* looping to save 32 bits in lsb's of 32 bytes */
	for (idx = 0; idx < 32; idx++)
	{
		if (*data & (1 << idx))
		{
			image_data[idx] |= 1;                         
		}
		else
		{
			image_data[idx] &= 0;
		}
	}

	/* writing 32 bytes to stego image */
	fwrite(image_data, sizeof (char), 32, encInfo->fptr_stego_image);

	return e_success;
}

/* encoding a string to the dest image */
Status encode_string(const char *string, EncodeInfo *encInfo)
{
	encode_data_to_image(string, strlen(string), encInfo);

	return e_success;
}

/* encoding password to the image */
Status encode_password(EncodeInfo *encInfo)
{	
	/* encoding the password string */
	if (encode_string(PWD_STRING, encInfo))
	{
		printf("Encoding password string failed\n");
		return e_failure;
	}

	int length = strlen(encInfo->pass_secret_file);

	/* encoding the length of the password */
	encode_int_to_lsb(&length, encInfo);
	
	/* encoding the password to the image */
	encode_data_to_image(encInfo->pass_secret_file, length, encInfo);
	
	printf("Password is encoded\n");

	return e_success;
}

/* encoding secret file's extension */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn, strlen(file_extn), encInfo);

	return e_success;
}

/* encoding secret file's size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
	encode_int_to_lsb(&file_size, encInfo);
	
	return e_success;
}

/* encoding secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	rewind(encInfo->fptr_secret);
	
	int size;

	/* reading secret data to the buffer */
	while ((size = fread(encInfo->secret_data, sizeof (char), MAX_SECRET_BUF_SIZE, encInfo->fptr_secret)) != 0)
	{
		/* encoding the secret data to the image */
		encode_data_to_image(encInfo->secret_data, size, encInfo);
	}
	
	printf("Data has been encoded\n");

	return e_success;
}

/* copying remaining data to the dest image */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char *temp = malloc(100000);
	
	while (fread(temp, sizeof (char), 100000, fptr_src))
	{
		fwrite(temp, sizeof (char), 100000, fptr_dest);
	}

	return e_success;
}

/* driver function for Encoding */
Status do_encoding(EncodeInfo *encInfo)
{
	/* opening files to perform stegging */
	if (open_files(encInfo) == e_failure)
	{
		printf("Opening failed !\n");
		return e_failure;
	}

	/* checking capacity of the image to encode */
	if (check_capacity(encInfo) == e_failure)
	{
		printf("The size of the message is too large to encode\n");
		return e_failure;
	}

	/* copying header */
	if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image))
	{
		printf("Copying of header file failed\n");
		return e_failure;
	}

	/* encoding string */
	if (encode_string(MAGIC_STRING, encInfo) == e_failure)
	{
		printf("Encoding magic string failed\n");
		return e_failure;
	}

	/* checking if the user wants to encrypt */
	printf("Do you want to encrypt ? (Y/N)");
	
	char ch;	
	ch = getchar();

	if (ch == 'Y' || ch == 'y')
	{
		printf("Enter the password (MAXIMUM of 10 characters)\n");
		scanf("\n%[^\n]", encInfo->pass_secret_file);

		/* encoding password to the image */
		if (encode_password(encInfo))
		{
			printf("Encoding Password failed\n");
			return e_failure;
		}
	}
	
	/* Encoding file extension */
	if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
	{
		printf("Encoding file extension failed\n");
		return e_failure;
	}

	/* Encoding file size */
	if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
	{
		printf("Encoding file size failed\n");
		return e_failure;
	}
	
	/* Encoding file data */
	if (encode_secret_file_data(encInfo) == e_failure)
	{
		printf("Encoding size of the file failed\n");
	}

	/* Encoding remaining image data */
	if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
	{
		printf("Copying remaining data failed\n");
	}
	
	/* closing opened files */
	fclose(encInfo->fptr_src_image);
	fclose(encInfo->fptr_secret);
	fclose(encInfo->fptr_stego_image);

	return e_success;
}
