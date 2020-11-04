#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char* argv[]){
	char* operator = argv[1];
	float operand = atof(argv[2]);

	float val;

	while(scanf("%f", &val) != EOF){
		if(!strcmp(operator, "+")){
			printf("%f\n", (val + operand));
		}else if(!strcmp(operator, "-")){
			printf("%f\n", (val - operand));
		}else if(!strcmp(operator, "*")){	
			printf("%f\n", (val * operand));
		}else if(!strcmp(operator, "/")){
			printf("%f\n", (val / operand));
		}else if(!strcmp(operator, "**")){
			printf("%f\n", pow(val, operand));
		}
	}

	return 0;
}
