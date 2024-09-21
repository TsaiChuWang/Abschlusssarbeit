% Test SeDuMi
addpath('/home/wtsaichu/Dokumente/Installieren/sedumi');

% Problem data
b=[1 4]';
A=[-1 1 0 1 1;0 0 -1 1 2];
c=[0 2 0 3.5 6];
[x, y, info]=sedumi(A,b,c)
