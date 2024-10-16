sudo ip netns add ns1
sudo ip netns add ns2
sudo ip link add veth-ns1 type veth peer name veth-ns2
sudo ip link set veth-ns1 netns ns1
sudo ip link set veth-ns2 netns ns2
sudo ip netns exec ns1 ip addr add 10.0.0.1/24 dev veth-ns1
sudo ip netns exec ns2 ip addr add 10.0.0.2/24 dev veth-ns2
sudo ip netns exec ns1 ip link set veth-ns1 up
sudo ip netns exec ns2 ip link set veth-ns2 up
