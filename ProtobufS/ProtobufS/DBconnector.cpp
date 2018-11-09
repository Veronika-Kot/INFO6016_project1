#include "DBconnector.h"
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_error.h>

#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>



DBconnector::DBconnector()
{
}


DBconnector::~DBconnector()
{
}

int DBconnector::ConnectToDatabase(
	const std::string &host,
	const std::string &user,
	const std::string &pass,
	const std::string &db)
{
	int status = 0;

	sql::ConnectOptionsMap connection_properties;

	/*connection_properties["hostName"] = sql::SQLString(host.c_str());
	connection_properties["userName"] = sql::SQLString(user.c_str());
	connection_properties["password"] = sql::SQLString(pass.c_str());
	connection_properties["CLIENT_MULTI_STATEMENTS"] = (true);*/



	try
	{
		// Step #1 Create our driver
		sql::Driver* driver = get_driver_instance();

		// Step #2 Attempt to connect to the database
		conn = driver->connect(host.c_str(), user.c_str(), pass.c_str());
		//conn = driver->connect(connection_properties);
		// Step #3 Select our schema
		conn->setSchema(db.c_str());

		// Step #4 Prepare our statements
		PrepareStatements();
	}
	catch (sql::SQLException e)
	{
		// Catch any error, and output here
		printf("Failed to connect to database with error code: %d\n", e.getErrorCode());
		printf(e.what());
		status = 1;
	}

	return status;
}

void DBconnector::PrepareStatements(void)
{
	mAddWebAuth = conn->prepareStatement("INSERT INTO web_auth (email, salt, hashed_password, userID) VALUES (?, ?, ?, ?);");
	mRegisterUser = conn->prepareStatement("INSERT INTO user (last_login, creation_date) VALUES (?, ?);");
	mSelectWebAuthByEmail = conn->prepareStatement("SELECT userID FROM web_auth WHERE email = ?");
	mSelectHashByEmail = conn->prepareStatement("SELECT salt, hashed_password FROM web_auth WHERE email = ?");

}


sql::ResultSet* DBconnector::Query(const std::string &query)
{
	sql::SQLString sql(query.c_str());
	sql::PreparedStatement* pStmt = conn->prepareStatement(sql);
	try
	{
		return pStmt->executeQuery();
	}
	catch (sql::SQLException e)
	{
		printf("SelectFromDB error code: %d\n", e.getErrorCode());
		printf(e.what());
	}

	return 0;
}

int DBconnector::QueryUpdate(const std::string &query)
{
	sql::SQLString sql(query.c_str());
	sql::PreparedStatement* pStmt = conn->prepareStatement(sql);
	try
	{
		return pStmt->executeUpdate();
	}
	catch (sql::SQLException e)
	{
		printf("SelectFromDB error code: %d\n", e.getErrorCode());
		printf(e.what());
	}

	return 0;
}

sql::ResultSet* DBconnector::QueryExecuteWithResult(const std::string &query)
{
	sql::SQLString sql(query.c_str());
	sql::PreparedStatement* pStmt = conn->prepareStatement(sql);

	try
	{
		if (pStmt->execute())
		{
			return pStmt->getResultSet();
		}
	}
	catch (sql::SQLException e)
	{
		printf("SelectFromDB error code: %d\n", e.getErrorCode());
		printf(e.what());
	}

	return 0;
}

bool DBconnector::QueryExecute(const std::string &query)
{
	sql::SQLString sql(query.c_str());
	sql::PreparedStatement* pStmt = conn->prepareStatement(sql);

	try
	{
		return pStmt->execute();
	}
	catch (sql::SQLException e)
	{
		printf("SelectFromDB error code: %d\n", e.getErrorCode());
		printf(e.what());
	}

	return false;
}

void DBconnector::AddAuth(const std::string &email, const std::string &salt, const std::string &hashed_p, int userID)
{
	mAddWebAuth->setString(1, email.c_str());
	mAddWebAuth->setString(2, salt.c_str());
	mAddWebAuth->setString(3, hashed_p.c_str());
	mAddWebAuth->setInt(4, userID);

	try
	{
		mAddWebAuth->executeUpdate();
	}
	catch (sql::SQLException e)
	{
		printf("Add auth exception error code: %d\n", e.getErrorCode());
		printf(e.what());
	}
}

void DBconnector::RegisterUser(std::string lastLogin, std::string creationDate)
{
	mAddWebAuth->setString(1, lastLogin);
	mAddWebAuth->setString(2, creationDate);
	

	try
	{
		mAddWebAuth->executeUpdate();
	}
	catch (sql::SQLException e)
	{
		printf("Register user exception error code: %d\n", e.getErrorCode());
		printf(e.what());
	}
}

bool DBconnector::SelectUserByEmail(std::string email, int& id)
{
	mSelectWebAuthByEmail->setString(1, email);
	sql::ResultSet* rs = mSelectWebAuthByEmail->executeQuery();

	if (!rs->next())
	{
		return false;
	}
	id = rs->getInt(1);
	return true;
}

void DBconnector::SelectHashByEmail(std::string email, std::string& salt, std::string& hash)
{
	mSelectHashByEmail->setString(1, email);
	sql::ResultSet* rs = mSelectHashByEmail->executeQuery();

	while (rs->next())
	{
		salt = rs->getString(1);
		hash = rs->getString(2);
	} 

}