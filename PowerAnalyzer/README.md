PowerAnalyzer
=============

# Usage
`./PowerAnalyzer -h`  
`./PowerAnalyzer --help`  

## Server Mode
`./PowerAnalyzer -m server [-p PORT] -d PAYLOADDIR [-z] [-v [debuglevel]]`

Examples:  
`./PowerAnalyzer -m server -p 8468 -d payload_dir -z -v`
`./PowerAnalyzer --mode=server --port=8468 --data=payload_dir --nodirectio --verbose=5`

## Client Mode
`./PowerAnalyzer -m client -s SERVER [-p PORT] -d DATDIR [-c none|gzip|quicklz|lzma|lzo|lz4] [-a] [-l CHUNKSIZE] [-r REPLAYSPEED] [-n REPLAYCOUNT] [-v [debuglevel]]`  

Examples:  
`./PowerAnalyzer -m client -s localhost -p 8468 -d payload_dir -c none -a -l 1024 -r 1 -n 1 -v`  
`./PowerAnalyzer -m client -s localhost -d payload_9 -c lz4 -l 4096 -r 1000 -n 2 -v 3`
`./PowerAnalyzer --mode=client --server 192.168.2.28 --port 8468 --data=payload_dir --compression=gzip --adler --chunksize=1024 --replay=4 --repeat=47 --verbose=1`  
