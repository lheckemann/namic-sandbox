1) Build VTK with Parallel enabled.
2) Configure this directory with CMake
3) Build
4) Run using the following two commands:

      SocketServer.exe   22222
      SocketClient.exe   127.0.0.1 22222


   Where 2222 can be replaced with any (free) port number
   and 127.0.0.1 can be any hostname.

   Note that the Server will only wait for input for
   a couple of seconds.


