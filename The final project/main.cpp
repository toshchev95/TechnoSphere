#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif



#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/enable_shared_from_this.hpp>
#include "Identification.cpp"

//using namespace std;
using namespace boost::asio;
using namespace boost::posix_time;
io_service service;

class talk_to_client;
typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> array;
array clients;

struct Client{
	//ip::tcp::socket _sock;
	std::string _name;
	std::string _password;
	int _id;
	Client(//ip::tcp::socket sock,
		   std::string name,
		   std::string password,
		   int id):
	//_sock(sock), 
		_id(id)
	{
		int bound = name.find(" ");
		std::string command = name.substr(0, bound);
		_name = name.substr(bound + 1);
		_password = password.substr(0, password.length()-1);
	}
};

extern std::vector<Client> clients_messange;

#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)

void update_clients_changed();

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>, 
							  boost::noncopyable, Identification {
    typedef talk_to_client self_type;
	talk_to_client() : sock_(service), started_(false),
					   timer_(service), clients_changed_(false) {
		read_buffer_[0] = '\0';
		write_buffer_[0] = '\0';
	}
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<talk_to_client> ptr;

    void start() {
        started_ = true;
		clients.push_back(shared_from_this());
		last_ping = boost::posix_time::microsec_clock::local_time();
		// first, we wait for client to login
        do_read();
    }
    static ptr new_() {
        ptr new_(new talk_to_client);
        return new_;
    }
    void stop() {
        if ( !started_) return;
        started_ = false;
        sock_.close();

		ptr self = shared_from_this();
		array::iterator it = std::find(clients.begin(), clients.end(), self);
		clients.erase(it);
		update_clients_changed();
    }
	bool started() const { return started_;}
    ip::tcp::socket & sock() { return sock_;}
	std::string username() const { return username_;}
	void set_clients_changed() { clients_changed_ = true;}
private:
    void on_read(const error_code & err, size_t bytes) {
		if (err) stop();
		if ( !started() ) return;
		// process the msg
		std::string msg(read_buffer_, bytes);

		if ( (msg.find("login ") == 0) ||
			 (msg.find("Sign ") == 0)) on_login(msg);
		else if (msg.find("ping") == 0) on_ping();
		else if (msg.find("ask_clients") == 0) on_clients();
		else if (msg.find("message") == 0) {
			int bound = msg.find(" ") + 1;
			on_message(msg.substr(bound));
		}
		else if (msg.find("quit") == 0) {
			stop();
			do_write(msg);
		}
		else {
			std::cerr<<"invalid msg "<<msg<<std::endl;
			do_write("wrong command\n");
			//stop();
		}
    }

	void on_login(const std::string & msg){
		int plus = msg.find("+"),
			space = msg.find(" ");
		const std::string login = msg.substr(0, plus),
						  password = msg.substr(plus + 1);

		if ((plus != std::string::npos) && 
			(authentication(login, password))){
			
			Client a(//sock_, 
				login, password, numberClients);

			clients_messange.push_back(a);

			std::cout << msg.substr(space + 1, plus - space - 1)
				<<" logged in"<<std::endl;
			do_write("login ok\n");
			update_clients_changed();

		}
		else
			do_write("Wrong input login and password\n");
	}

	void on_message(const std::string & otherClient){
		// main func
		int bound = otherClient.find(" ");
		std::string nameClient = otherClient.substr(0, bound),
					message = otherClient.substr(bound + 1);
		for (std::vector<messadesClient>::iterator it = DBClients_send_message.begin();
			it != DBClients_send_message.end(); ++it)
		{
			if ((*it).first == nameClient)
				it->second.push_back(std::make_pair(name_thisClient, message));
		}
	}

	void on_ping(){
		do_write(clients_changed_? "ping client_list_changed\n" : "ping ok\n");
		clients_changed_ = false;
	}

	void on_clients(){
		std::string msg;
		for (array::iterator b = clients.begin(), e = clients.end(); b != e; ++b){
			msg += (*b)->username() + " ";
		}
		do_write("clients " + msg + "\n");
	}

	void do_ping(){
		do_write("ping\n");
	}

	void do_ask_clients(){
		do_write("ask_clients\n");
	}

	void on_check_ping(){
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		if ( (now - last_ping).total_milliseconds() > 5000 ) {
			std::cout<< "stopping "<<username_<<" - no ping in time"<<std::endl;
			stop();
		}
		last_ping = boost::posix_time::microsec_clock::local_time();
	}

	void post_check_ping(){
		timer_.expires_from_now(boost::posix_time::millisec(50000));
		timer_.async_wait(MEM_FN(on_check_ping));
	}

    void on_write(const error_code & err, size_t bytes) {
        do_read();
    }
    void do_read() {
        async_read(sock_, buffer(read_buffer_), 
                   MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
		//post_check_ping();
    }
    void do_write(const std::string & msg) {

        std::copy(msg.begin(), msg.end(), write_buffer_);
        sock_.async_write_some( buffer(write_buffer_, msg.size()), 
                                MEM_FN2(on_write,_1,_2));
    }
    size_t read_complete(const boost::system::error_code & err, size_t bytes) {
        if ( err) return 0;
        bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
    }
private:
    ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
	std::string username_;
	deadline_timer timer_;
	boost::posix_time::ptime last_ping;
	bool clients_changed_;
	// process send/receive messages;
	const std::string name_thisClient;
};

void update_clients_changed() {
	for (array::iterator b = clients.begin(), e = clients.end(); b != e; ++b){
		(*b)->set_clients_changed();
	}
}

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::address::from_string("127.0.0.1") , 35200));//ip::tcp::v4()

void handle_accept(talk_to_client::ptr client, const boost::system::error_code & err) {
    client->start();
	//boost::this_thread::sleep( boost::posix_time::millisec(millisec(rand() % 7000)));
    talk_to_client::ptr new_client = talk_to_client::new_();
    acceptor.async_accept(new_client->sock(), boost::bind(handle_accept,new_client,_1));
}


int main(int argc, char* argv[]) {
    talk_to_client::ptr client = talk_to_client::new_();
    acceptor.async_accept(client->sock(), boost::bind(handle_accept,client,_1));
    service.run();
	std::cout<<"THE END hasn't ever come, that the winter is coming soon"<<std::endl;
}
