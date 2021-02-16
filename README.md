# tplt
Network Performance Measurement for HTTP Traffic Under Varying Loss and Latency Conditions

# Examples
./tplt ntest -s vlan.10 -c 20 -l 0.5 -L 100

Attempting to perform 20 HTTP GET requests against ntest with a packet loss of 0.50 percent and added latency of 100 milliseconds

Total time to perform 20 HTTP GET Requests: 9455 milliseconds

Clearing latency and loss from interface vlan.10

# Using iperf3

It's also possible to measure TCP's performance using tc to add both latency and loss to a netdev.  The data below represents varying conditions of loss and latency for a single stream of iperf3 over a 1Gbps interface.  The hosts tested run Ubuntu 18.04 and have the following characteristics:

perftest1:~$ cat /proc/cpuinfo | grep model
model           : 1
model name      : AMD EPYC Processor (with IBPB)
model           : 1
model name      : AMD EPYC Processor (with IBPB)
model           : 1
model name      : AMD EPYC Processor (with IBPB)
model           : 1
model name      : AMD EPYC Processor (with IBPB)

perftest1:~$ sudo sysctl -A | grep net.ipv4.tcp_congestion_control
net.ipv4.tcp_congestion_control = cubic

perftest1:~$ lspci | grep "00:06.0"
00:06.0 Ethernet controller: Realtek Semiconductor Co., Ltd. RTL-8100/8101L/8139 PCI Fast Ethernet Adapter (rev 20)

![Alt text](images/loss_graph.png?raw=true)
![Alt text](images/loss_table.png?raw=true)
