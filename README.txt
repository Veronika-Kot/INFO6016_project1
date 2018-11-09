README

* ProtobufS is a new
* Server has new socket which connects to ProtobufS server and new thread for those connections

* Data base containes 1 register user.
	email-example@gmail.com
	password-example
* type "exit" to logout and return to main menu in any time during auth flow or after.
* when inside a room, type "leaveRoom" to leave the room, then you will be able to pick a new room
* During registartion flow, password is invalid if its length is less than 3 chars
* During registartion flow, email is invalid if it is already in database

* During login flow, password is invalid if data base containes different hashed password
* During login flow, email is invalid if database doesn't containes it. 

Positive scenario:
I
	1.Run protobufS.sln
	2.Run server.sln
	3.Run client.sln (or several client.exe)

II in Client.exe window
	"Wecome to the Fan chat! 1-Login, 2-Crete Account"
1. Send "2"
	"Send me your email"
2. Send "youremail@fanshawe.com"
	"Send me your password"
3. Send "yourpassword"
	"Registration successful; your id is #"
	 What is your name?
4. Send "exit"
	"Wecome to the Fan chat! 1-Login, 2-Crete Account"
5. send "1"
	"Send me your email"
6. Send "youremail@fanshawe.com"
	"Send me your password"
7. Send "yourpassword"
	Creation date ~ ############
	What is your name?





