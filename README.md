# DigiPing - Skeleton for CS260 Assignment

## Goal
In this assignment, you will deepen your understanding of network 
protocols by implementing a program in C++ that replicates the 
functionality of the “ping” command. You will use the ICMP 
protocol to implement ping, rather than TCP or UDP.

Ping is an application that tests whether a particular host is 
reachable across an IP network. It provides some latency information as 
well. Our Ping will work by sending an ICMP Echo packet to the destination 
and then listen for an ICMP Echo Reply (aka “Pong”).

Your program will take an IP address or domain name as an argument and send 
an ICMP Echo packet to the address roughly once per second. The program will 
receive the ICMP Echo Reply packets – or note their absence! It will then 
print out the results – including the payload bytes returned, the round-trip 
time in milliseconds, and the TTL value from the reply.

## Example Usage and Expected Output

You'll want to start testing using localhost as your destination:
```
$ ping 127.0.0.1

Pinging 127.0.0.1 with 32 bytes of data:
Reply from 127.0.0.1: bytes=32, time=0ms, ttl=128
Reply from 127.0.0.1: bytes=32, time=0ms, ttl=128
Reply from 127.0.0.1: bytes=32, time=0ms, ttl=128
<ctrl-c>
```
The program should work when given IP addresses or DNS domain names:
```
$ ping www.google.com

Pinging www.google.com with 32 bytes of data:
Reply from 142.251.215.228: bytes=32 time=16ms ttl=114
Reply from 142.251.215.228: bytes=32 time=32ms ttl=114
Reply from 142.251.215.228: bytes=32 time=15ms ttl=114
<ctrl-c>
```

And when the target does NOT respond to your pings:
```
$ ping cs260.meancat.com

Pinging cs260.meancat.com with 32 bytes of data:
cs260.meancat.com: Request timed out.
cs260.meancat.com: Request timed out.
<ctrl-c>
```

