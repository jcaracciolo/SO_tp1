# SO - TP1

A stock exchange of vegetables simulator (yes, with potatoes!)

## How to run

### Fast Way
In the root folder use "./run.sh". This will compile and execute the server, the client, the user interaction and logs in separate terminals.

### Long Way
In  root folder: 
* make [optional: com=Sockets]
---> Project compiled

In root folder:
* cd Server
* ./run
---> Server Running

Then in another terminal (from root folder):
* cd Client
* ./Client [optional: address to connect]
---> A simulated client running

In a third terminal (From root folder):
* cd Client
* ./user
---> User can interact with application

In fourth terminal (from root folder):
* cd Server
* ./log.sh
---> Logs showing

**IMPORTANT NOTE:** all executables must be runned in their folders (DO NOT DO Client/client for example from root folder). The team is not responsable of possible consequences of not following this rule.