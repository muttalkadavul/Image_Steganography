#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "decode.h"
#include "common.h"

/* reading and validating decoding arguments */
Status read_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
	int idx = 0;
	char extn_src_file[5];

	/* allocating memory to save the file name */
	decInfo->secret_fname = (char*)malloc(sizeof (char) * 32);
	decInfo->stego_image_fname = (char*)malloc(sizeof (char) * 32);
	
	/* reading the file name if no arguments are passed by the user */
	if (argc != 3)
	{
		printf("Enter the stego image file name : ");
		scanf("%s", decInfo->stego_image_fname);
	}
	
	/* reading file name from command line */
	else
	{
		strcpy(decInfo->stego_image_fname, argv[2]);
	}

	/* reading source file extension */
	strcpy(extn_src_file , strchr(decInfo->stego_image_fname, '.'));
	
	/* validating source file extension */
	if (strcmp(extn_src_file, ".bmp"))
	{
		printf("The image file should be a Bitmap file\n");
		return e_failure;
	}

	return e_success;
}

/* opening files to decode */
Status open_decode_files(DecodeInfo *decInfo)
{
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

	/* error checking */
	if (decInfo->fptr_stego_image == NULL)
	{
		fprintf(stderr, "Error! The stego image file %s doesn't exist\n", decInfo->stego_image_fname);
		return e_failure;
	}
	
	printf("Files are opened\n");

	return e_success;
}

/* decoding data from stegged image */
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo)
{
	int idx;

	for (idx = 0; idx < size; idx++)
	{
		/* reading 8 bytes from stegged image to image buffer */
		fread(decInfo->image_data, sizeof (char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);

		/* decoding from image buffer */
		decode_lsb_to_byte(&(data[idx]), decInfo->image_data);
	}
}

/* decoding 8 bytes to one byte */
Status decode_lsb_to_byte(char *data, char *image_buffer)
{	
	int idx;
	*data = 0;
	
	for (idx = 0; idx < 8; idx++)
	{
		if (image_buffer[idx] & 1)
		{
			*data |= (1 << idx);
		}
	}

	return e_success;
}

/* decoding lsb to integer */
Status decode_lsb_to_int(int *data, DecodeInfo *decInfo)
{	
	int idx;
	*data = 0;
	char image_data[32];	
	
	fread(image_data, sizeof (char), 32, decInfo->fptr_stego_image);
	
	for (idx = 0; idx < 32; idx++)
	{
		if (image_data[idx] & 1)
		{
			*data |= (1 << idx);
		}
	}
	
	return e_success;
}

/* decoding string from stegged image */
Status decode_string(const char *string, DecodeInfo *decInfo)
{
	int idx;

	/* clearing the secret data buffer */
	memset(decInfo->secret_data, '\0', sizeof (decInfo->secret_data));
	
	decode_data_from_image(decInfo->secret_data, strlen(string), decInfo);

	/* verifying the string */
	for (idx = 0; idx < strlen(string); idx++)
	{
		if (string[idx] != decInfo->secret_data[idx])
		{
			return e_failure;
		}
	}
	
	return e_success;
}

/* decoding password from stegged image */
Status decode_password(DecodeInfo *decInfo)
{
	int length;
	
	/* clearing the data buffer */
	memset(decInfo->secret_data, '\0', sizeof (decInfo->secret_data));

	/* decoding length of the password */
	decode_lsb_to_int(&length, decInfo);

	/* checking the length of the password */
	if (length != strlen(decInfo->pass_secret_file))
	{
		return e_failure;
	}
	else
	{
		int idx;

		/* decoding password */
		decode_data_from_image(decInfo->secret_data, length, decInfo);

		/* verifying the password */
		for (idx = 0; idx < length; idx++)
		{
			if (decInfo->secret_data[idx] != decInfo->pass_secret_file[idx])
			{
				return e_failure;
			}
		}
	}

	return e_success;
}

/* decoding extension for secret file */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	int idx;

	/* resetting data buffer */
	memset(decInfo->secret_data, '\0', sizeof (decInfo->secret_data));

	decode_data_from_image(decInfo->extn_secret_file, MAX_FILE_SUFFIX - 1, decInfo);
	
	/* ending the buffer with null */
	decInfo->extn_secret_file[4] = '\0';

	/* reading name for secret file */
	strcpy(decInfo->secret_fname, "Secret");
	strcat(decInfo->secret_fname, decInfo->extn_secret_file);

	/* opening secret file */
	decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");

	/* error checking */
	if (decInfo->fptr_secret == NULL)
	{
		fprintf(stderr, "Error! The secret message file %s can't be opened\n", decInfo->secret_fname);
		return e_failure;
	}

	return e_success;
}

/* decoding secret file's size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	decode_lsb_to_int(&(decInfo->size_secret_file), decInfo);

	return e_success;
}

/* decoding secret file's data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	int idx;
	char data;
	/* loop to decode secret data */
	for (idx = 0; idx < decInfo->size_secret_file; idx++)
	{
		fread(decInfo->image_data, sizeof (char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);
		decode_lsb_to_byte(&data, decInfo->image_data);
		fputc(data, decInfo->fptr_secret);
	}	
	
	printf("Data has been decoded\n");
	return e_success;
}

/* driver function to perform decoding */
Status do_decoding(DecodeInfo *decInfo)
{
	/* opening files to decode */
	if (open_decode_files(decInfo) == e_failure)
	{
		printf("Opening failed !\n");
		return e_failure;
	}

	/* seeking to 54th byte - after header */
	fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
	
	/* decoding magic string */
	if (decode_string(MAGIC_STRING, decInfo) == e_failure)
	{
		printf("NOT a stego image\n");
		return e_failure;
	}
	/* decoding password string */
	if (decode_string(PWD_STRING, decInfo) == e_success)
	{
		printf("Password : ");
		scanf("\n%[^\n]", decInfo->pass_secret_file);
		
		/* decoding password */
		if (decode_password(decInfo) == e_failure)
		{
			printf("Wrong Password\n");
			return e_failure;
		}

		printf("Passwords match\n");

	}

	else
	{
		/*seeking behind if there's no password */
		fseek(decInfo->fptr_stego_image, -16, SEEK_CUR);
	}

	/* decoding extension for secret file */
	if (decode_secret_file_extn(decInfo) == e_failure)
	{
		printf("Failed to decode the file extension\n");
		return e_failure;
	}

	/* decoding secret file's size */
	if (decode_secret_file_size(decInfo) == e_failure)
	{
		printf("Failed to decode file size\n");
		return e_failure;
	}

	/* decoding secret data */
	if (decode_secret_file_data(decInfo) == e_failure)
	{
		printf("Failed to decode the data\n");
		return e_failure;
	}
	
	printf("Secret File created: %s\n", decInfo->secret_fname);
	/* closing the files */
	fclose(decInfo->fptr_stego_image);
	fclose(decInfo->fptr_secret);

	return e_success;
}
