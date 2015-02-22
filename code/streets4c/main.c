#include <stdio.h>
#include <stdlib.h>

#include "osmpbf/osmpbf.h"
#include "osmpbf/fileformat.pb-c.h"

static size_t read_buffer (unsigned max_length, uint8_t *out, FILE *file)
{
  size_t cur_len = 0;
  size_t nread;
  while ((nread=fread(out + cur_len, 1, max_length - cur_len, file)) != 0)
  {
    cur_len += nread;
    if (cur_len == max_length)
    {
      fprintf(stderr, "max message length exceeded\n");
      exit(1);
    }
  }
  return cur_len;
}

int main(int argc, char** argv)
{
	printf("This is streets4c\n");

	char *file_name = argv[1];
	printf("Opening file %s\n", file_name);
	FILE *file = fopen(file_name, "r");

	OSMPBF__Blob *blob;

	uint8_t buf[max_blob_header_size];
  	size_t msg_len = read_buffer (max_blob_header_size, buf, file);

	//blob = osmpbf__blob__unpack(NULL, msg_len, buf);

	if (blob == NULL)
	{
		printf("error unpacking message\n");

		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
