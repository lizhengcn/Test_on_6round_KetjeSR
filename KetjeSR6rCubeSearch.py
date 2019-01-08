'''
Run by command gurobi.sh, the time is about 1 second.
Search cube variables
'''
from gurobipy import *


v0 = [(2,0,0),(2,1,0),(2,4,1),(2,3,1)]
multiv0=[(2,0,0),(2,4,1)]
multiv1=[(1,2,12),(1,4,12),(4,0,1)]
v0s = [(0,4,14),(4,1,14)]
v1s = [(4,4,14),(0,1,14)]
multiVars=[(3,4,14),(3,1,14)]


lanesize = 16
minOrMax = 'Minimize'
dim = 31



parityTemplate = [
(0,0,0,0,0,-1,-1,1),\
(-1,0,0,0,0,1,1,0),\
(0,-1,0,0,0,1,1,0),\
(0,0,-1,0,0,1,1,0),\
(0,0,0,-1,0,1,1,0),\
(0,0,0,0,-1,1,1,0),\
(1,1,1,1,1,-2,-1,0)]


chiTemplate = [
(-1,-1,0,0,0,0,0,0,1),\
(-1,0,0,0,0,0,0,1,0),\
(0,0,-1,0,-1,0,0,0,1),\
(0,-1,0,-1,0,0,0,0,1),\
(-1,-1,0,0,0,0,-1,1,1),\
(1,0,0,-1,0,-1,0,-1,2),\
(1,0,0,0,-1,0,1,-1,1),\
(1,1,1,0,0,0,0,-1,0),\
(0,-1,-1,1,1,0,0,1,0),\
(0,-1,-1,0,1,1,0,1,0)]


chi2Template = [(-1, 0, -1, -1, 0, 0, 2),    
(-1, 0, -1, 0, 0, 1, 1),
(-1, 0, 0, -1, 1, 0, 1),
(-1, 0, -1, 0, -1, 0, 2),
(-1, 0, 0, -1, 0, -1, 2),
(-1, -1, -1, 0, 0, 0, 2)]

def constraintsOfKeyCapacity(a):####need change
	#add for ketjesr
	constraint = []
	ts=zeroState()
	for i in range(9):#128+16=16*9
		for z in range(lanesize):
			
			ts[i%5][i/5][z]=1
	ts[4][4][lanesize-2]=1
	ts[4][4][lanesize-1]=1
	tsinvpi=inv_Pi(ts)
	for y in range(5):
		for x in range(5):
			for z in range(lanesize):
				if tsinvpi[x][y][z]:
					#print (x,y,z)
					constraint.append(a[x][y][z] + ' = 0') 
	#m=raw_input()
	return constraint


def vars_state(s):
    """generate vars of size 5*5*lanesize  """
    a = []
    for x in range(5):
        col = []
        for y in range(5):
            col.append( [s+'_'+str(x)+'_'+str(y)+'_'+str(z) for z in range(lanesize)] )
        a.append(col)
    return a

def vars_plane(s):
    """  generate vars of size 5*lanesize   """
    d = []
    for x in range(5):
        d.append([s+'_'+str(x)+'_'+str(z) for z in range(lanesize)])
    return d

def vars_dummy():
	ind_set=[]

	num=4
	print "dummynum",num
	dummy = ['e' +str(i) for i in range(num)]
	return dummy

def getVariables_From_Constraints(C):
    V = set([])
    for s in C:
        temp = s.strip()
        temp = temp.replace('+', ' ')
        temp = temp.replace('-', ' ')
        temp = temp.replace('>=', ' ')
        temp = temp.replace('<=', ' ')
        temp = temp.replace('=', ' ')
        temp = temp.replace('>', ' ')
        temp = temp.replace('<', ' ')
        temp = temp.split()
        for v in temp:
            if not v.isdigit():
                V.add(v)
    return V

def theta(a):
    C = []
    D = []
    for x in range(5):
        C.append([0 for z in range(lanesize)])
        D.append([0 for z in range(lanesize)])

    for x in range(5):
        for z in range(lanesize):
            for y in range(5):
                C[x][z] ^= a[x][y][z]
    for x in range(5):
        E = rot(C[(x+1)%5], 1)
        for z in range(lanesize):
            D[x][z] = E[z]^C[(x+4)%5][z]
    for x in range(5):
        for z in range(lanesize):
            for y in range(5):
                a[x][y][z] ^= D[x][z]
    return a


def rot(x, r):
    """Bitwise rotation (to the left) of r bits considering the \
    string of bits is lanesize bits long"""
    r = r%lanesize
    return x[lanesize-r:lanesize] + x[0:lanesize-r]




