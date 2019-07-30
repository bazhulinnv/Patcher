#include <string>
#include <vector>
#include <utility>
#include <pqxx/pqxx>

class DBConnection
{
public:

	struct LoginData
	{
		std::string databaseName;
		std::string username;
		std::string password;
		std::string host;
		unsigned int portNumber;
		std::string result;
	};

	LoginData info;

	/* 
	*/
	explicit DBConnection(std::string loginCredentials);
	
	// Safely deletes connection.
	~DBConnection();
	
	/*	Sets connection using login credentials.
		Login credentials is assumed to have been passed
		to the constructor when the object was created.
	*/
	void setConnection();
	
	// Returns pointer to current connection object.
	pqxx::connection* getConnection();

private:
	// Points to current active connection.
	pqxx::connection *current = nullptr;
	
	/*	Drops current connection; disconects from database.
		Can be called only by destructor, when the lifetime of an object ends.
	*/
	void disconnect();
};