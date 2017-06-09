#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "encode.h"
#include "decode.h"

/* checking operation type */
OperationType check_operation_type(char *argv[])
{
	/* checking option symbol */
	if (*argv[1] == '-')
	{
		switch (*(argv[1] + 1))
		{
			case 'e':
				return e_encode;
			case 'd':
				return e_decode;
			default:
				return e_unsupported;
		}
	}

	else
	{
		return e_unsupported;
	}
}

/* main function to perform encoding and decoding */
int main(int argc, char *argv[])
{
	/* Declaring structure variables */
	EncodeInfo encInfo;
	DecodeInfo decInfo;
	int op;

	/* reading operation type from user */
	if ((argc != 5) && (argc != 3))
	{
		printf("ERROR !\nUsage action : <a.out> <e/d> <bitmap file> <secret file> <output file>\n");
		printf("Select the operation\n1.Encode\n2.Decode\n");
		scanf("%d", &op);
	}

	else
	{
		/* checking operation type */
		op = check_operation_type(argv);
	}

	switch (op)
	{
		/* switch case to perform encoding */
		case e_encode:
			/* reading and validating arguments */
			if (read_validate_encode_args(argc, argv, &encInfo) == e_failure)
			{
				printf("Reading and validating failed\n");
				break;
			}

			/* encoding function */
			if (do_encoding(&encInfo) == e_failure)
			{
				printf("Encoding failed\n");
				break;
			}

			printf("Stegging successful !!\n");
			break;

		case e_decode:
			/* reading and validating arguments */
			if (read_validate_decode_args(argc, argv, &decInfo) == e_failure)
			{
				printf("Reading and validating failed\n");
				break;
			}

			/* decoding function */
			if (do_decoding(&decInfo) == e_failure)
			{
				printf("Decoding failed\n");
				break;
			}

			printf("Decoding Successful !!\n");
			break;

		default: printf("Unsupported operation\n");
	}

	return 0;
}