def RhoAndPi(A):
    offsets = [[0,    36,     3,    41,    18]    ,
       [1,    44,    10,    45,     2]    ,
       [62,    6,    43,    15,    61]    ,
       [28,   55,    25,    21,    56]    ,
       [27,   20,    39,     8,    14]    ]
    B = zeroState()
    for x in range(5):
        for y in range(5):
            B[y][(2*x+3*y)%5]= rot(A[x][y], offsets[x][y])
    return B

def inv_rot(x, r):
    """Bitwise rotation (to the left) of r bits considering the \
    string of bits is lanesize bits long"""
    r = r%lanesize
    return x[r:lanesize] + x[0:r]

def inv_RhoAndPi(A):
    offsets = [[0,    36,     3,    41,    18]    ,
       [1,    44,    10,    45,     2]    ,
       [62,    6,    43,    15,    61]    ,
       [28,   55,    25,    21,    56]    ,
       [27,   20,    39,     8,    14]    ]
    B = zeroState()
    for x in range(5):
        for y in range(5):
            B[x][y] = A[y][(2*x+3*y)%5]
            B[x][y] = inv_rot(B[x][y], offsets[x][y])
    return B


def inv_Pi(A):
    B = zeroState()
    for x in range(5):
        for y in range(5):
            B[x][y] = A[y][(2*x+3*y)%5]
    return B

def constraintsOfV0V1Init(a, v0, v1):
    constraint = []
    for (x,y,z) in v0:
        constraint.append(a[x][0][z] + ' + ' + a[x][1][z] + ' + ' + a[x][2][z] + ' + ' + a[x][3][z] + ' + ' + a[x][4][z] + ' = 0 ') 
 	for (x,y,z) in v1:
		constraint.append(a[x][y][z] + ' = 0')
    return constraint




def genFromConstraintTemplate(vars_list, ineq_template):
    """
    Example:
        >>> ConstraintGenerator.genFromConstraintTemplate(['x0', 'x1'], ['y0'], [(-1, 2, 3, 1), (1, -1, 0, -2)] )
        ['-1 x0 + 2 x1 + 3 y0 >= - 1', '1 x0 - 1 x1 + 0 y0 >= 2']
        >>> ConstraintGenerator.genFromConstraintTemplate(['x0', 'x1'], ['y0'], [(-1, 2, 3, 1), (-1, -1, 0, -2)] )
        ['-1 x0 + 2 x1 + 3 y0 >= - 1', '-1 x0 - 1 x1 + 0 y0 >= 2']
    """
    assert ineq_template != list([])
    assert (len(vars_list)) == (len(ineq_template[1]) - 1)

    constraints = list([])
    for T in ineq_template:
        s = str(T[0]) + ' ' + vars_list[0]
        for j in range(1, len(vars_list)):
            if T[j] >= 0:
                s = s + ' + ' + str(T[j]) + ' ' + vars_list[j]
            elif T[j] < 0:
                s = s + ' - ' + str(-T[j]) + ' ' + vars_list[j]
        s = s + ' >= '
        if T[-1] <= 0:
            s = s + str(-T[-1])
        elif T[-1] > 0:
            s = s + '- ' + str(T[-1])
        constraints.append(s)
    return constraints

def constraintsOfTheta(a, b, F, G):
	constraints = []

	# constraints of column sums
	for x in range(5):
		for z in range(lanesize):
			sum = []
			for y in range(5):
				sum.append(a[x][y][z])
			constraints += genFromConstraintTemplate(sum + [F[x][z], G[x][z]], parityTemplate)
			
			
	for x in range(5):
		for z in range(lanesize):
			for y in range(5):
				constraints.append(b[x][y][z]+' - '+a[x][y][z]+' >= 0')
				constraints.append(b[x][y][z]+' - '+G[(x+4)%5][z]+' >= 0')
				constraints.append(b[x][y][z]+' - '+G[(x+1)%5][(z+lanesize-1)%lanesize]+' >= 0')
				constraints.append(a[x][y][z]+ ' + ' +G[(x+4)%5][z] + ' + '+G[(x+1)%5][(z+lanesize-1)%lanesize] + ' - ' +b[x][y][z] + ' >= 0' )

	return constraints
def zeroState():
    a = []
    for x in range(5):
        col = []
        for y in range(5):
            col.append( [0 for z in range(lanesize)] )
        a.append(col)
    return a

def loadVars(Vars):#########################
    a = zeroState()
    for (x,y,z) in Vars:
        a[x][y][z] = 1
    return a

def loadv0v1(v0,v1):
    a = zeroState()
    for (x,y,z) in v0:
        a[x][y][z] = 1
	for (x,y,z) in v1:
		a[x][y][z] = 1
    return a


