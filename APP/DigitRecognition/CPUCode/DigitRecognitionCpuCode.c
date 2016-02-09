#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "Maxfiles.h"

const int X = DigitRecognition_X; // image size in pixels
const int Y = DigitRecognition_Y; // number of test images
const int C = DigitRecognition_CFactor; // C Factor used in the DFE
const int REF_NUM = DigitRecognition_REF_NUM; // number of reference pictures (artificially made to be 12 instead of 10 (check DigitRecognitionManager.maxj to see))
int REF_SKART = 12 % 10; // number of reference pictures that are of no interest (related to REF_NUM problem)

int big2littleEndian(int big){
	int little = (big << 24) + ((big & 0xff00) << 8) + ((big & 0xff0000) >> 8) + ((big >> 24) & 0xff);
	return little;
}

int loadInputData(char* filename, float **inputTransposed)
{
	// Open file for reading
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		fprintf(stderr, "Error opening file %s.", filename);
		exit(1);
	}

	// Read file header
	int picNum, picX, picY;
	fread(&picNum, sizeof(int), 1, file); // magic number - NOT important
	fread(&picNum, sizeof(int), 1, file); picNum = big2littleEndian(picNum); // number of pictures - important
	fread(&picX, sizeof(int), 1, file); picX = big2littleEndian(picX); // picture height - importanat
	fread(&picY, sizeof(int), 1, file); picY = big2littleEndian(picY); // picture width - important

	// Check if header is ok
	if(picX * picY != X || picNum < Y){
		printf("***error in generateInputData()\n");
		fclose(file);
		return 1;
	}

	// Read gray level of each pixel
	const int size = X * Y;
	int sizeBytes = size * sizeof(float);
	float *input = malloc(sizeBytes);
	int i = 0;
	char ch = '0';
	while(i < size){
		fread(&ch, sizeof(char), 1, file);
		input[i] = (float)(ch & 0xff) / 255.0f;
		i++;
	}

	// Transpose the inputs
	(*inputTransposed) = malloc(sizeBytes);
	int count = 0;
	for (int yy = 0; yy < Y; yy += C) {
		for (int x = 0; x < X; ++x) {
			for (int y = yy; y < yy + C; ++y) {
				(*inputTransposed)[count] = input[y * X + x];
				count++;
			}
		}
	}

	// Release resources
	free(input);
	fclose(file);

	printf("Input Data - Loaded\n");

	return 0;
}

void loadWeights(char* filename, double** rom){

	// Allocate space for weights
	int size = X * REF_NUM;
	int sizeBytes = size * sizeof(double);
	(*rom) = malloc(sizeBytes);

	// Open file for reading
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file %s.", filename);
		exit(1);
	}
	// Read file
	char pom[20];
	float weights[10];
	int started = 0;
	int pixel = 0;
	while(!feof(fp)){
		if(started == 0){
			// reading: weights
			fscanf(fp, "%s", pom);
			// reading: [[
			fscanf(fp, "%s", pom);
			started = 1;
		}
		else{ // reading: \n_[ (if there are more hot vectors)
			  // reading: ] (if there aren't any hot vectors left)
			int ch;
			ch = fgetc(fp);
			if(ch != ']'){
				fgetc(fp); // reading: _ (space)
				fgetc(fp); // reading: [
			}
			else{
				break;
			}
		}

		for(int i=0; i< REF_NUM - REF_SKART; i++){
			fscanf(fp, "%f", &(weights[i]));
			weights[i] *= 1000.00;
			(*rom)[i * X + pixel] = (double)(weights[i]/1000.00);
		}
		pixel++;
		fgetc(fp); // reading: ]
	}
	fclose(fp);

	// adding artificial weights
	for(int i = REF_NUM - REF_SKART; i< REF_NUM; i++){
		for(int j=0; j<pixel; j++){
			(*rom)[i * X + j] = 0.0;
		}
	}

	printf("Weights - Loaded\n");
}

void loadBiases(char* filename, double** biases){
	FILE * fp;
	fp = fopen (filename, "r");
	char pom[20];
	(*biases) = malloc(REF_NUM * sizeof(double));

	// reading: biases
	fscanf(fp, "%s", pom);
	// reading: [
	fscanf(fp, "%s", pom);

	for(int i=0; i<REF_NUM - REF_SKART; i++){
		fscanf(fp, "%lf", &((*biases)[i]));
	}
	// not reading: ]
	fclose(fp);

	// adding artificial biases
	for(int i=REF_NUM - REF_SKART; i<REF_NUM; i++){
		(*biases)[i] = 0.0;
	}

	printf("Biases - Loaded\n");
}

void loadLabels(char* filename, int** labels){
	int pic_num = Y;

	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		fprintf(stderr, "Error opening file %s.", filename);
		exit(1);
	}

	int labels_num;
	fread(&labels_num, 4, 1, file); // magic number - not important
	fread(&labels_num, 4, 1, file); // number of labels - important
	labels_num = big2littleEndian(labels_num);

	// check if data is ok
	if(labels_num < pic_num){
		printf("Labels are wrong!\n");
		fclose(file);
		return;
	}

	labels_num = pic_num;
	// allocate space for file contents
	int labelBytes = sizeof(int) * labels_num;
	(*labels) = malloc(labelBytes);

	// read gray level of each pixel
	int i = 0;
	char result;
	while(i < labels_num){
		fread(&result, 1, 1, file);
		(*labels)[i] = (result & 0x000000ff);
		i++;
	}

	fclose(file);
	printf("Labels - Loaded\n");
}

