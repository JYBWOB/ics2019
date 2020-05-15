#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char* get_ch(char *dest, int num, int n) {
	char number[20];
	for (int j=0;j<10;j++) {number[j]=j+'0';}
	number[10] = 'a'; number[11] = 'b'; number[12] = 'c'; number[13] = 'd'; number[14] = 'e'; number[15] = 'f';
	int l=0;
	int flag=0;
	if(num==0) {
		dest[0]='0';
		dest[1]='\0';
		return dest;
	}
	if(num<0) {
		flag=1;
		l++;
		num=-num;
	}
	int temp=num;
	while(temp>0) {
		l++;
		temp=temp/n;
	}
	for (int a=0; a<l; a++) {
		int x=num%n;
		assert(x<16);
		dest[l-a-1]=number[x];
		num=num/n;
	}
	if(flag) {dest[0]='-';}
	dest[l]='\0';
	return dest;
}

int printf(const char *fmt, ...) {
  va_list ap;
  	va_start(ap,fmt);
  	int sum=0;
  	int i=0;
  	int len=strlen(fmt);
	while(i<len) {
    if(fmt[i]=='%') {
      int num=0;
      char ls[50]="";
      switch(fmt[i+1]) {
        case 'd': {
	  		num=va_arg(ap,int);
	  		get_ch(ls, num, 10);
	  		for(int j = 0; j < strlen(ls); j++) {_putc(ls[j]);}
	  		sum += strlen(ls);	
			i+=2;
        } break;
		case 's': {
	  		strcpy(ls,va_arg(ap, char*));
	  		for(int j = 0; j < strlen(ls); j++) _putc(ls[j]);
	  		sum += strlen(ls);
			i+=2;
		} break;
		case '0': {
			num=va_arg(ap,int);
	  		get_ch(ls, num, 10);
			int width = fmt[i+2]-'0';
			for(int i = strlen(ls); i < width; i++) {
				sum++;
				_putc('0');
			}
	  		for(int j = 0; j < strlen(ls); j++) {_putc(ls[j]);}
	  		sum += strlen(ls);	
			i+=4;
		} break;
		case 'p': {
				num=va_arg(ap, int);
				get_ch(ls, num, 16);
				_putc('0'); _putc('x');
				sum += 2;
				for(int j = 0; j < strlen(ls); j++) _putc(ls[j]);
				sum+=strlen(ls);
				i += 2;
		} break;
		default: {
			i = len;
		}
	  } 
	}
    else {
      	_putc(fmt[i]);
		sum++;  	
      	i++;
  	}
 }
  va_end(ap);
  return sum; 
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char c;
  char *str = out;
  const char *tmp;
  char num_s[100];
  int i,j,len,num;
  int flag,field_width;

  for(;*fmt; fmt++)
  {
      if(*fmt != '%')
      {
	  *str++ = *fmt;
	  continue;
      }

      flag = 0;
      fmt++;
      while(*fmt == ' ' || *fmt == '0')
      {
	if(*fmt == ' ')  flag |= 8;
	else if(*fmt == '0') flag |= 1;
	fmt++;
      }

      field_width = 0;
      if(*fmt >= '0' && *fmt <= '9')
      {
	      while(*fmt >= '0' && *fmt <= '9')
	      {
		      field_width = field_width*10 + *fmt -'0';
		      fmt++;
	      }
      }
      else if(*fmt == '*')
      {
	      fmt++;
	      field_width = va_arg(ap,int);
      }
      switch(*fmt)
      {
	  case 's':
	      tmp = va_arg(ap,char *);
	      len = strlen(tmp);
	      for(i = 0;i < len;i ++)
	      {
		   *str++ = *tmp++;
	      }
	      continue;
	  case 'd': break;
      }

      num = va_arg(ap,int);
      j = 0;
      if(num == 0)
      {
	  num_s[j++] = '0';
      }
      else
      {
	  if(num < 0)
	  {
	      *str++ = '-';
	      num = -num;
	  }
	  while(num)
	  {
	      num_s[j++] = num%10 + '0';
	      num /= 10;
	  }
      }
      if(j < field_width)
      {
	      num = field_width - j;
	      c = flag & 1 ? '0' : ' ';
	      while(num--)
	      {
		      *str++ = c;
	      }
      }
      while(j--)
      {
	  *str++ = num_s[j];
      }
  }
  *str = '\0';
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int  val;
  va_start(args,fmt);
  val = vsprintf(out,fmt,args);
  va_end(args);
  return val;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