def minusv0sv1s(a,v0s,v1s):
	for x in range(5):
		for y in range(5):
			for z in range(lanesize):
				if a[x][y][z]:
					print (x,y,z)
	for (x,y,z) in v0s:
		a[x][y][z] = 0
	for (x,y,z) in v1s:
		a[x][y][z] = 0
	print "middle"
	for x in range(5):
		for y in range(5):
			for z in range(lanesize):
				if a[x][y][z]:
					print (x,y,z)


def genConObj():
    v = vars_state('v')
    sum = []
    for x in range(5):
        for y in range(5):
            for z in range(lanesize):
                sum.append(v[x][y][z])
    return ' + '.join(sum)

def genDimObj():
	a = vars_state('a')
	F = vars_plane('f')
	sum = []
	consumption = []
	for x in range(5):
		for y in range(5):
		    sum += a[x][y]
	FD = ' + '.join(sum)
	for x in range(5):
		consumption += F[x]
	objective = FD + ' - '+ ' - '.join(consumption)
	dummy = vars_dummy()
	objective = objective + ' - ' + ' - '.join(dummy)

	return objective

def genDimCon(a, F):
	sum = []
	consumption = []
	for x in range(5):
		for y in range(5):
		    sum += a[x][y]
	FD = ' + '.join(sum)
	for x in range(5):
		consumption += F[x]
	objective = FD + ' - '+ ' - '.join(consumption)
	dummy = vars_dummy()
	objective = objective + ' - ' + ' - '.join(dummy)

	return objective + ' = ' + str(dim)
	

def writeFile( name, C, V):
	f = open(name, 'w')
	f.write(minOrMax+'\n')

	if minOrMax == 'Minimize':
		f.writelines(genConObj())
	else:
		f.writelines(genDimObj())

	f.write('\n\n')
	f.write('Subject To\n')
	for c in C:
		f.write(c)
		f.write('\n')
	f.write('\n')

	f.writelines(genConObj()+' >= 1\n')

	f.write('Binary\n')
	for v in V:
		f.write(v)
		f.write('\n')
		

	f.close()

def indexInvRhoandPi(index3):
	#print "bindex3",index3
	a=zeroState()
	(x,y,z)=index3
	a[x][y][z]=1
	ai=inv_RhoAndPi(a)
	for x in range(5):
		for y in range(5):
			for z in range(lanesize):
				if ai[x][y][z]==1:
					index3=(x,y,z)
	#print "aindex3",index3
	return index3


def indexRhoandPi(index3):
	#print "bindex3",index3
	a=zeroState()
	(x,y,z)=index3
	a[x][y][z]=1
	ai=RhoAndPi(a)
	for x in range(5):
		for y in range(5):
			for z in range(lanesize):
				if ai[x][y][z]==1:
					index3=(x,y,z)
	#print "aindex3",index3
	return index3

