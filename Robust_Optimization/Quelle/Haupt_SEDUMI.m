addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/extras')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/solvers')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/parametric')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/moment')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/global')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/robust')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/sos')
addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/operators')
addpath('/home/wtsaichu/Dokumente/Installieren/sedumi/')

P = 3
E = 4
W = 3
C = 60
N = 5

f_st = sdpvar(N, N, E); 
x = sdpvar(P); 
y = sdpvar(W, E); 
% f_st = sdpvar(N, N, E); 
alpha = sdpvar(1); 

Objective = alpha;
Constraints = [];

% Constraints = [Constraints, f_st(0, 0) == mu(p)];
mu = [10.365820, 9.65275435, 8.4787278]

for p = 1:P
	% disp(p)
	Constraints = [Constraints, x(p) == mu(p)];
end

for w=1:W
	for e=1:E
		if e == w
			Constraints = [Constraints, y(w,e) == x(w)]
		elseif e== E
			Constraints = [Constraints, y(w,e) == x(w)]
		else
			Constraints = [Constraints, y(w,e) == 0]
		end
	end
end

Constraints = [Constraints, 2.145966026289347*norm((y(1,1)*(3.1658205/10.3658205)), 2) <= alpha*C - y(1,1)]
Constraints = [Constraints, 2.145966026289347*norm((y(2,2)*(3.6270764/9.65275)), 2) <= alpha*C - y(2,2)]
Constraints = [Constraints, 2.145966026289347*norm((y(3,3)*(3.6193458/8.478727)), 2) <= alpha*C - y(3,3)]
Constraints = [Constraints, 2.145966026289347*norm((y(1,4)*(3.1658205/10.3658205))+(y(2,4)*(3.6270764/9.65275))+(y(3,4)*(3.6193458/8.478727)), 2) <= alpha*C - (y(1,4)+y(2,4)+y(3,4))]


Constraints = [Constraints, alpha >= 0];

sol = optimize(Constraints, Objective);

disp('alpha:');
disp(value(alpha));

