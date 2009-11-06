#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

char command[40];
char* infilename = "31t0457.fts";
void main()
{

 sprintf(command,"analyse %s", infilename);
 system(command);

}
