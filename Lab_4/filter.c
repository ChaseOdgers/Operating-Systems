#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	char* comparingOperator = argv[1];
	float comparingOperand = atof(argv[2]);

	float val;

	while(scanf("%f", &val) != EOF){
		if(!strcmp(comparingOperator, "<") && (val < 
comparingOperand)){
			printf("%f\n", val);
		}else if(!strcmp(comparingOperator, "<=") && (val <= comparingOperand)){
			printf("%f\n", val);			
		}else if(!strcmp(comparingOperator, ">") && (val > comparingOperand)){
			printf("%f\n", val);
		}else if(!strcmp(comparingOperator, ">=") && (val >= comparingOperand)){
			printf("%f\n", val);
		}else if(!strcmp(comparingOperator, "==") && (val == comparingOperand)){
			printf("%f\n", val);
		}else if(!strcmp(comparingOperator, "!=") && (val != comparingOperand)){
			printf("%f\n", val);
		}else{

		}
	}

	return 0;
}
