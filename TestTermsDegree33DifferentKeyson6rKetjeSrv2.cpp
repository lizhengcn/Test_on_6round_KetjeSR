/*
TestTermsDegree33DifferentKeyson6rKetjeSrv2.cpp;
The program is run in Visual Studio 2010 with x64 platform Release;
The time is about 2 hours and 50 minutes for one experiment.
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

#define ExpTimes 1000//times of experiment
#define cubedim 33
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

	INT64 i,j,k,aa,temp,temp1;

	srand(time(NULL));

	unsigned int rightkey={0};
	unsigned int index[cubedim+5][2];
	//0lane,1deep
	index[0][0]=2;index[0][1]=0;
	index[1][0]=7;index[1][1]=0;
	index[2][0]=22;index[2][1]=1;
	index[3][0]=17;index[3][1]=1;
	index[4][0]=11;index[4][1]=12;
	index[5][0]=21;index[5][1]=12;
	index[6][0]=4;index[6][1]=1;
	FILE *f;
	f=fopen("result.txt","w+b");
	fprintf(f,"cube:\n");
	i=7;
	while(i<cubedim+5){
		unsigned int t0,t1;
		t0=random(25);
		t1=random(16);
		unsigned int flag=0;
		for(j=0;j<i;j++){
			if((t0==index[j][0])&&(t1==index[j][1])){
				flag=1;
				break;
			}
		}
		if(flag==0){
			index[i][0]=t0;
			index[i][1]=t1;
			i++;
		}
	}

		
	for(i=7;i<cubedim+5;i++){
		fprintf(f,"%d:%d %d,",i-5,index[i][0],index[i][1]);
	}
	for(aa=0;aa<ExpTimes;aa++){
		
		fprintf(f,"\n128-bit key:\n");

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

		KeccakInitialize();
		for(j=0;j<(INT64(1)<<cubedim);j++)//initialize tempstate
			
		{
			for(k=0;k<25;k++)//fresh the tempstate for the key and initial value
			{
				TempState[k]=InitialState[k];
			}
			//give the value for cube variables
			UINT16 cubev[cubedim]={0};
			UINT16 statev[cubedim+5]={0};
			for(k=0;k<cubedim;k++)
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
			for(k=2;k<cubedim;k++){
				statev[k+5]=cubev[k];
			}
			/*for(k=0;k<56;k++)
			{
				printf("%d:%d ",k,statev[k]);
			}
			printf("\n");*/
			for(k=0;k<cubedim+5;k++)
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
		
	}

	fclose(f);
	finish = clock();
	printf("time=%f\n",(double)(finish-start)/CLK_TCK);
	printf("Done!\n");
    getch();
    return 0;
}
