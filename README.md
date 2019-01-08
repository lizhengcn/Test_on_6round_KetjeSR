It includes search and verification of the 33-dimension cube satisfying the rules in first two rounds. 
search：KetjeSR6rCubeSearch.py (gurobi)
verification: KetjeSR6rCubeVerification2Round.py (sagemath)

Tests for the key recovery of 6-round and the rationality of 7-round are also given.
KetjeSrv1.cpp;
The program is run in Visual Studio 2010 with x64 platform Release;
The time is about 6 hours for one recovery of 1 key bits.
KetjeSrv2.cpp;
The program is run in Visual Studio 2010 with x64 platform Release;
The time is about 6 hours for one recovery of 1 key bits.

To give evidence for the rationality of assumption, we do 2 tests for 6-r Ketje SR v2: 
(1)For 1000 random 33-dim cubes with random keys, every cube sum over the L-bit output is nonzero;
TestTermsDegree33on6rKetjeSrv2.cpp;
The program is run in Visual Studio 2010 with x64 platform Release;
The time is about 2 hours and 50 minutes for one experiment.
(2)Choose a 33-dim cube randomly, for 1000 random keys,  each output bit has a nonzero cube sum with about half the keys. 
TestTermsDegree33DifferentKeyson6rKetjeSrv2.cpp;
The program is run in Visual Studio 2010 with x64 platform Release;
The time is about 2 hours and 50 minutes for one experiment.