def constraintsofRound():#
	constraints=[]
	a = vars_state('a')
	b = vars_state('b')
	F = vars_plane('f')
	G = vars_plane('g')
	

	constraints += constraintsOfKeyCapacity(a)
	constraints += constraintsOfV0V1Init(a, v0, multiv1)
	constraints += constraintsOfTheta(a, b, F, G)

	b = RhoAndPi(b)
	v = vars_state('v')
	h = vars_state('h')
	c = vars_state('c')
	v0v1Mask = loadv0v1(v0,multiv1)

	
	dummy = vars_dummy()
	#1 round
	
	
	A1T=theta(v0v1Mask)####
	A1RP=RhoAndPi(A1T)

	minusv0sv1s(A1RP,v0s,v1s)

	for x in range(5):
		for y in range(5):
			for z in range(lanesize):
				if A1RP[x][y][z]:
					#22bitleftandright
					constraints.append(b[(x+4)%5][y][z] + ' = 0')
					constraints.append(b[(x+1)%5][y][z] + ' = 0')

	for i in multiv0:
		(x,y,z)=i
		zz = (z-1+lanesize)%lanesize
		constraints.append(G[(x+4)%5][z] + ' = 0')
		constraints.append(G[(x+1)%5][zz] + ' = 0')
		
	for i in multiv1:
		(x,y,z)=i
		zz = (z-1+lanesize)%lanesize
		constraints.append(G[(x+4)%5][z] + ' = 0')
		constraints.append(G[(x+1)%5][zz] + ' = 0')

	
	for y in range(5):
		for z in range(lanesize):
			if (z==multiVars[0][2]) and (y==multiVars[0][1] or y==multiVars[1][1]):
				#need changed according to multiIndex!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				
				if y==multiVars[0][1]:
					x=(multiVars[0][0]+1)%5	
				else:
					x=(multiVars[1][0]+1)%5	
				print "multivar+1",(x,y,z)

			
				constraints.append(b[(x+2)%5][y][z]+' = 0')
				constraints.append(b[(x+4)%5][y][z]+' = 0')

				constraints.append(c[(x+3)%5][y][z]+' - '+b[(x+3)%5][y][z]+' = 0')
				
				constraints.append(c[x][y][z]+' = 0')
				constraints.append(c[(x+1)%5][y][z]+' = 0')
				constraints.append(c[(x+2)%5][y][z]+' = 0')
				####constraints.append(c[(x+3)%5][y][z]+' = 0')
				constraints.append(c[(x+4)%5][y][z]+' = 1')

				constraints.append(v[x][y][z]+' <= 1')
				constraints.append(v[(x+1)%5][y][z]+' <= 1')
				constraints.append(v[(x+2)%5][y][z]+' <= 1')
				constraints.append(v[(x+3)%5][y][z]+' <= 1')
				constraints.append(v[(x+4)%5][y][z]+' <= 1')

				
				constraints.append(h[x][y][z]+' <= 1')#non limit
				constraints.append(h[(x+1)%5][y][z]+' <= 1')#non limit
				constraints.append(h[(x+2)%5][y][z]+' <= 1')#non limit
				constraints.append(h[(x+3)%5][y][z]+' <= 1')#non limit
				constraints.append(h[(x+4)%5][y][z]+' <= 1')#non limit
				
				
				
			else:
				for x in range(5):
					#print y,z
					constraints += genFromConstraintTemplate([b[x][y][z], b[(x+1)%5][y][z], b[(x+2)%5][y][z], v[(x+1)%5][y][z], v[(x+2)%5][y][z], h[(x+1)%5][y][z], h[(x+2)%5][y][z], c[x][y][z]], chiTemplate)
			
	#2 round
	
	MULT=loadVars(multiVars)
	A2RP=RhoAndPi(theta(MULT))

	nemult=zeroState()
	for x in range(5):
		for y in range(5):
			for z in range(lanesize):
				if A2RP[x][y][z] == 1: 
					nemult[(x+4)%5][y][z]=1
					nemult[(x+1)%5][y][z]=1

	NEMULT=inv_RhoAndPi(nemult)

	ind=0
	#DUMMY num 4
	for x in range(5):
		for y in range(5):
			for z in range(lanesize):
				if NEMULT[x][y][z] == 1:
					print "dummy",ind
					sum = []
					sum.append(c[x][y][z])
					constraints.append(dummy[ind] + ' - ' + c[x][y][z] + ' >= 0' )
					constraints += genFromConstraintTemplate([dummy[ind], b[x][y][z], b[(x+1)%5][y][z], b[(x+2)%5][y][z], v[(x+1)%5][y][z], v[(x+2)%5][y][z]], chi2Template)
					zz = (z-1+lanesize)%lanesize
					for yy in range(5):
						sum.append(c[(x+4)%5][yy][z])
						sum.append(c[(x+1)%5][yy][zz])
						constraints.append(dummy[ind] + ' - ' + c[(x+4)%5][yy][z] + ' >= 0' )
						constraints.append(dummy[ind] + ' - ' + c[(x+1)%5][yy][zz] + ' >= 0' )
						constraints += genFromConstraintTemplate([dummy[ind], b[(x+4)%5][yy][z], b[x][yy][z], b[(x+1)%5][yy][z], v[x][yy][z], v[(x+1)%5][yy][z]], chi2Template)
						constraints += genFromConstraintTemplate([dummy[ind], b[(x+1)%5][yy][zz], b[(x+2)%5][yy][zz], b[(x+3)%5][yy][zz], v[(x+2)%5][yy][zz], v[(x+3)%5][yy][zz]], chi2Template)
					constraints.append(' + '.join(sum) + ' - ' + dummy[ind] + ' >= 0')
					#print dummy[ind],sum
					ind += 1
	if minOrMax == 'Minimize':
		constraints.append(genDimCon(a,F))
	return constraints


def genModel():
	V = set([])
	C = list([])
	
	C = C + constraintsofRound()
	V = getVariables_From_Constraints(C)

	name = 'ketjesr'+str(dim)
	writeFile(name + '.lp',C,V)
	m = read(name + '.lp')
	m.optimize()
	

	if m.status == 2:
		print m.ObjVal
		m.write(name + '.sol')
		cnt=0
		for v in m.getVars():
			if (v.x == 1) and (v.VarName[0]=='a'):
				cnt+=1
		print "jishu",cnt
		############readSol(name + '.sol', conditionalVars)
		return m.ObjVal
	return 0

if __name__ == '__main__':
	genModel()
			

