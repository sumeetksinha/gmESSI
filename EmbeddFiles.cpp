// Copyright (c) Sergey Lyubka, 2013.
// All rights reserved.
//
// Taken from :- https://gist.github.com/cpq/4714740#file-embed-c
// This program is used to embed arbitrary data into a C binary. It takes
// a list of files as an input, and produces a .c data file that contains
// contents of all these files as collection of char arrays.
// Usage:
//   1. Compile this file:
//      cc -o embed embed.c
//
//   2. Convert list of files into single .c:
//      ./embed file1.data file2.data > embedded_data.c
//
//   3. In your application code, you can access files using this function:
//
//      const char *find_embedded_file(const char *file_name, size_t *size);
//      size_t size;
//      const char *data = find_embedded_file("file1.data", &size);
//
//   4. Build your app with embedded_data.c:
//      cc -o my_app my_app.c embedded_data.c

#include <stdio.h>
#include <stdlib.h>

static const char *header =
	"/*********************************************************************************************************\n"
	"*  File:        Embedded.h                              | Copyright:: Version of Creative Commons License*\n"
	"*  Description: For Embedding Files in Executable       | Boris Jeremic,                                 *\n"
	"*  Rev:         Version 1                               | University of California Davis                 *\n"
	"*  Created:     June 28, 2015                           | California                                     *\n"
	"*  Author:      Sumeet Kumar Sinha                      | jeremic@ucdavis.edu                            *\n"
	"*  Email:       sumeet.kumar507@gmail.com               |                                                *\n"
	"*                                                                                                        *\n"
	"*  The copyright to the computer program(s) herein is the property of Sumeet Kumar Sinha, Boris Jeremic  *\n"
	"*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha       *\n"
	"*  or in accordance with the terms and conditions stipulated in the agreement/contract under which       *\n"
	"*  the program(s) have been supplied.                                                                    *\n"
	"*********************************************************************************************************/\n"
	"\n"
	"#include \"Embedded.h\"\n"
	"using namespace::std;\n"
	"\n";

static const char *middlecode =
	"const char *find_embedded_file(const char *name, size_t *size) {\n"
	"  const struct embedded_file *p;\n"
	"  for (p = embedded_files; p->name != NULL; p++) {\n"
	"    if (!strcmp(p->name, name)) {\n"
	"      if (size != NULL) { *size = p->size; }\n"
	"      return (const char *) p->data;\n"
	"    }\n"
	"  }\n"
	"  return NULL;\n"
	"}\n"
	"\n";

static const char *endpart =
	"/******************************************************************************\n"
	"****************************** Constructor ************************************\n"
	"******************************************************************************/\n"
	"\n"
	"Embedded::Embedded(){}\n"
	"\n"
	"Embedded::~Embedded(){}\n"
	"\n"
	"/******************************************************************************\n"
	"**************************** Public Functions *********************************\n"
	"******************************************************************************/\n"
	"\n"
	"const char* Embedded::ReadFile(string filename){\n"
	"\n"
	"  const char* sourceFileName = filename.c_str();\n"
	"    size_t *size = NULL;\n"
	"    const char* filecontent= find_embedded_file(sourceFileName,size);\n"
	"    return filecontent;\n"
	"}\n";


int main(int argc, char *argv[]) {
  FILE *fp;
  int i, j, ch;

  printf("%s", header);
  for (i = 1; i < argc; i++) {
    if ((fp = fopen(argv[i], "rb")) == NULL) {
      exit(EXIT_FAILURE);
    } else {
      printf("static const unsigned char v%d[] = {", i);
      for (j = 0; (ch = fgetc(fp)) != EOF; j++) {
        if ((j % 12) == 0) {
          printf("%s", "\n ");
        }
        printf(" %#04x,", ch);
      }
      // Append zero byte at the end, to make text files appear in memory
      // as nul-terminated strings.
      printf("%s", " 0x00\n};\n");
      fclose(fp);
    }
  }

  printf("%s", "\nconst struct embedded_file {\n");
  printf("%s", "  const char *name;\n");
  printf("%s", "  const unsigned char *data;\n");
  printf("%s", "  size_t size;\n");
  printf("%s", "} embedded_files[] = {\n");

  for (i = 1; i < argc; i++) {
    printf("  {\"%s\", v%d, sizeof(v%d) - 1},\n", argv[i], i, i);
  }
  printf("%s", "  {NULL, NULL, 0}\n");
  printf("%s", "};\n\n");
  printf("%s", middlecode);
  printf("%s", endpart);

  return EXIT_SUCCESS;
}