// HARDCODED NUMBER 28 (28x28 is picture size in pixels)
void writeRom(double * rom){
	for(int z = 0; z < REF_NUM; z++){
		printf("\nSlika %d\n", z);
		for(int y = 0; y < 28; y++){
			for(int x = 0; x < 28; x++){
				printf("%f ", rom[z * 28 * 28 + y * 28 + x]);
			}
			printf("\n");
		}
	}
}

void writeBiases(double* biases){
	printf("\nbiases\n");
	for(int i=0; i<REF_NUM; i++){
		printf("%lf ", biases[i]);
	}
}

void DigitRecognitionCPU(float *input, double *rom, float *output, double* biases)
{
	int count = 0;
	for (int yy=0; count < X*Y; yy += C) {
	  for (int x=0; x<X; x += 1) {
	    for (int y=yy; y < yy+C; y += 1) {
	      if (x == 0) {
	    	  for(int k=0; k<REF_NUM; k++)
	    		  output[y * REF_NUM + k] = biases[k];
	      }
	      for(int k=0; k<REF_NUM; k++){
	    	  output[y * REF_NUM + k] += input[count] * rom[k * X + x];
	      }
	      count++;
	    }
	  }
	}
}

void writeCPUOutput(float* output){
	printf("CPUOutput:\n");
	for(int i=0; i<10; i++){
		printf("%d. %lf\n", i, output[i]);
	}
}


void check(float *output, float *expected)
{
	int status = 0;
	for (int i = 0; i < Y * REF_NUM; i++) {
		if (fabs(output[i] - expected[i]) > 1) {
			fprintf(stderr, "[%d] error, output: %f != expected: %f\n",
				i, output[i], expected[i]);
			status = 1;
		}
	}

	if(status == 0){
		printf("DFE - Success.\n");
	}
	else{
		printf("DFE - Fail.\n");
	}
}

void checkFinal(int* labels, int* result){
	int rightGuess = 0;
		for (int i = 0; i < Y; i++) {
			if (labels[i] != result[i]) {
				//fprintf(stderr, "[%d] error, result: %d != label: %d\n",
					//i, result[i], labels[i]);
			}
			else{
				rightGuess++;
			}
		}

		printf("Final Result: %d/%d\n", rightGuess, Y);
}

void convertToSoftmax(float* rawOutput){
	float* devider = malloc(Y * sizeof(float));

	// create devider
	for(int j = 0; j < Y; j++){
		devider[j] = 0.0;
		for(int i=0; i < REF_NUM - REF_SKART; i++){
			devider[j] += exp(rawOutput[j * REF_NUM + i]);
		}
	}

	// create softmax probabilities
	for(int j = 0; j < Y; j++){
		for(int i=0; i < REF_NUM - REF_SKART; i++){
			float dividend = exp(rawOutput[j * REF_NUM + i]);
			rawOutput[j * REF_NUM + i] = (float)(dividend / devider[j]);
		}
	}
}

void extractResult(float* rawInput, int* output){

	// converting to softmax probabilities
	convertToSoftmax(rawInput);

	// choosing max values from raw output
	float *max = malloc(sizeof(float) * Y);
	for(int i=0; i < Y; i++){
		max[i] = FLT_MIN;
		output[i] = -1;
		for(int z = 0; z < REF_NUM - REF_SKART; z++){
			float next = rawInput[i * REF_NUM + z];
			if(max[i] < next){
				max[i] = next;
				output[i] = z;
			}
		}
	}

	free(max);

	printf("Running Softmax Layer on CPU...\n");
}

void writeFinalResult(int* result){
	printf("\nfinal result\n");
	for(int i=0; i<Y; i++){
		printf("%d ", result[i]);
	}
}

int main()
{
	double *rom;
	double *biases;
	float *inputTranspose;
	int *labels;

	int inputSize = X * Y;
	int outputSize = REF_NUM * Y;
	int outputSizeBytes = outputSize * sizeof(float);
	float* output = malloc(outputSizeBytes);
	float* expected = malloc(outputSizeBytes);

	int *result = malloc(Y * sizeof(int));

	loadWeights("../EngineCode/src/files/weights", &rom);
	loadBiases("../EngineCode/src/files/biases", &biases);
	loadLabels("../EngineCode/src/files/labels", &labels);
	loadInputData("../EngineCode/src/files/grayTests", &inputTranspose);
	//writeRom(rom);
	//writeBiases(biases);

	// run on DFE
	printf("Running Weight Calculation on DFE...\n");
	clock_t start = clock();
	DigitRecognition(inputSize, outputSize, inputTranspose, output, biases, rom);
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("DFE finished in %f.\n", seconds);


	// run on CPU
	printf("Running Weight Calculation on CPU...\n");
	start = clock();
	DigitRecognitionCPU(inputTranspose, rom, expected, biases);
	//writeCPUOutput(expected);
	end = clock();
	seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("CPU finished in %f\n", seconds);

	// checking results of CPU and DFE
	//check(output, expected);

	// extracting results from raw output (that came from DFE)
	extractResult(output, result);
	// writeFinalResult(result);
	checkFinal(labels, result);

	// freeing memory
	free(rom);
	free(biases);
	free(inputTranspose);
	free(labels);
	free(output);
	free(expected);
	free(result);

	return 0;
}
