# http-server
A static http server to quickly serve the website in port 8080

# Building

Make sure you are in linux / unix system (or cygwin in windows)

Run this command to build this project
```
make
```
the binaries are compiled in ``build`` folder

### add to path
compiled program is stored in bin folder
install it with
```
make install
```

# Usage
Run the server with
```
./bin/http-server <site's folder path>
```

visit this url in web browser to see the site's homepage 
```
http://localhost:8080/
```

or specify the port and folder with command line flags
```
./bin/http-server -p <port> -d <site path>
```

Get more usage of this program
```
./bin/http-server -h
```
