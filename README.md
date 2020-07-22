# SimplePing

## Description
This program takes in a website info and can theoretically keep pinging info from that website but for simplicity and to avoid getting banned it only pings the website four times returning data on the Packets received and if any loss over the network appeared.
Program Language: C , OS: Linux Ubuntu 14.04 Version

## Interesting Fact
Before writing this program, I had never worked with the C programming Language ever nor had I any idea about socket programming and networking in computers. Withing a week I brushed up C and learnt the basics of pinging, the ICMP protocol, and an introduction to Socket Programming

## How It Works
The output of all the programs is displayed in the a.out file that has to be compiled in linux to show the output. To assist me in understanding the concept of pinging over the ICMP protocol I used the UNPVe13 repository on github which is part of a famous UNIX networking guide. My code is written in the main.new.c file. First, we open a new socket connection. Then, we get an IP address or the domain name and send a request using the ICMP4 protocol and then recieve the data in the form of packets over the ICMP4 protocol. Finally, we close the socket connection.



 
