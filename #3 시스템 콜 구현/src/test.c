/* 20172655 LEE KANG SAN */
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main() {
	long length, num1, num2, result;
	char exp_ori[101];
	char* exp; //trim whitespace in exp_ori
	char n1[50];
	char n2[50];
	char op;
	char* tok;
	int opcheck;

	printf("< **************** + - * %% Calculator **************** >\n");
	printf("input expression as following format : (num1) (op) (num2)\n");
	printf("input character 'x' to exit                              \n");
	printf("=========================================================\n");

	while(1) {
		opcheck=0;
		printf(">> ");
		fgets(exp_ori, 100, stdin);
		exp_ori[strlen(exp_ori)-1]='\0'; //remove '\n'
		length=strlen(exp_ori);
		exp=(char*)malloc(sizeof(length));
		//remove white space in exp_ori, save trimmed string in exp
		for(int i=0, j=0; i<length; i++)
			if(exp_ori[i]!=' ') exp[j++]=exp_ori[i];
		length=strlen(exp);

		//input 'x' to exit 
		if(exp[0]=='x') {
			printf("inputed 'x', exit program.\n");
			return 0;
		}

		//find the last character of the num1 in expression
		for(int i=0; i<length; i++) {
			if('0'<=exp[i] && exp[i]<='9') {
				if(exp[i+1]=='+'||exp[i+1]=='-'||exp[i+1]=='*'||exp[i+1]=='%') {
					op=exp[i+1];
					opcheck=1; //found valid operator : +, -, *, % after num1
					exp[i+1]=' '; //replace operator with ' ' for tokenizing
					break;
				}
			}
		}

		//no operator found. wrong expression.
		if(opcheck==0) {
			printf("Wrong expression.\n");
			continue;
		}

		//tokenizing
		tok=strtok(exp, " ");
		strcpy(n1, tok);
		tok=strtok(NULL, " ");
		if(tok==NULL) {
			printf("Wrong expression.\n");
			continue;
		}
		strcpy(n2, tok);
		
		//check if n1 and n2 are a number by using atoi()
		if(0==(num1=atol(n1))) {
			if(n1[0]=='0') ;
			else { printf("num1 is not a number.\n"); continue; }
		}
		if(0==(num2=atol(n2))) {
			if(n2[0]=='0') ;
			else { printf("num2 is not a number.\n"); continue; }
		}
		
		printf("[%ld] %c [%ld] = ", num1, op, num2);
		
		switch(op)
		{
			case '+': //sys_my_add
				result=syscall(443, num1, num2);
				break;
			case '-': //sys_my_sub
				result=syscall(444, num1, num2);
				break;
			case '*': //sys_my_mul
				result=syscall(445, num1, num2);
				break;
			case '%': //sys_my_mod
				result=syscall(446, num1, num2);
				break;
		}
		printf("[%ld]\n", result);
	}
	return 0;
}
	
