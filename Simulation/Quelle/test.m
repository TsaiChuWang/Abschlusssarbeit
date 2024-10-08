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
C = 360
N = 5

% x = sdpvar(P); 
% y = sdpvar(W, E); 
% f_st = sdpvar(N, N, E); 
f_st = sdpvar(10); 
alpha = sdpvar(1); 

Objective = alpha;
Constraints = [];

% Constraints = [Constraints, f_st(0, 0) == mu(p)];
mu = [120, 120, 120]

% Constraints = [Constraints, f_st(1,5,1) == mu(1)];
% Constraints = [Constraints, f_st(1,5,4) == mu(1)];

% Constraints = [Constraints, f_st(2,5,2) == mu(2)];
% Constraints = [Constraints, f_st(2,5,4) == mu(2)];

% Constraints = [Constraints, f_st(3,5,3) == mu(3)];
% Constraints = [Constraints, f_st(3,5,4) == mu(3)];

% for p = 1:P
% 	% disp(p)
% 	Constraints = [Constraints, x(p) == mu(p)];
% end

% for w=1:W
% 	for e=1:E
% 		if e == w
% 			Constraints = [Constraints, y(w,e) == x(w)]
% 		elseif e== E
% 			Constraints = [Constraints, y(w,e) == x(w)]
% 		else
% 			Constraints = [Constraints, y(w,e) == 0]
% 		end
% 	end
% end

% Constraints = [Constraints, 2.145966026289347*norm((y(1,1)*(1/3)), 2) <= alpha*C - y(1,1)]
% Constraints = [Constraints, 2.145966026289347*norm((y(2,2)*(1/3)), 2) <= alpha*C - y(2,2)]
% Constraints = [Constraints, 2.145966026289347*norm((y(3,3)*(1/3)), 2) <= alpha*C - y(3,3)]
% Constraints = [Constraints, 2.145966026289347*norm((y(1,4)*(1/3))+(y(2,4)*(1/3))+(y(3,4)*(1/3)), 2) <= alpha*C - (y(1,4)+y(2,4)+y(3,4))]


% Constraints = [Constraints, 2.145966026289347*norm((f_st(1,5,1)*(1/3)), 2) <= alpha*C - f_st(1,5,1)]
% Constraints = [Constraints, 2.145966026289347*norm((f_st(2,5,2)*(1/3)), 2) <= alpha*C - f_st(2,5,2)]
% Constraints = [Constraints, 2.145966026289347*norm((y(3,3)*(1/3)), 2) <= alpha*C - y(3,3)]
% Constraints = [Constraints, 2.145966026289347*norm((f_st(1,5,1)*(1/3))+(f_st(2,5,2)*(1/3))+(f_st(3,5,3)*(1/3)), 2) <= alpha*C - (f_st(1,5,1)+f_st(2,5,2)+f_st(3,5,3))]

for p = 1:10
    Constraints = [Constraints, f_st(p) == 120];
    Constraints = [Constraints, 3.034854258770293*norm((f_st*(1/3)), 2) <= alpha - f_st]
    Constraints = [Constraints, f_st(p) >= 0];
Constraints = [Constraints, alpha >= 0];

sol = optimize(Constraints, Objective);

disp('alpha:');
disp(value(alpha));

