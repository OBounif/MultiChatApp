# Multi-Chat Application (Mono thread Server)
[![Open Source Love](https://img.shields.io/badge/Open%20Source-%E2%9D%A4-red.svg)](https://github.com/Fr3eX/MultiChatApp)


Using Socket programming and synchronous I/O multiplexing and mysql api for Db access.Tested on Ubuntu 20.04.1

### How it's work 
```bash
	## Just run make 
	make
```
### Server script usage 
```bash
./SERVER <config-file>
```
### Config file 
```bash
	#Server Configuration
	SERVER_IP=127.0.0.2
	SERVER_PORT=2500
	SERVER_BACKLOG=10
	#Database Configuration
	DB_NAME=DB_NAME
	DB_PASSWORD=DB_PASS
	DB_USER=DB_USER
	DB_HOST=DB_HOST
```
### Commands


| Command       | Parameter             | Desription                          |
| ------------- | --------------------- | ----------------------------------- |
| login         |                       | login to the server                 |
| mp            |  username data        | for private message                 |
| who           |                       | who is online                       |
| logout        |                       | disconnect from the server          |
| help          |                       | show command list                   |
| Ban           |    username           | ban permanently a user              |


## Notes : 
1. The Server manipulate memory efficiently.
2. This Server is not a production-grade server.
3. They might be some bugs that i didn't pay attention.
4. Feel free to collaborate.
