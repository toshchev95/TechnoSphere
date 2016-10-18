//#pragma once
#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include "DateBaseOfClients.cpp"

static size_t lengthLogin = 4,
				 lengthPassword = 4;
class Identification: public DateBaseOfClients
{

public:
	//Identification(void);
	//~Identification(void);
	//bool authentication(const std::string &, const std::string &);
	bool authentication(const std::string & login, 
						const std::string & password)
	{
		bool bLogPas = (login.length() > lengthLogin) &&
							 (password.length() > lengthPassword);
		if (!bLogPas)
			return false;
		int bound = login.find(" ");
		std::string command = login.substr(0, bound);
		bLogPas = isExistUserInDB(login.substr(bound + 1), 
								  password.substr(0, password.length()-1));
		if (command == "login")
			return bLogPas;	
		else if (!bLogPas)
		{
			createUserInDB( login.substr(bound + 1), 
							password.substr(0, password.length()-1));
			return true;
		}
		return false;
	}

	
private:
	bool flag;
};

/*Identification::Identification(void)
{
}
Identification::~Identification(void)
{
}

*/

