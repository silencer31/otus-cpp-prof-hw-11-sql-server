#pragma once

#include "client_session.h"

using boost::asio::ip::tcp;

using session_map = std::map<int, session_shared>; // ��� ��������� ������ �������������.


/**
* @brief ����� - ������ ��������� ������� ����������� � ���������� �������� �������������.
*/
class JoinServer : public std::enable_shared_from_this<JoinServer>
{
public:
	JoinServer() = delete;

	JoinServer(boost::asio::io_context& io_context, unsigned short port, const char* file_path);
	
	~JoinServer();

	/**
	* �������� ������� �� ���������� �������.
	* @param session_id ���������� id ������, �� ������� �������� �������.
	*/
	void shutdown_server(int session_id);

	/**
	* �������� ������.
	* @param session_id ���������� id ������.
	*/
	void close_session(int session_id);


private: // methods
	/**
	* ���������� � �������� ����� �����������.
	*/
	void do_accept();
	
private: // data
	int session_number{ 0 };   // ���-�� ������.

	bool shutdown_flag{ false }; // ����, ��� ����� ��������� ������ �������.	

	tcp::acceptor acceptor_;

	session_map sessions; // ��������� ������.			
};