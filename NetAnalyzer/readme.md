# About NetAnalyzer

The NetAnalyzer uses **libpcap** an IPv4 and TCP re-assembler to capture HTTP traffic on a server and
tracks statistics like latency, payload size/entropy, etc.
Further options include exporting those payloads (for later use within the **PowerAnalyzer**)
or applying various compression algorithms to check the payloads compressibility and needed
compression times.

Note: As the NetAnalyzer uses **libpcap** for capturing, the compressors are currently triggered
at packet level for each chunk. Compressors like gzip support streaming and thus do not suffer from
this fact. Other algorithms like lzo, quicklz and lz4 compress each chunk individually and could
achieve much better compression ratios when triggered with larger payload chunks. 
As latest versions of lz4 provide a streaming API, this issue needs further work/investigations
to ensure comparable results between those algorithms.


# External Dependencies

The NetAnalyzer requires some external libraries which are listed here:
	build-essential (compiler and default libs)
	cmake			(compilation scripts)
	libpcap-dev		(capturing traffic from network interfaces)
	ncurses-dev		(commandline GUI)
	
Some additional libraries might be required when configuring the build
for some optional features like gzip decompression for HTTP streams.
A list of this options can be seen here:


# Additional Features

Compilation switches for additional functionalities and
required external libraries:
	-DWITH_ZLIB=ON		[requires libz]		(gzip (de)compression support, also applies to HTTP streams)
	-DWITH_LZMA=ON		[requires liblzma]	(lzma (de)compression support)
	-DWITH_LZO=ON		[built-in]			(lzo (de)compression support)
	-DWITH_LZ4=ON		[built-in]			(lz4 (de)compression support)
	-DWITH_QUICKLZ=ON	[built-in]			(quickLZ (de)compression support)
	

# Compiling
	
Just clone the repository from GitHub. Create a (cmake-typical) **build** folder next to the **NetAnalyzer**
folder and then run the following commands. 

	cd ..
	mkdir build
	cd build
	cmake ../NetAnalyzer
	
	# or (with optional packages)
	cmake ../NetAnalyzer -DWITH_ZLIB=ON -DWITH_LZO=ON ...
	

# Run and Configuration

To run a minimal version of the NetAnalyzer only a few command-line arguments are necessary.
One basic setup would be: "capturing all HTTP traffic on a given network-interface".
The corresponding command-line is as follows:
	sudo ./NetAnalyzer --src=sniffer --dev=eth0 --gui=ncurses
Ncurses is currently the only supported GUI.

For a fully featured setup, a configuration file **config.xml** is necessary
to adjust the behavior in detail:
 #	what to capture / analyze.
 #	which compressors to apply the the payloads
 #	what to export to the filesystem
 
The checkout contains a basic version of a **config.xml** which can be adjusted
to your specific needs.



# License
