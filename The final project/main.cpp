#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif



#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/enable_shared_from_this.hpp>
//using namespace std;
using namespace boost::asio;
using namespace boost::posix_time;
io_service service;

class talk_to_client;
typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> array;
array clients;


#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)

void update_clients_changed();

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>, boost::noncopyable {
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
        /*if ( !err) {
            std::string msg(read_buffer_, bytes);
			if (msg == "quit\n")	{
				stop();
			}
            // echo message back, and then stop
            do_write(msg);
        }
        stop();*/
		if (err) stop();
		if ( !started() ) return;
		
		// process the msg
		std::string msg(read_buffer_, bytes);
		int bound = msg.find(" ") + 1,
			boundEnd = msg.length() - bound - 1;
		if (msg.find("login ") == 0) on_login(msg.substr(bound, boundEnd));
		else if (msg.find("ping") == 0) on_ping();
		else if (msg.find("ask_clients") == 0) on_clients();
		else if (msg.find("message") == 0) {
			on_message(msg.substr(bound, boundEnd));
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

	void on_message(const std::string & otherClient){
		// main func
		int bound = otherClient.find(" ");
		std::string nameClient = otherClient.substr(0, bound),
					message = otherClient.substr(bound + 1);
		
		for (std::vector<client_ptr>::iterator it = clients.begin();
			 it != clients.end(); ++it)
		{
			if ((*it)->username_ == nameClient)
			{
				do_write(message, (*it)->sock_);
				std::cout<<this->username_<<" sent to "
					<<nameClient<<" "<<"\""<<message<<"\""<<std::endl;
				break;
			}

		}
		stop();
		//*this;
	}

	void on_login(const std::string & msg){

		
		// parse
		int bound = msg.find("+");
		std::string name_ = msg.substr(0, bound),
			password= msg.substr(bound + 1, msg.length()-1);
		this->username_ = name_;
		this->password_ = password;

		//std::istringstream in(msg);
		//in >> username_ >> username_;
		std::cout << username_ <<" logged in"<<std::endl;
		do_write("login ok\n");
		update_clients_changed();
	}

	void on_ping(){
		do_write(clients_changed_? "ping client_list_changed\n" : "ping ok\n");
		clients_changed_ = false;
	}

	void on_clients(){
		std::string msg;
		//::cout << clients[0]->username_ << '!' <<  std::endl;
		for (array::iterator b = clients.begin(), e = clients.end(); b != e; ++b){
			msg += (*b)->username_ + " ";
		//	std::cout<<"! "<<(*b)->username_<<std::endl;
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
	void do_write(const std::string & msg, ip::tcp::socket &socket) {

        std::copy(msg.begin(), msg.end(), write_buffer_);
        socket.async_write_some( buffer(write_buffer_, msg.size()), 
                                MEM_FN2(on_write,_1,_2));
    }
    size_t read_complete(const boost::system::error_code & err, size_t bytes) {
        if ( err) return 0;
        bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
    }
protected:
    ip::tcp::socket sock_;
	std::string m_login;
private:
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
	std::string username_;
	std::string password_;
	deadline_timer timer_;
	boost::posix_time::ptime last_ping;
	bool clients_changed_;
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
	std::cout<<"THE END hasn't ever come, but the winter is coming soon"<<std::endl;
}
