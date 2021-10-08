# tcp-listener-single-client

A very basic TCP server class that ~~accepts a single client~~ can now accept multiple clients! Implemented using [Windows Sockets 2](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2).  
A sample main.cpp file is included to illustrate how it can be used.  

Similar to [tcp-server](https://github.com/molnar-david/tcp-server), but abstracted into a class form to make it more portable and flexible.

## Note

You can customize it to your own liking by making a subclass from it, and implementing the three virtual functions (onClientConnected, onClientDisconnected and onMessageReceived).

*Made with the help of the [Networking in C++](https://www.youtube.com/playlist?list=PLZo2FfoMkJeEogzRXEJeTb3xpA2RAzwCZ) YouTube series by [Sloan Kelly](https://www.youtube.com/c/sloankelly)*  
*The tutorial series didn't include a lot of error checking, so most of it I had to look up and write myself*
