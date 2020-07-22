#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/***********************************************************************/

void toHex(int IEEE[32]){
	int sum;
	
	printf("\n*** IEEE HEX: ");
	for(int c=0; c<29; c+=4){
		sum=0;
		if(c==0 || c%4==0){
			sum=(IEEE[c]*8)+(IEEE[c+1]*4)+(IEEE[c+2]*2)+(IEEE[c+3]*1);
		}
		printf("%X", sum);
	}
	printf("\n");
}
int* getExp(float val){
	float absValue=val;
	int exp=0, expMod=-1, size=8;
	static int expArray[8];
	
	while(absValue>=2){
		absValue/=2;
		exp++;
	}
	while(absValue<1){
		absValue*=2;
		exp--;
	}
	if(exp>=0) exp+=127;
	else exp-=127;		
	for(int i=0; i<8; i++){
		expMod=exp%2;
		exp/=2;
		expArray[--size] = expMod;
	}
	printf("\n*** Biased Exponent: ");
	for(int c=0; c<8; c++){
		printf("%d", expArray[c]);
	}
	return expArray;
}
int* toBinary(int intOnly){
	static int binaryArray[8];
	int binary=128;
	
	//	Initialize binaryArray
	for(int i=0; i<8; i++)
		binaryArray[i]=0;
	//	Converts integer to binary
	for(int p=0; p<8; p++){
		if(intOnly%binary==intOnly)
			binaryArray[p]=0;
		else if(intOnly%binary==0 || intOnly%binary>0){
			binaryArray[p]=1;
			intOnly-=binary;
		}
		else
			binaryArray[p]=0;
		binary/=2;
	}
	return binaryArray;
}
int* getSig(float wholeN){
	int wholeNum=wholeN, decIndex=8, oneIndex=0, sigSize=23;
	float decimal=wholeN-wholeNum;
	static int significand[23];
	int* intNum=toBinary(wholeNum);		//	Converts whole number to binary

	//	Initilizes all significand array
	for(int i=0; i<sigSize; i++){
		if(i<8)
			significand[i]=intNum[i];
		else
			significand[i]=0;
	}
	
	do{
		decimal*=2;
		if(decimal<1)
			significand[decIndex++]=0;
		else{
			decimal-=1;
			significand[decIndex++]=1;
		}
	}
	while(decimal!=0);
	//	Searches for the hidden 1
	for(int a=0; a<sigSize; a++){
		if(significand[a]==1){
			oneIndex=++a;
			break;
		}
	}
	int index=0;
	for(int d=oneIndex; d<sigSize; d++){
		significand[index++]=significand[d];
	}
	//	Prints mantissa
	printf("\n*** Mantissa: ");
	for(int t=0; t<sigSize; t++){
		printf("%d", significand[t]);
	}
	return significand;
}
void toIEEE (){
	float input;
	int sign=-1, binary[32];
	printf("\nEnter the decimal representation:");
	scanf("%f", &input);
	if(input!=0){
		if(input>=0) sign=0;
		else sign=1;		
		printf("\n*** Sign: %d", sign);			
		
		int* exponent=getExp(fabsf(input));
		int* mantissa=getSig(fabsf(input));
		//	Initializes binary array to send to toHex function
		binary[0]=sign;
		int m=0;
		for(int i=1; i<32; i++){
			if(i<9)
				binary[i]=exponent[i-1];
			else
				binary[i]=mantissa[m++];
		}
		toHex(binary);
	}
	else{
		printf("\n*** Sign: 0\n*** Biased exponent: 00000000\n*** Mantissa: 00000000000000000000000\n");
		printf("*** The IEEE-754 representation is: 0.000000\n");
	}
}
/*
	Param:	string, string size
	Brief:	converts hex string to binary
	Return:	binary array
*/
int* hexToBinary(char hex[], int hexSize){
	static int bin[32];
	int binIndex=0, check=0;

	for(int a=0; a<32; a++)
		bin[a]=0;
	for(int i=0; i<hexSize; i++){
		int hexDec=hex[i]-'0';
		if(hexDec<10)
			check=0;
		else
			check=7;			
		int* converted=toBinary(hexDec-check);
		for(int b=4; b<8; b++){
			if(b%4!=0 || b!=0)
				bin[binIndex++]=converted[b];
		}		
	}	
	return bin;
}
/*	Param:	user inputted string
	Brief:	checks for sample special cases
	Return:	0 - nonspecial case
			1 - special case
*/
int getSpecial(char checkArray[]){
	int special=0;
	if(strcmp(checkArray, "-126")==0){
		printf("\n*** Sign: -\n*** Special Case: NaN\n");
		special=1;
	}
	if(strcmp(checkArray, "-0")==0){
		printf("\n*** Sign: -\n*** Special Case: -0\n");
		special=1;
	}
	if(strcmp(checkArray, "0")==0){
		printf("\n*** Sign: +\n*** Special Case: 0\n");
		special=1;
	}
	if(strcmp(checkArray, "NaN")==0){
		printf("\n*** Sign: -\n*** Special Case: NaN\n");
		special=1;
	}
	if(strcmp(checkArray, "+infinity")==0 || strcmp(checkArray, "+in")==0){
		printf("\n*** Sign: +\n*** Special Case: +Infinity\n");
		special=1;
	}
	if(strcmp(checkArray, "-infinity")==0 || strcmp(checkArray, "-in")==0){
		printf("\n*** Sign: -\n*** Special Case: -Infinity\n");
		special=1;
	}
	return special;
}
/*	Param:	binary array and array size
	Brief:	converts binary array into an decimal value
	Return:	converted decimal value
*/	
int binToDecimal(int binArray[], int size){
	int decSize=size/sizeof(int);
	int decimalNum[decSize];
	int binary=pow(2,size-1);
	int sum=0;

	for(int i=0; i<size; i++){
		sum+=binArray[i]*binary;
		binary/=2;
	}
	return sum;
}
float getFraction(int binArray[23]){
	int power=-1;
	float fraction=0, binary=0;
	for(int i=0; i<23; i++){
		binary=pow(2, power--);
		fraction+=binArray[i]*binary;
	}
	return fraction;
}
//	Prompts user for IEEE-754 hexadecimal input and converts to decimal
void toDecimal (){
	char temp[9] = {'0','0','0','0','0','0','0','\0'};
	int binExp[8], binSig[23];
	int nullIndex=9, sigIndex=0;;
	char sign;

	for(int b=0; b<23; b++)
		binSig[b]=0;
	printf("\nEnter the IEEE-754 representation:");
	scanf("%s", temp);
	for(int i=0; i<9; i++){
		if(temp[i]=='\0'){
			nullIndex=i;
			break;
		}
	}
	//	Creates an array with the exact string size and finds null value
	char hexArray[nullIndex];
	hexArray[nullIndex]='\0';
	//	Copies the exact string into a new array
	for(int c=0; c<nullIndex; c++)
		hexArray[c]=temp[c];
	//	Checks for sample special cases and if not converts regularly
	if(getSpecial(hexArray)==0){
		int* hexConverted=hexToBinary(hexArray, nullIndex);
		if(hexConverted[0]==0)
			sign='+';
		else
			sign='-';
		for(int a=1; a<32; a++){
			if(a<9)
				binExp[a-1]=hexConverted[a];
			else
			 	binSig[sigIndex++]=hexConverted[a];
		}
		int biasExp=binToDecimal(binExp, 8);
		float fracDec=getFraction(binSig);
		int checkFrac=fracDec;
		if(biasExp==255 && fracDec!=0){
			char special[4]="NaN";
			getSpecial(special);
		}
		else if(biasExp==255 && fracDec==0){
			if(sign=='-'){
				char special[4]="-in";
				getSpecial(special);
			}
			else{
				char special[4]="+in";
				getSpecial(special);		
			}	
		}
		else if(biasExp==0){
			printf("\n%f*2^(-126)", fracDec);
		}
		else{
			int unbiasExp=biasExp-127;
			printf("\n*** Sign: %c", sign);
			printf("\n*** Unbiased exponent: %d", unbiasExp);
			fracDec+=1;
			printf("\n*** Normalized decimal: %f", fracDec);
			if(sign=='-')
				fracDec*=-1;
			printf("\n*** Decimal: %f\n", fracDec*pow(2, unbiasExp));			
		}
	}
}
void printMenu() {
	printf("Floating-point conversion:\n");
	printf("1) Decimal to IEEE-754 conversion\n2) IEEE-754 to Decimal conversion\n3) Exit\n");
	printf("Enter selection:");
}
int main()
{
  int selection;
  do{
  		printMenu();
		scanf("%d", &selection);
		if(selection==1)
			toIEEE();
		else if(selection==2)
			toDecimal();
  }
  while(selection!=3); 
  printf("\n*** Program Terminated Normally");
  return 0;
}