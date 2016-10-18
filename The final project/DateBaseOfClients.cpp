//#pragma once
#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

//using namespace std;
#include <iostream>
#include <vector>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <thread> 

const std::string _filename = "input.txt";

typedef std::pair<const std::string, const std::string> LogPas;
typedef std::pair<std::vector<LogPas>::iterator, int> referID;
// additional DB of informations about client
typedef std::vector<std::pair<const std::string, std::string>> info;
typedef std::pair<std::vector<LogPas>::iterator, info> infoID;
typedef std::pair<const std::string, std::vector<std::pair<const std::string, std::string>>> messadesClient;

struct Message{
	int id_message;
	std::string senderID, recivierID;
	std::string text;
	//std::chrono::time_point time;

};
typedef std::vector<Message> list;

/*
	Есть БД по парам имя - пароль, в соответствии этой БД
	существует другая БД - БД идентификаторов.
	По спец. команде или по исключению, выгружаем сохранившеюся БД
	в файл. При загрузке программы загружаем БД из файла.
*/

// main
static std::vector<LogPas> DBClients_LogPas_;
	// reference Login_Password -> Identificator of client
static std::vector<referID> DBClients_referID_;
	// additional DB Info
static std::vector<infoID> DBClients_infoID_;
	// BD message
static std::vector<messadesClient> DBClients_send_message;
static int numberClients = 0;
class DateBaseOfClients{
	// members of class
	int ID_client;
public:
	DateBaseOfClients()
	{
		read_clients_from_file();
	}
	~DateBaseOfClients()
	{
		write_clients_to_file();
	}
	//bool isExistUserInDB(const std::string &, const std::string &);
	//void createUserInDB(const std::string &, const std::string &);

	void createUserInDB(const std::string & login, 
						const std::string & password)
	{
		try
		{
			LogPas record = std::make_pair(login, password);
			
			ID_client = numberClients++;
			DBClients_referID_.push_back(std::make_pair(DBClients_LogPas_.end(), ID_client));
			
			DBClients_LogPas_.push_back(record);
			write_client_to_file(record);
			
			return;
		}
		catch(...)
		{
			write_clients_to_file();
			return;
		}
	}

	bool isExistUserInDB ( const std::string & loginUser,
						   const std::string & password )
	{
		try
		{
			for (std::vector<LogPas>::iterator it = DBClients_LogPas_.begin();
				it != DBClients_LogPas_.end(); ++it)
			{
				if (( (*it).first == loginUser) && ( (*it).second == password))
					return true;
			}
			return false;
		}
		catch(...)
		{
			write_clients_to_file();
			return false;
		}
	}

private:
	void read_clients_from_file()
	{
		int boundString, boundLogin;

		std::ifstream in(_filename);
		std::string s((std::istreambuf_iterator<char>(in)), 
					   std::istreambuf_iterator<char>());
		in.close();

		while ((boundString = s.find("\n")) != std::string::npos)
		{
			std::string str = s.substr(0, boundString);
			boundLogin = str.find(" ");

			LogPas* logPas = new LogPas(str.substr(0, boundLogin).c_str(), 
										str.substr(boundLogin + 1).c_str());
			DBClients_LogPas_.push_back(*logPas);
			// DBClients_referID_.init()

			s = s.substr(boundString + 1);
		}

	}

	void write_clients_to_file()
	{
		std::string input;
		/*
		for (std::vector<referID>::iterator it = DBClients_referID_.begin();
			it != DBClients_referID_.end(); ++it)
		{
			input += (*it).first->first + " " + (*it).first->first + " " + itoa((*it).second) + "\n";
		}
		*/
		for (std::vector<LogPas>::iterator it = DBClients_LogPas_.begin();
			it != DBClients_LogPas_.end(); ++it)
		{
			input += (*it).first + " " + (*it).second +  "\n";
		}

		std::ofstream out(_filename);
		out << input;
		out.close();
	}
	void write_client_to_file(LogPas & record)
	{
		std::string input;

		input = record.first + " " + record.second + "\n";

		std::ofstream out(_filename, std::ios_base::app);
		out << input;
		out.close();
	}
};