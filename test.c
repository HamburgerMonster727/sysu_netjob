#include<string.h>
#include<stdio.h>
int main(void)
{
char string[15];
char*ptr,c='r';
strcpy(string,"Thisisastring");
ptr=strrchr(string,c);
if(ptr)
printf("%s\n",ptr);
else
printf("Thecharacterwasnotfound\n");
return 0;
}
