'''
Run by sagemath, the time is about 1 minute.
Verify two rules of cube variables in first two rounds:
1. If the bit conditions are satisfied, no ordinary cube variables out of c(2), ...c(32) multiply with c(0)c(1) in the second round.
2. If the bit conditions are not satisfied, some ordinary cube variables out of c(2), ...c(32) multiply with c(0)c(1) in the second round.
'''
from brial import *
import copy
import pdb
import xdrlib ,sys
import random
import time

lanesize=16
Keccak=declare_ring([Block('k',128),Block('v',254),Block('c',100)],globals())

def theta(state):
	tempstate=[[] for i in range(25)]
	for i in range(25):
		for j in range(lanesize):
			tempstate[i].append(state[i][j])
			for k in range(5):
				tempstate[i][j]+=state[(i%5+4)%5+5*k][j]+state[(i%5+1)%5+5*k][(j-1+lanesize)%lanesize]

	for i in range(25):
		for j in range(lanesize):
			state[i][j]=tempstate[i][j]



def rio(state):
	rot=[0,1,62,28,27,36,44,6,55,20,3,10,43,25,39,41,45,15,21,8,18,2,61,56,14]
	tempstate=[[] for i in range(25)]
	for i in range(25):
		for j in range(lanesize):
			tempstate[i].append(state[i][(j-rot[i]+lanesize)%lanesize])

	for i in range(25):
		for j in range(lanesize):
			state[i][j]=tempstate[i][j]

def pi(state):
	tempstate=[[] for i in range(25)]
	for i in range(25):
		y=floor(i/5)
		x=i%5
		x1=y
		y1=(2*x+3*y)%5
		temp=5*y1+x1
		for j in range(lanesize):
			tempstate[temp].append(state[i][j])
	for i in range(25):
		for j in range(lanesize):
			state[i][j]=tempstate[i][j]

def invrio(state):
	rot=[0,1,62,28,27,36,44,6,55,20,3,10,43,25,39,41,45,15,21,8,18,2,61,56,14]
	tempstate=[[] for i in range(25)]
	for i in range(25):
		for j in range(lanesize):
			tempstate[i].append(state[i][(j+rot[i])%lanesize])

	for i in range(25):
		for j in range(lanesize):
			state[i][j]=tempstate[i][j]

def invpi(state):
	tempstate=[[] for i in range(25)]
	for i in range(25):
		y=floor(i/5)
		x=i%5
		x1=y
		y1=(2*x+3*y)%5
		temp=5*y1+x1
		for j in range(lanesize):
			tempstate[i].append(state[temp][j])
	for i in range(25):
		for j in range(lanesize):
			state[i][j]=tempstate[i][j]




def chi(state):
	tempstate=[[] for i in range(25)]
	for i in range(5):
		for j in range(5):
			for k in range(lanesize):
				tempstate[5*i+j].append(state[5*i+j][k]+(state[5*i+(j+1)%5][k]+1)*state[5*i+(j+2)%5][k])

	for i in range(25):
		for j in range(lanesize):
			state[i][j]=tempstate[i][j]



state=[[] for i in range(25)]

state[0].append(Keccak(0))
state[0].append(Keccak(1))
state[0].append(Keccak(0))
state[0].append(Keccak(0))
state[0].append(Keccak(1))
state[0].append(Keccak(0))
state[0].append(Keccak(0))
state[0].append(Keccak(0))

ck=0
for i in range(8):
	state[0].append(k(ck))
	ck+=1

for i in range(7):
	for j in range(lanesize):
		state[1+i].append(k(ck))
		ck+=1
for i in range(8):
	state[8].append(k(ck))
	ck+=1

state[8].append(Keccak(1))
state[8].append(Keccak(0))
state[8].append(Keccak(0))
state[8].append(Keccak(0))
state[8].append(Keccak(0))
state[8].append(Keccak(0))
state[8].append(Keccak(0))
state[8].append(Keccak(0))

for i in range(254):
	state[9+(i/lanesize)].append(v(i))


state[24].append(Keccak(1))
state[24].append(Keccak(1))

invpi(state)

state[2][0]=c(0)
state[7][0]=c(0)
state[22][1]=c(0)
state[17][1]=c(0)
state[11][12]=c(1)
state[21][12]=c(1)
state[4][1]=c(1)
state[5][0]=c(2)
state[20][0]=c(2)
state[5][1]=c(3)
state[15][1]=c(4)
state[20][1]=c(3) + c(4)
state[5][7]=c(5)
state[15][7]=c(6)
state[20][7]=c(5) + c(6)
state[15][8]=c(7)
state[20][8]=c(7)
state[5][10]=c(8)
state[15][10]=c(9)
state[20][10]=c(8) + c(9)
state[5][11]=c(10)
state[15][11]=c(11)
state[20][11]=c(10) + c(11)
state[5][15]=c(12)
state[15][15]=c(13)
state[20][15]=c(12) + c(13)
state[1][7]=c(14)
state[21][7]=c(14)
state[1][14]=c(15)
state[11][14]=c(15)
state[2][2]=c(16)
state[17][2]=c(17)
state[22][2]=c(16) + c(17)
state[2][3]=c(18)
state[7][3]=c(19)
state[17][3]=c(20)
state[22][3]=c(18) + c(19) + c(20)
state[2][4]=c(21)
state[17][4]=c(22)
state[22][4]=c(21) + c(22)
state[2][6]=c(23)
state[7][6]=c(24)
state[17][6]=c(25)
state[22][6]=c(23) + c(24) + c(25)
state[2][9]=c(26)
state[22][9]=c(26)
state[7][11]=c(27)
state[22][11]=c(27)
state[8][12]=c(28)
state[13][12]=c(29)
state[23][12]=c(28) + c(29)
state[8][15]=c(30)
state[13][15]=c(30)
state[4][7]=c(31)
state[14][7]=c(32)
state[19][7]=c(31) + c(32)


state[22][10]=k(34) + k(65) + k(82) + k(97) + v(42) + v(73) + v(105) + v(154) + v(186) + v(217)

dim=33
theta(state)
rio(state)
pi(state)

print "1----------"
for i0 in range(25):
	for j0 in range(lanesize):
		flag=0
		for i in range(dim):
			for j in range(dim):
				if(j!=i):
					if(state[i0][j0]/Keccak(c(i))) and (state[5*(i0/5)+(i0%5+1)%5][j0]/Keccak(c(j))):
						print i0,j0,state[i0][j0],state[5*(i0/5)+(i0%5+1)%5][j0]
						flag=1
						break
			if flag:
				break

print "1----------"


chi(state)



print "1----------"
for i in range(dim-1):
	for j in range(i+1,dim,1):
		for i0 in range(25):
			for j0 in range(lanesize):
				if((state[i0][j0]/Keccak(c(i)*c(j)))!=0):
					print "quarterm",i,j,i0,j0,state[i0][j0]/Keccak(c(i)*c(j))


print "1----------"

theta(state)
rio(state)
pi(state)
chi(state)



print "2----------"

vi=1

for i0 in range(25):
	#print i0
	for j0 in range(lanesize):
		t=state[i0][j0]/Keccak(c(0)*c(vi))
		flag=0
		if(t!=0):
			s=0
			for i in range(2,dim):
				if t/c(i):
					s+=(t/c(i))*c(i)
					flag=1
			if flag:
				print i0,j0,s
		###########
			for i in range(2,dim,1):
				if t/c(i):
					print "cubic term",i,i0,j0,t/c(i)
				
print "2----------"








