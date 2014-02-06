# About NetAnalyzer and PowerAnalyzer

During a course at University, vobject and I worked on a paper about
"TITLE". We decided to have a look at the average composition of HTTP traffic
on an average webserver and also analyze the compressibility of this traffic
using various compression algorithms like the well known GZIP (lz77, deflate)
but also crowbars as LZMA and lightweights like quickLZ, lzo and lz4.

As this was not an easy task an required tons of testing data, we wrote
the NetAnalyzer to capture (and export) HTTP streams, analyze their latencies
(per mime-type), export all payloads (and statistics) to disk, and many many
more.

To be able to capture a server's HTTP traffic, we decided to use **libpcap**.
Other approaches as a proxy server would required infrastructure changes to
the server itself which is usually unfeasible. We wrote a basic IPv4 and TCP
stack to capture the traffic and send it to the HTTPAnalyzer which handles
both stream directions and performs gzip decompression if compressed streams
are encountered.

Another project is the PowerAnalyzer which uses the exports of the NetAnalyzer
and sends them between two given computers (client/server) to replay real 
HTTP traffic for testing conditions. Those payloads may be compressed using
aforementioned compression algorithms to check the size-reduction but also
check on cpu-usage and energy-consumption.

![screenshot](http://imageshack.com/a/img703/1840/pom.gif)
