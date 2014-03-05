About NetAnalyzer and PowerAnalyzer
===================================

During a course at University, vobject and I worked on a paper titled
**_Analysis of Web Data Compression and its Impact on Traffic and Energy Consumption_**
where we decided to take a look at the average composition of HTTP traffic
on an average webserver. In addition we analyzed the compressibility of this traffic
using various compression algorithms such as the well known GZIP (LZ77, deflate),
LZMA, and high speed algorithms such as QuickLZ, LZO, and LZ4.

This was not an easy task and required tons of statistics and testing data.
We wrote NetAnalyzer to capture and export HTTP streams, analyze their latencies
(per mime-type), export all payloads (and statistics) to disk, and much more.

We decided to use **libpcap** to capture a server's HTTP traffic.
Other approaches such as a proxy server would require infrastructure changes to
the server itself which makes it usually unfeasible. We wrote a basic IPv4 and TCP
stack to capture the traffic and send it to the HTTPAnalyzer module which handles
both stream directions and performs gzip decompression if compressed streams
are encountered.

The PowerAnalyzer project is a tool that can _resend_ data exported by
NetAnalyzer. The data is sent from a server to a client machine, following its
original progression. This recreates the real-life
HTTP traffic under testing conditions. The data being sent may be compressed using
aforementioned compression algorithms. During these tests, the PowerAnalyzer
exports statistics about the amount of data send/received and the CPU usage
on the host machine. These statistics can be used in combination with measurements
of the machine's power consumption to compare the impact on traffic and power
consumption of different compression algorithms on web data.

![screenshot](http://i.imgur.com/1qcvKLg.gif)
