/*
Attackon6rKetjeSrv2.cpp;
The program is run in Visual Studio 2010 with x64 platform Release;
The time is about 6 hours for one recovery of 1 key bits.
*/

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

typedef unsigned char UINT8;
typedef unsigned short int UINT16;
typedef unsigned long int UINT32;
typedef signed long long int INT64;

#define random(x) (rand())%x;
#define nrRounds 6
UINT32 KeccakRoundConstants[nrRounds];//these are constant,
#define nrLanes 25
unsigned int KeccakRhoOffsets[nrLanes];//these are constant,
#define nrMessage 0x8000

void KeccakPermutationOnWords(UINT16 *state);
void theta(UINT16 *A);
void rho(UINT16 *A);
void pi(UINT16 *A);
void chi(UINT16 *A);
void iota(UINT16 *A, unsigned int indexRound);



void KeccakPermutationOnWords(UINT16 *state)
{
    unsigned int i;

    for(i=0; i<nrRounds; i++) {
        theta(state);
        rho(state);
        pi(state);
        chi(state);
        iota(state, i);
    }
}


// used in theta
#define index(x, y) (((x)%5)+5*((y)%5))
//used in theta
#define ROL16(a, offset) ((offset != 0) ? ((((UINT16)a) << offset) ^ (((UINT16)a) >> (16-offset))) : a)

void theta(UINT16 *A)
{
    unsigned int x, y;
    UINT16 C[5], D[5];//C are the Xors of the five bits in every column. D are the Xors of the ten bits in right-behind column and right column

    for(x=0; x<5; x++) {
        C[x] = 0;
        for(y=0; y<5; y++)
            C[x] ^= A[index(x, y)];
    }
    for(x=0; x<5; x++)
        D[x] = ROL16(C[(x+1)%5], 1) ^ C[(x+4)%5];
    for(x=0; x<5; x++)
        for(y=0; y<5; y++)
            A[index(x, y)] ^= D[x];
}

void rho(UINT16 *A)
{
    unsigned int x, y;

    for(x=0; x<5; x++) for(y=0; y<5; y++)
        A[index(x, y)] = ROL16(A[index(x, y)], KeccakRhoOffsets[index(x, y)]%16);
}

void pi(UINT16 *A)
{
    unsigned int x, y;
    UINT16 tempA[25];

    for(x=0; x<5; x++) for(y=0; y<5; y++)
        tempA[index(x, y)] = A[index(x, y)];
    for(x=0; x<5; x++) for(y=0; y<5; y++)
        A[index(0*x+1*y, 2*x+3*y)] = tempA[index(x, y)];
}

void invpi(UINT16 *A)
{
    unsigned int x, y;
    UINT16 tempA[25];

    for(x=0; x<5; x++) for(y=0; y<5; y++)
        tempA[index(x, y)] = A[index(x, y)];
    for(x=0; x<5; x++) for(y=0; y<5; y++)
        A[index(x+3*y, x)] = tempA[index(x, y)];
}

void chi(UINT16 *A)
{
    unsigned int x, y;
    UINT16 C[5];

    for(y=0; y<5; y++) {
        for(x=0; x<5; x++)
            C[x] = A[index(x, y)] ^ ((~A[index(x+1, y)]) & A[index(x+2, y)]);
        for(x=0; x<5; x++)
            A[index(x, y)] = C[x];
    }
}

void iota(UINT16 *A, unsigned int indexRound)
{
    A[index(0, 0)] ^= KeccakRoundConstants[indexRound];
}

int LFSR86540(UINT8 *LFSR)
{
    int result = ((*LFSR) & 0x01) != 0;
    if (((*LFSR) & 0x80) != 0)
        // Primitive polynomial over GF(2): x^8+x^6+x^5+x^4+1
        (*LFSR) = ((*LFSR) << 1) ^ 0x71;
    else
        (*LFSR) <<= 1;
    return result;
}

void KeccakInitializeRoundConstants()
{
    UINT8 LFSRstate = 0x01;
    unsigned int i, j, bitPosition;

    for(i=0; i<nrRounds; i++) {
        KeccakRoundConstants[i] = 0;
        for(j=0; j<7; j++) {
            bitPosition = (1<<j)-1; //2^j-1
            if (LFSR86540(&LFSRstate))
                KeccakRoundConstants[i] ^= (UINT16)1<<bitPosition;
        }
    }
}

void KeccakInitializeRhoOffsets()
{
    unsigned int x, y, t, newX, newY;

    KeccakRhoOffsets[index(0, 0)] = 0;
    x = 1;
    y = 0;
    for(t=0; t<24; t++) {
        KeccakRhoOffsets[index(x, y)] = ((t+1)*(t+2)/2) % 32;
        newX = (0*x+1*y) % 5;
        newY = (2*x+3*y) % 5;
        x = newX;
        y = newY;
    }
}

void KeccakInitialize()
{
    KeccakInitializeRoundConstants();
    KeccakInitializeRhoOffsets();
}



