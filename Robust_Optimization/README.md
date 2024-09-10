# Robust Optimization

Assume that the traffic usage patterns of each tenant follow a Gaussian distribution. We can then represent the problem using the model outlined in Equation \ref{tab:robust_optimization_model}. The constant values used in this formulation are provided in Table \ref{tab:robust_optimization_symbols}.

## Table: Symbols and Their Descriptions Used in the Robust Optimization Model

| Symbol       | Description                                                                                                                  |
|--------------|------------------------------------------------------------------------------------------------------------------------------|
| $E$          | The set of physical network links.                                                                                             |
| $N$          | The set of physical network nodes.                                                                                             |
| $E^{OUT}_n$  | The set of links emanating from node $n$.                                                                                     |
| $E^{IN}_n$   | The set of links terminating at node $n$.                                                                                     |
| $d_{st}$     | A Gaussian random variable representing the traffic demand between origin-destination pair $st$, where $d_{st} \sim \mathcal{N}(\mu_{st}, \sigma^2_{st})$. |
| $\mu_{st}$   | The mean of the Gaussian random variable $d_{st}$, indicating the average traffic demand for the pair $st$.                  |
| $\sigma^2_{st}$ | The variance of the Gaussian random variable $d_{st}$, reflecting the uncertainty in the traffic demand for the pair $st$. |
| $c_e$        | The capacity of link $e$, which represents the maximum allowable traffic flow on the link.                                    |
| $\eta$       | The probability threshold ensuring that the traffic does not exceed the link capacity, serving as a reliability measure.       |

**Decision Variables**

| Symbol     | Description                                                         |
|------------|---------------------------------------------------------------------|
| $\alpha$   | The maximum allowable link utilization, where $\alpha \geq 0$.     |
| $f_{st}(e)$| The traffic flow from node $s$ to node $t$ through link $e$.       |

## Model Equation

The optimization problem can be formulated as follows:

$$
\begin{aligned}
    \min & \ \alpha \\
    & \text{subject to} \\
    & P\left(\sum_{s,t \in N, s \neq t} d_{st} f_{st}(e) \leq c_e\right) \geq \eta & \forall e \in E \\
    & \sum_{s,t \in N, s \neq t} f_{st}(e) \leq \alpha c_e & \forall e \in E \\
    & \sum_{e \in E^{OUT}_n} f_{st}(e) - \sum_{e \in E^{IN}_n} f_{st}(e) = \begin{cases}
        \mu_{st} & \text{if } n=s \\
        0 & \text{if } n \neq s, n \neq t \\
        -\mu_{st} & \text{if } n=t
    \end{cases} \\
    & \forall n, s, t \in N \text{ with } s \neq t \\
    & f_{st}(e) \geq 0 & \forall s, t \in N \text{ with } s \neq t \text{ and } e \in E
\end{aligned}
$$