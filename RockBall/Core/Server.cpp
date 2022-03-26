#include "SocketWrapperPCH.hpp"
#include "Server.hpp"


//Client can contact the server that offers three services
//Echo - just repeats back what you send
//DateAndTime - current date and time
//Stats - returns the number of queries that the server has handled

int main()
{
	SocketUtil::StaticInit();
	UDPSocketPtr server_socket = SocketUtil::CreateUDPSocket(INET);
	SocketAddress server_address = SocketAddress(Server::ConvertIPToInt("127.0.0.1"), 50005);
	server_socket->Bind(server_address);
	server_socket->SetNonBlockingMode(false);
	Server::ReceivePlayerInputByteStream(server_socket);
	//Server::DoServiceLoop(server_socket);
}

uint32_t Server::ConvertIPToInt(std::string ip_string)
{
	u_int int_ip = 0;

	for(u_int i=0; i < ip_string.length();++i)
	{
		if(ip_string[i] == '.')
		{
			ip_string[i] = ' ';
		}
	}

	vector<u_int> array_tokens;
	std::stringstream ss(ip_string);
	u_int temp;
	while(ss >> temp)
	{
		array_tokens.emplace_back(temp);
	}

	for(u_int i =0; i < array_tokens.size(); ++i)
	{
		int_ip += (array_tokens[i] << ((3 - i) * 8));
	}

	return int_ip;
}



void Server::DoServiceLoop(const UDPSocketPtr& server_socket)
{
	bool service_running = true;
	char receive_buffer[kMaxPacketSize];

	//set the buffer to null
	char* begin = receive_buffer;
	char* end = begin + sizeof(receive_buffer);
	std::fill(begin, end, 0);

	SocketAddress sender_address;
	int bytes_received = 0;
	int requests = 0;

	while(service_running)
	{
		bytes_received = server_socket->ReceiveFrom(receive_buffer, sizeof(receive_buffer), sender_address);
		if(bytes_received > 0)
		{
			requests++;
			ProcessReceivedData(receive_buffer, bytes_received, sender_address, server_socket, requests, service_running);
		}
	}
}

void Server::ProcessReceivedData(char* receive_buffer, int bytes_received, const SocketAddress& socket_address, const UDPSocketPtr& server_socket, int requests, bool service_running)
{
	std::cout << "Got " << bytes_received << " from " << socket_address.ToString() << std::endl;
	std::cout << "The message is: " << receive_buffer << std::endl;

	char response_data[kMaxPacketSize] = "";
	int choice = atoi(receive_buffer);
	string current_date_time = "";
	std::string requests_string = "";
	std::string quit_string = "QUIT";

	switch(static_cast<Choice>(choice))
	{
	case Choice::ECHO:
		std::cout << "ECHO request" << std::endl;
		strcpy_s(response_data, receive_buffer);
		break;
	case Choice::DATEANDTIME:
		std::cout << "DATEANDTIME request" << std::endl;
		current_date_time = ReturnCurrentDateAndTime();
		current_date_time.copy(response_data, current_date_time.length(), 0);
		break;
	case Choice::STATS:
		std::cout << "STATS request" << std::endl;
		requests_string = std::to_string(requests);
		requests_string.copy(response_data, requests_string.length(), 0);
		break;
	case Choice::QUIT:
		std::cout << "QUIT request" << std::endl;
		quit_string.copy(response_data, quit_string.length(), 0);
		service_running = false;
		break;
	}
	int bytes_send = server_socket->SendTo(response_data, sizeof(response_data), socket_address);
}

std::string Server::ReturnCurrentDateAndTime()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}

void Server::ReceivePlayerInputByteStream(UDPSocketPtr server_socket)
{
	Player* receiver = new Player();
	SocketAddress sender_address;

	char* temporary_buffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	int bytes_received = server_socket->ReceiveFrom(temporary_buffer, kMaxPacketSize, sender_address);
	InputMemoryStream stream(temporary_buffer, static_cast<uint32_t>(bytes_received));
	receiver->Read(stream);
	std::cout << "Received: " << bytes_received;
	receiver->toString();
}