int main()
{
	clock_t start,finish;
	start = clock();
    UINT16 InitialState[25]={0};
	UINT16 TempState[25]={0};
	UINT16 FinalState[2]={0};
	UINT16 Key[9]={0};
	int tempkey=0;
	int guessedkey=2;

	INT64 i,j,k,temp,temp1;

	unsigned int rightkey={0};
	unsigned int index[56][2];

	index[0][0]=2;index[0][1]=0;
	index[1][0]=7;index[1][1]=0;
	index[2][0]=22;index[2][1]=1;
	index[3][0]=17;index[3][1]=1;
	index[4][0]=11;index[4][1]=12;
	index[5][0]=21;index[5][1]=12;
	index[6][0]=4;index[6][1]=1;
	index[7][0]=5;index[7][1]=0;
	index[8][0]=20;index[8][1]=0;
	index[9][0]=5;index[9][1]=1;
	index[10][0]=15;index[10][1]=1;
	index[11][0]=20;index[11][1]=1;
	index[12][0]=5;index[12][1]=7;
	index[13][0]=15;index[13][1]=7;
	index[14][0]=20;index[14][1]=7;
	index[15][0]=15;index[15][1]=8;
	index[16][0]=20;index[16][1]=8;
	index[17][0]=5;index[17][1]=10;
	index[18][0]=15;index[18][1]=10;
	index[19][0]=20;index[19][1]=10;
	index[20][0]=5;index[20][1]=11;
	index[21][0]=15;index[21][1]=11;
	index[22][0]=20;index[22][1]=11;
	index[23][0]=5;index[23][1]=15;
	index[24][0]=15;index[24][1]=15;
	index[25][0]=20;index[25][1]=15;
	index[26][0]=1;index[26][1]=7;
	index[27][0]=21;index[27][1]=7;
	index[28][0]=1;index[28][1]=14;
	index[29][0]=11;index[29][1]=14;
	index[30][0]=2;index[30][1]=2;
	index[31][0]=17;index[31][1]=2;
	index[32][0]=22;index[32][1]=2;
	index[33][0]=2;index[33][1]=3;
	index[34][0]=7;index[34][1]=3;
	index[35][0]=17;index[35][1]=3;
	index[36][0]=22;index[36][1]=3;
	index[37][0]=2;index[37][1]=4;
	index[38][0]=17;index[38][1]=4;
	index[39][0]=22;index[39][1]=4;
	index[40][0]=2;index[40][1]=6;
	index[41][0]=7;index[41][1]=6;
	index[42][0]=17;index[42][1]=6;
	index[43][0]=22;index[43][1]=6;
	index[44][0]=2;index[44][1]=9;
	index[45][0]=22;index[45][1]=9;
	index[46][0]=7;index[46][1]=11;
	index[47][0]=22;index[47][1]=11;
	index[48][0]=8;index[48][1]=12;
	index[49][0]=13;index[49][1]=12;
	index[50][0]=23;index[50][1]=12;
	index[51][0]=8;index[51][1]=15;
	index[52][0]=13;index[52][1]=15;
	index[53][0]=4;index[53][1]=7;
	index[54][0]=14;index[54][1]=7;
	index[55][0]=19;index[55][1]=7;


	FILE *f;
	f=fopen("result.txt","w+b");
	srand(200);
	fprintf(f,"128-bit key:\n");

	for(i=0;i<9;i++){
		Key[i]=0;
	}
	unsigned int cnt=0;
	for(i=8;i<16;i++){
		temp=random(2);
		if(temp){
			Key[0]^=((UINT16)0x1<<i);
			fprintf(f,"1");
		}
		else{
			fprintf(f,"0");
		}
		cnt++;
	}
	Key[0]^=((UINT16)0x1<<1);
	Key[0]^=((UINT16)0x1<<4);
	
	for(i=1;i<8;i++){
		for(j=0;j<16;j++){
			temp=random(2);
			if(cnt==34||cnt==65||cnt==82||cnt==97){
				printf("%d ",temp);
			}
			if(temp)
			{
				Key[i]^=((UINT16)0x1<<j);
				fprintf(f,"1");
			}
			else{
				fprintf(f,"0");
			}
			cnt++;
			if(cnt==64)
				fprintf(f,"\n");
		}
	}
	for(j=0;j<8;j++){
		temp=random(2);
		if(temp)
		{
			Key[i]^=((UINT16)0x1<<j);
			fprintf(f,"1");
		}
		else{
			fprintf(f,"0");
		}
		cnt++;
	}
	Key[8]^=((UINT16)0x1<<8);
	fprintf(f,"\n");


	
	for(i=0;i<25;i++){
		InitialState[i]=0;
	}
	int ink[9]={0,6,12,18,24,3,9,10,16};
	for(i=0;i<9;i++){
		InitialState[ink[i]]=Key[i];
	}

	
	InitialState[21]|=((UINT16)0x1<<14);
	InitialState[21]|=((UINT16)0x1<<15);





	//k(34) + k(65) + k(82) + k(97)
	printf("\n%d %d %d %d\n",(Key[2]>>10)&1,(Key[4]>>9)&1,(Key[5]>>10)&1,(Key[6]>>9)&1);
	rightkey=((Key[2]>>10)&1)^((Key[4]>>9)&1)^((Key[5]>>10)&1)^((Key[6]>>9)&1);                       
	


	fprintf(f,"right key:");
	printf("right key:");

	if(rightkey)
	{
		fprintf(f,"1");
		printf("1");
	}
	else
	{
		fprintf(f,"0");
		printf("0");
	}
	
	fprintf(f,"\n");
	printf("\n");

	KeccakInitialize();



	for(i=0;i<2;i++)
	{
		for(j=0;j<2;j++){
			FinalState[j]=0;
		}	
		
		fprintf(f,"guessed value:");
		printf("guessed value:");
		if(i)
		{
			tempkey=1;
			fprintf(f,"1");
			printf("1");
		}
		else
		{
			tempkey=0;
			fprintf(f,"0");
			printf("0");
		}
		fprintf(f,", ");
		printf(", ");
		temp=(InitialState[22]>>10)&1;
		temp1=(tempkey&1);
		if(temp!=temp1)
		{
			InitialState[22]^=(UINT16)0x1<<10;
		}
		for(j=0;j<(INT64(1)<<33);j++)//initialize tempstate
			//33,20,3s,both nonzero
		{
			for(k=0;k<25;k++)//fresh the tempstate for the key and initial value
			{
				TempState[k]=InitialState[k];
			}
			//give the value for cube variables
			UINT16 cubev[33]={0};
			UINT16 statev[56]={0};
			for(k=0;k<33;k++)
			{
				cubev[k]=(j>>k)&1;	
			}
			statev[0]=cubev[0];
			statev[1]=cubev[0];
			statev[2]=cubev[0];
			statev[3]=cubev[0];
			statev[4]=cubev[1];
			statev[5]=cubev[1];
			statev[6]=cubev[1];
			statev[7]=cubev[2];
			statev[8]=cubev[2];
			statev[9]=cubev[3];
			statev[10]=cubev[4];
			statev[11]=(cubev[3]^cubev[4])&1;
			statev[12]=cubev[5];
			statev[13]=cubev[6];
			statev[14]=(cubev[5]^cubev[6])&1;
			statev[15]=cubev[7];
			statev[16]=cubev[7];
			statev[17]=cubev[8];
			statev[18]=cubev[9];
			statev[19]=(cubev[8]^cubev[9])&1;
			statev[20]=cubev[10];
			statev[21]=cubev[11];
			statev[22]=(cubev[10]^cubev[11])&1;
			statev[23]=cubev[12];
			statev[24]=cubev[13];
			statev[25]=(cubev[12]^cubev[13])&1;
			statev[26]=cubev[14];
			statev[27]=cubev[14];
			statev[28]=cubev[15];
			statev[29]=cubev[15];
			statev[30]=cubev[16];
			statev[31]=cubev[17];
			statev[32]=(cubev[16]^cubev[17])&1;
			statev[33]=cubev[18];
			statev[34]=cubev[19];
			statev[35]=cubev[20];
			statev[36]=(cubev[18]^cubev[19]^cubev[20])&1;
			//if(statev[36])
				//printf("%d %d %d %d\n",cubev[18],cubev[19],cubev[20],statev[36]);
			statev[37]=cubev[21];
			statev[38]=cubev[22];
			statev[39]=cubev[21]^cubev[22];
			statev[40]=cubev[23];
			statev[41]=cubev[24];
			statev[42]=cubev[25];
			statev[43]=cubev[23]^cubev[24]^cubev[25];
			statev[44]=cubev[26];
			statev[45]=cubev[26];
			statev[46]=cubev[27];
			statev[47]=cubev[27];
			statev[48]=cubev[28];
			statev[49]=cubev[29];
			statev[50]=cubev[28]^cubev[29];
			statev[51]=cubev[30];
			statev[52]=cubev[30];
			statev[53]=cubev[31];
			statev[54]=cubev[32];
			statev[55]=cubev[31]^cubev[32];
			/*for(k=0;k<56;k++)
			{
				printf("%d:%d ",k,statev[k]);
			}
			printf("\n");*/
			for(k=0;k<56;k++)
			{
				if(statev[k]){
					TempState[index[k][0]]|=UINT16(1)<<index[k][1];
				}
				else{
					TempState[index[k][0]]&=(~(UINT16(1)<<index[k][1]));
				}
			}
			KeccakPermutationOnWords((UINT16 *)TempState);
			for(k=0;k<2;k++){
				FinalState[k]^=TempState[6*k];
			}
		}
		fprintf(f,"0x%x, 0x%x\n",FinalState[0],FinalState[1]);
		printf("0x%x, 0x%x\n",FinalState[0],FinalState[1]);
		if(FinalState[0]==0 && FinalState[1]==0)
		{
			guessedkey=tempkey;
		}
	}

	fprintf(f,"guess key:");
	printf("guess key:");
	fprintf(f,"%d",guessedkey);
	printf("%d",guessedkey);
	printf("\n");

	fclose(f);
	finish = clock();
	printf("time=%f\n",(double)(finish-start)/CLK_TCK);
	printf("Done!\n");
    getch();
    return 0;
}
