# tplt
Network Performance Measurement for HTTP Traffic Under Varying Loss and Latency Conditions

# Examples
./tplt ntest -s vlan.10 -c 20 -l 0.5 -L 100

Attempting to perform 20 HTTP GET requests against ntest with a packet loss of 0.50 percent and added latency of 100 milliseconds

Total time to perform 20 HTTP GET Requests: 9455 milliseconds

Clearing latency and loss from interface vlan.10

![Alt text](images/loss_graph.png?raw=true)
![Alt text](images/loss_table.png?raw=true)
