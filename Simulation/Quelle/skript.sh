sudo ip netns add ns1
sudo ip netns add ns2
sudo ip netns add ns3

sudo ip link add veth-ns1 type veth peer name veth-ns3-1
sudo ip link add veth-ns2 type veth peer name veth-ns3-2

sudo ip link set veth-ns1 netns ns1
sudo ip link set veth-ns2 netns ns2

sudo ip link set veth-ns3-1 netns ns3
sudo ip link set veth-ns3-2 netns ns3

sudo ip netns exec ns3 ip link add name br-ns3 type bridge

sudo ip netns exec ns3 ip link set br-ns3 up
sudo ip netns exec ns3 ip link set veth-ns3-1 up
sudo ip netns exec ns3 ip link set veth-ns3-2 up

sudo ip netns exec ns3 ip link set veth-ns3-1 master br-ns3
sudo ip netns exec ns3 ip link set veth-ns3-2 master br-ns3

sudo ip netns exec ns3 ip addr add 10.0.0.3/24 dev br-ns3
sudo ip netns exec ns3 ip link set br-ns3 up

sudo ip netns exec ns1 ip addr add 10.0.0.1/24 dev veth-ns1
sudo ip netns exec ns1 ip link set veth-ns1 up

sudo ip netns exec ns2 ip addr add 10.0.0.2/24 dev veth-ns2
sudo ip netns exec ns2 ip link set veth-ns2 up

sudo ip netns exec ns1 ping 10.0.0.3 -c 5

sudo ip netns delete ns1
sudo ip netns delete ns2
sudo ip netns delete ns3