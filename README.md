# socket.io-cpp-client-sample
Sample of Socket.IO C++ Client.

This program provide result of command on native client's shell to web browser.

This program is explained in [linked Japanese article](http://llamerad-jp.hatenablog.com/entry/2015/05/22/151140).

## How to use.

### Compile native client.

Build [socket.io-client-cpp](https://github.com/socketio/socket.io-client-cpp)

ref. https://github.com/socketio/socket.io-client-cpp/blob/master/INSTALL.md#with-cmake

```
# at workdir
$ git -b 3.0.0 clone --recurse-submodules https://github.com/socketio/socket.io-client-cpp.git
$ mkdir -p socket.io-client-cpp/build
$ cd socket.io-client-cpp/build
$ cmake ..
$ make
$ sudo make install
```

Build [sample]()

```
# at workdir
$ git clone https://github.com/llamerada-jp/socket.io-cpp-client-sample.git
$ mkdir -p socket.io-cpp-client-sample/src/build
$ cd socket.io-cpp-client-sample/src/build
$ cmake -D SIO_DIR=<Socket.IO C++ Client dir> -D BOOST_ROOT=<Boost dir> ..
$ make
```

### Run server.

```
$ cd socket.io-cpp-client-sample
$ cd src
$ npm install
$ node index.js
```

### Run client

```
$ cd socket.io-cpp-client-sample
$ cd src/build
$ ./client http://localhost:8000/ <Your pc's ID.>
```

### Access by web browser.

Access to [http://localhost:8000/](http://localhost:8000/) and input browser's ID.
Input command (like 'ls') and push button.
The results on C++ client are displayed.
