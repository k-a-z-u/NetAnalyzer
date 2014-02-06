CLIENT:

usage:  ./PowerAnalyzer -h
        ./PowerAnalyzer --help
        ./PowerAnalyzer -m client -s localhost -p 8468 -d payload_dir -c none -a -l 1024 -r 1 -n 1 -v
        ./PowerAnalyzer --mode=client --server 192.168.2.28 --port 8468 --data=payload_dir --compression=none|gzip|quicklz|lzma|lzo|lz4 --adler --chunksize=1024 --replay=4 --repeat=47 --verbose=1


SERVER:

usage:  ./PowerAnalyzer -h
        ./PowerAnalyzer --help
        ./PowerAnalyzer -m server -p 8468 -d payload_dir -z -v
        ./PowerAnalyzer --mode=server --port=8468 --data=payload_dir --nodirectio --verbose=5
