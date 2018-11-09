#ifndef DB_Connector_H
#define DB_Connector_H
#include <mysql_connection.h>
#include <cppconn/resultset.h>

#include <string>

class DBconnector
{
public:
	DBconnector();
	virtual ~DBconnector();

	int ConnectToDatabase(const std::string &host, const std::string &user, const std::string &pass, const std::string &db);
	sql::ResultSet* Query(const std::string &query);
	int QueryUpdate(const std::string &query);
	bool QueryExecute(const std::string &query);
	sql::ResultSet* QueryExecuteWithResult(const std::string &query);

	void AddAuth(const std::string &email, const std::string &salt, const std::string &hashed_p, int userID);
	void RegisterUser(std::string lastLogin, std::string creationDate);
	bool SelectUserByEmail(std::string email, int& id);
	void SelectHashByEmail(std::string email, std::string& salt, std::string& hash);

private:
	void PrepareStatements(void);

	sql::Connection* conn;

	sql::PreparedStatement* mAddWebAuth;
	sql::PreparedStatement* mRegisterUser;
	sql::PreparedStatement* mSelectWebAuthByEmail;
	sql::PreparedStatement* mSelectHashByEmail;
};

#endif