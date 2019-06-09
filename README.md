# P2P_bittorrent_server  
## Client -> Server  
#### There are three types of messages a client can send to a server(tracker).
### 1. complete  
> 0  
> (filename)  
> (port)  
### 2. start  
> 1  
> (filename)  
### 3. stop  
> 2  

## Server -> Client
#### The sever will reply message 2 as follows:
> (length_of_list)  
> (ip1)  
> (port1)  
> (ip2)  
> (port2)  
> ...  
