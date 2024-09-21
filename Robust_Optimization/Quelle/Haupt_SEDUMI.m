phi_inv_eta = norminv(0.9);  % Phi^{-1}(\eta)

% Initialize A_robust and b_robust
A_robust = [];
b_robust = [];

for e = 1:E
    A_block = zeros(1 + 2*42, total_vars);  % Create a block for each edge (SOC constraint)
    
    % Left-hand side for the norm: norm(\sum_{s,t} f_{st}(e) * sigma_{st})
    for s = 1:N
        for t = 1:N
            if s ~= t
                idx = (s-1)*N + t;  % Linear index for f_{st}(e)
                A_block(1, 1 + idx + (e-1)*42) = sigma_st(s, t);  % Coefficients for the norm
            end
        end
    end
    
    % Right-hand side: [c_e - \sum f_{st}(e) * \mu_{st}] / \Phi^{-1}(\eta)
    for s = 1:N
        for t = 1:N
            if s ~= t
                idx = (s-1)*N + t;
                A_block(2:end, 1 + idx + (e-1)*42) = -mu_st(s, t);  % Coefficients for the RHS
            end
        end
    end
    A_robust = [A_robust; A_block];
    
    % Right-hand side for the SOC constraint
    b_robust = [b_robust; c_e(e)/phi_inv_eta];
end
