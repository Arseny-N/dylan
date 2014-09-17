#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *arr_type = "static const unsigned char", *arr_name = "webpage", 
	     *len_type = "static const unsigned int",  *len_name = "webpage_len";
	unsigned int llen = -1, i, arr_len;
	FILE *out = stdout, *in = stdin;
	fprintf(out, "%s %s[] = { \n\t", arr_type, arr_name);
	
	for(;;) {
		int ch = getc(in);
		if(ch == EOF)
			break;
		fprintf(out,"%ld,",(long) ch );
		
		if(i++ == llen) {
			i = 0;
			printf("\n\t");
		}
		arr_len ++;
	}
	
	fprintf(out,"\n};\n");
	fprintf(out,"%s %s = %ld;", len_type, len_name, arr_len);
	exit(EXIT_SUCCESS);
}
