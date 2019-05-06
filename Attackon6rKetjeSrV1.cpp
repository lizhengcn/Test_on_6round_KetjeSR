/*
Attackon6rKetjeSrV1.cpp;
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
	unsigned int index[57][2];

	index[0][0]=10;index[0][1]=0;
	index[1][0]=20;index[1][1]=0;
	index[2][0]=11;index[2][1]=9;
	index[3][0]=21;index[3][1]=9;
	index[4][0]=16;index[4][1]=6;
	index[5][0]=21;index[5][1]=6;
	index[6][0]=17;index[6][1]=4;
	index[7][0]=11;index[7][1]=0;
	index[8][0]=16;index[8][1]=0;
	index[9][0]=16;index[9][1]=2;
	index[10][0]=21;index[10][1]=2;
	index[11][0]=16;index[11][1]=4;
	index[12][0]=21;index[12][1]=4;
	index[13][0]=11;index[13][1]=7;
	index[14][0]=21;index[14][1]=7;
	index[15][0]=12;index[15][1]=7;
	index[16][0]=17;index[16][1]=7;
	index[17][0]=22;index[17][1]=7;
	index[18][0]=12;index[18][1]=9;
	index[19][0]=17;index[19][1]=9;
	index[20][0]=22;index[20][1]=9;
	index[21][0]=12;index[21][1]=10;
	index[22][0]=17;index[22][1]=10;
	index[23][0]=22;index[23][1]=10;
	index[24][0]=12;index[24][1]=12;
	index[25][0]=17;index[25][1]=12;
	index[26][0]=12;index[26][1]=13;
	index[27][0]=22;index[27][1]=13;
	index[28][0]=14;index[28][1]=0;
	index[29][0]=19;index[29][1]=0;
	index[30][0]=24;index[30][1]=0;
	index[31][0]=9;index[31][1]=1;
	index[32][0]=14;index[32][1]=1;
	index[33][0]=19;index[33][1]=1;
	index[34][0]=24;index[34][1]=1;
	index[35][0]=9;index[35][1]=2;
	index[36][0]=14;index[36][1]=2;
	index[37][0]=19;index[37][1]=2;
	index[38][0]=24;index[38][1]=2;
	index[39][0]=9;index[39][1]=3;
	index[40][0]=14;index[40][1]=3;
	index[41][0]=19;index[41][1]=3;
	index[42][0]=24;index[42][1]=3;
	index[43][0]=9;index[43][1]=6;
	index[44][0]=14;index[44][1]=6;
	index[45][0]=24;index[45][1]=6;
	index[46][0]=9;index[46][1]=13;
	index[47][0]=19;index[47][1]=13;
	index[48][0]=14;index[48][1]=8;
	index[49][0]=19;index[49][1]=8;
	index[50][0]=24;index[50][1]=8;
	index[51][0]=14;index[51][1]=9;
	index[52][0]=19;index[52][1]=9;
	index[53][0]=24;index[53][1]=9;
	index[54][0]=9;index[54][1]=10;
	index[55][0]=14;index[55][1]=10;
	index[56][0]=24;index[56][1]=10;



	FILE *f;
	f=fopen("result429.txt","w+b");
	srand(100);
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
			if(cnt==39||cnt==70||cnt==119){
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
	
	for(i=0;i<9;i++){
		InitialState[i]=Key[i];
	}

	
	InitialState[24]|=((UINT16)0x1<<14);
	InitialState[24]|=((UINT16)0x1<<15);





	//state[9][14]=k(39) + k(70) + k(119) + v(63) + v(94) + v(143) + v(159) + v(174) + v(223) + 1
	//k(39) + k(70) + k(119) ....+ 1
	printf("\n%d %d %d\n",(Key[2]>>15)&1,(Key[4]>>14)&1,(Key[7]>>15)&1);
	rightkey=((Key[2]>>15)&1)^((Key[4]>>14)&1)^((Key[7]>>15)&1);                       
	


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
		temp=(InitialState[9]>>14)&1;
		temp1=(tempkey&1)^1;
		if(temp!=temp1)
		{
			InitialState[9]^=(UINT16)0x1<<14;
		}
		for(j=0;j<(INT64(1)<<33);j++)//initialize tempstate
			//33,20,3.4s,22:14s
		{
			for(k=0;k<25;k++)//fresh the tempstate for the key and initial value
			{
				TempState[k]=InitialState[k];
			}
			//give the value for cube variables
			UINT16 cubev[33]={0};
			UINT16 statev[57]={0};
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
			statev[10]=cubev[3];
			statev[11]=cubev[4];
			statev[12]=cubev[4];
			statev[13]=cubev[5];
			statev[14]=cubev[5];
			statev[15]=cubev[6];
			statev[16]=cubev[7];
			statev[17]=cubev[6]^cubev[7];
			statev[18]=cubev[8];
			statev[19]=cubev[9];
			statev[20]=cubev[8]^cubev[9];
			statev[21]=cubev[10];
			statev[22]=cubev[11];
			statev[23]=cubev[10]^cubev[11];
			statev[24]=cubev[12];
			statev[25]=cubev[12];
			statev[26]=cubev[13];
			statev[27]=cubev[13];
			statev[28]=cubev[14];
			statev[29]=cubev[15];
			statev[30]=cubev[14]^cubev[15];
			statev[31]=cubev[16];
			statev[32]=cubev[17];
			statev[33]=cubev[18];
			statev[34]=cubev[16]^cubev[17]^cubev[18];
			statev[35]=cubev[19];
			statev[36]=cubev[20];
			statev[37]=cubev[21];
			statev[38]=cubev[19]^cubev[20]^cubev[21];
			statev[39]=cubev[22];
			statev[40]=cubev[23];
			statev[41]=cubev[24];
			statev[42]=cubev[22]^cubev[23]^cubev[24];
			statev[43]=cubev[25];
			statev[44]=cubev[26];
			statev[45]=cubev[25]^cubev[26];
			statev[46]=cubev[27];
			statev[47]=cubev[27];
			statev[48]=cubev[28];
			statev[49]=cubev[29];
			statev[50]=cubev[28]^cubev[29];
			statev[51]=cubev[30];
			statev[52]=cubev[31];
			statev[53]=cubev[30]^cubev[31];
			statev[54]=cubev[16]^cubev[17]^cubev[18];
			statev[55]=cubev[32];
			statev[56]=cubev[16]^cubev[17]^cubev[18]^cubev[32];
			for(k=0;k<57;k++)
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
