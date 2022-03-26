#pragma once
class Server
{
public:
	static uint32_t ConvertIPToInt(std::string ip_string);
	static const int32_t kMaxPacketSize = 1300;


	static void DoServiceLoop(const UDPSocketPtr& server_socket);
	static void ProcessReceivedData(char* receive_buffer, int bytes_received, const SocketAddress& socket_address, const UDPSocketPtr& server_socket, int requests, bool service_running);
	static std::string ReturnCurrentDateAndTime();
	static void ReceivePlayerInputByteStream(UDPSocketPtr server_socket);
	enum class Choice { ECHO = 1, DATEANDTIME, STATS, QUIT, SENDPLAYERBYTESTREAM};
};