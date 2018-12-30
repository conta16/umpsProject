#ifndef STRUTILS_H
#define STRUTILS_H

unsigned int strtoi();
void itostr();

unsigned int strtoi(char* s){ /*Simple conversion on string to unsigned int, -1 in case of error*/
    unsigned int a=0;
    for (; *s!=0; s++){
    if (*s<'0' || *s>'9')
        return -1;   
    a=a+(*s-'0')*10;
	}
    a/=10;
    return a;
}

void strcpy(char* str1, char* str2){
    while (*str1!=0)
        *str2=*(str1++);
}

void itostr(unsigned int a, char* dst){ /*Conversion of unsigned int to string*/
    char tmp[10];
    unsigned int c=0;
    unsigned int i;
    do{
        tmp[c]=a%10+'0';
        a/=10;
        c+=1;
        }while (a>0);
    for (i=0; i<c; i++)
        dst[i]=tmp[c-i-1];
    dst[i]=0;
}

#endif
