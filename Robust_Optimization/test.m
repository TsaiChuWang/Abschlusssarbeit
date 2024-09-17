% Test SeDuMi
addpath('/home/wtsaichu/Dokumente/Installieren/sedumi');

% Problem data
D = [0 0 0; 0 0 0; 0 0 0; -1 0 1.5; 0 1 -1];
f = [2.4 -2.4 2.4];
b = [1; 0; 0; 0; 0];  % Convert b to a column vector
A1 = [0 -1 0 1 0; 0 0 1 0 -1];
A2 = [0 0.5 0 0 0; 0 0 1 0 0];
A3 = [0 0 0 -0.7071 -0.7071; 0 0 0 -0.3536 0.3536];

% Define the vectors and matrices
b1 = b;
b2 = zeros(5,1);
b3 = b2;
c1 = [0; 0];  % Convert c1 to a column vector
c2 = [-0.5; 0];  % Convert c2 to a column vector
c3 = [4.2426; -0.7071];  % Convert c3 to a column vector
d1 = 0; 
d2 = 1;
d3 = 1;

% Construct constraint matrices
At1 = -[b1, A1];
At2 = -[b2, A2];
At3 = -[b3, A3];
At = [-D, At1, At2, At3];
bt = -b;
ct1 = [d1; c1];
ct2 = [d2; c2];
ct3 = [d3; c3];
ct = [f; ct1; ct2; ct3];

% Define SeDuMi cones
K.l = size(D,2);
K.q = [size(At1,2), size(At2,2), size(At3,2)];

% Solve the problem using SeDuMi
[xs, ys, info] = sedumi(At, bt, ct, K);

% Extract solution
x = ys;
r_s = x(2:3);
s_s = x(4:5);

% Display results
disp('solution points in regions R and S are:')
disp('r_s:'); disp(r_s);
disp('s_s:'); disp(s_s);
disp('minimum distance:');
disp(norm(r_s - s_s));