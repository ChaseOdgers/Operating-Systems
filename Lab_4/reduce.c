#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float getMax(int numElements, float* arr);
float getMin(int numElements, float* arr);
float getSum(int numElements, float* arr);
float getAvg(int numElements, float* arr);

int main(int argc, char* argv[]){
	char* operator = argv[1];
	
	int arraySize = 10, numElements = 0;

	float val;
	float* arr = (float*)malloc(arraySize * sizeof(float*));

	while(scanf("%f", &val) != EOF){
		if(numElements == arraySize){
			arraySize = arraySize * 2;

			arr = (float*)realloc(arr, arraySize * sizeof(float*));
		}

		arr[numElements++] = val;
	}

	if(!strcmp(operator, "max")){
		float maxVal = getMax(numElements, arr);
		printf("%f\n", maxVal);
	}else if(!strcmp(operator, "min")){
		printf("%f\n", getMin(numElements, arr));
	}else if(!strcmp(operator, "sum")){
		printf("%f\n", getSum(numElements, arr));
	}else if(!strcmp(operator, "avg")){
		printf("%f\n", getAvg(numElements, arr));
	}
}

float getMax(int numElements, float* arr){
	float max = -10000000.00;

	for(int i=0; i<numElements; i++){
		if(arr[i] > max){
			max = arr[i];
		}
	}

	return max;
}

float getMin(int numElements, float* arr){
	float min = 10000000.00;

	for(int i=0; i<numElements; i++){
		if(arr[i] < min){
			min = arr[i];
		}
	}

	return min;
}

float getSum(int numElements, float* arr){
	float sum = 0;

	for(int i=0; i<numElements; i++){
		sum = sum + arr[i];
	}

	return sum;
}

float getAvg(int numElements, float* arr){
	return getSum(numElements, arr)/numElements;
}
