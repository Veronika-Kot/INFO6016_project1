#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <stdio.h>      /* puts */
#include <time.h> 
#include "../gen/chat.pb.h"
#include "DBconnector.h"


extern DBconnector dbHelper;
void passwordToHashNewSalt(std::string password, std::string& salt, std::string& hashedPassword);
void passwordToHash(std::string password, std::string salt, std::string& hashedPassword);
std::string getTimeNow();

void connectToDB()
{
	int result = dbHelper.ConnectToDatabase("127.0.0.1:3306", "root", "root", "chatdb");

	if (result != 0)
	{
		printf("Can't connect to DB!\n");
		return;
	}
}

void addEntryToWebAuth(std::string email, std::string password, int id)
{
	std::string salt = "";
	std::string hashedPassword = "";
	passwordToHashNewSalt(password, salt, hashedPassword);
	dbHelper.AddAuth(email, salt, hashedPassword, id);
}

bool checkPassword(std::string password, std::string salt, std::string hash)
{
	std::cout << password << std::endl;
	std::cout << salt << std::endl;
	std::cout << hash << std::endl;
	std::string tempHash;
	passwordToHash(password, salt, tempHash);
	std::cout << tempHash << std::endl;
	if (tempHash == hash)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool checkIfAccountExists(std::string email, int& ID)
{
	if (dbHelper.SelectUserByEmail(email, ID))
	{
		return true;
	}
	return false;
}

bool addNewClient(std::string email, std::string password, int &id)
{
	
	//insert info to "user" table
	std::string timeNow = getTimeNow();
	std::string newRegistry = "INSERT INTO user(last_login, creation_date) VALUES('" + timeNow + "', '" + timeNow + "');";
	sql::ResultSet* insertNewRegistry = dbHelper.Query(newRegistry);
	sql::ResultSet* lastID = dbHelper.Query("SELECT LAST_INSERT_ID()");
	if (lastID->next())
	{
		id = lastID->getInt(1);
	}
	//insert info to "web_auth" table
	addEntryToWebAuth(email, password, id);
	return true;
}

bool authenticateTheClient(std::string email, std::string password, int id, std::string& creationDateStr)
{
	std::string savedSalt;
	std::string savedHash;
	dbHelper.SelectHashByEmail(email, savedSalt, savedHash);
	if (!checkPassword(password, savedSalt, savedHash))
	{
		std::cerr << "Wrong password" << std::endl;
		return false;
	}

	//update last login time in "user" table
	std::string timeNow = getTimeNow();
	std::string idStr = std::to_string(id);
	std::string updateRegistry = "UPDATE user SET last_login = '" + timeNow + "' WHERE id = " + idStr + ";";
	if (dbHelper.QueryUpdate(updateRegistry))
	{
		std::cout << "Last login was updated" << std::endl;
	}

	sql::ResultSet* creationDate = dbHelper.Query("SELECT creation_date from user WHERE id = " + idStr + ";");
	if (creationDate->next())
	{
		creationDateStr = creationDate->getString(1);
	}


	return true;
}

std::string getTimeNow()
{
	std::string timeSQLFormat;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[24];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 24, "%F %T", timeinfo);
	std::stringstream ss;
	ss << buffer;
	timeSQLFormat = ss.str();
	return timeSQLFormat;
}