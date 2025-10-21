Sample.GameServer contains project as the original game server
informer.idl is a file provided by us(ICE) which defines the interface between informer and game server
Sample.GameServer.With.RPC contains the modified game server which implement the interface
Sample.Informer contains the test code as informer which calls the interface
To do the test, import the rpc.reg on the sample game server and run Sample.GameServer.With.RPC.exe
on the informer server, run Sample.informer.exe with a single argument which specifies the host name or ip address of the sample game server.

Good Luck!

Jordan Zhou
