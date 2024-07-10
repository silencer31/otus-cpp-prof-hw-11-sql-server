#include "join_server.h"

#include <boost/algorithm/string.hpp>
#include <iostream>

void ClientSession::do_read()
{
	auto self(shared_from_this());

	socket_.async_read_some( // � ������ �������� async_read_some
		boost::asio::buffer(data_read, max_length),
		[this, self](boost::system::error_code errcode, std::size_t length)
		{
			if (errcode) {
				std::cout << " Session: " << session_id << ". Read data boost system error code: " << errcode.message() << std::endl;

				// �������� ������� � ������������� ���������� ������
				join_server_ptr->close_session(session_id);

				return;
			}

			std::cout << "Session: " << session_id << ". Received: " << length << " bytes\n" << data_read << std::endl;

			// ��������� ���������� ������ ����� \n
			std::vector<std::string> strings;
			boost::split(strings, data_read, boost::is_any_of("\n"));

			for (const std::string& str : strings)
			{
				// ����� �� ��������� ������.
				if (0 == strcmp(str.data(), "exit")) {
					join_server_ptr->close_session(session_id);
					return;
				}

				// ����� �� ��������� ������.
				if (0 == strcmp(str.data(), "shutdown")) {
					//std::this_thread::sleep_for(std::chrono::seconds(1));
					join_server_ptr->shutdown_server(session_id);
					return;
				}

				// ���������� ������ � ��������.
				//async::receive(handle, str.data(), strlen(str.data()));
			}
			
			// ��������� ������� 
			handle_request();
		}
	);
}

void ClientSession::do_write(const std::string& answer)
{
	auto self(shared_from_this());

	prepare_data_send(answer);

	boost::asio::async_write( // �������� async_write, �������� �����.
		socket_, boost::asio::buffer(data_send, answer.length()),
		[this, self](boost::system::error_code errcode, std::size_t length)
		{
			if (errcode) {
				std::cout << "Session: " << session_id << ". Write data boost system error: " << errcode.message() << std::endl;
			}
			else {
				std::cout << "Session: " << session_id << ". To send: " << length << " bytes\n" << data_send << "\n" << std::endl;
			}

			if (shutdown_session_flag) { // ���� �������� ������� �� ����������, ������ �� �������� ������ �� ����.
				return;
			}

			do_read();
		}
	);
}

// ���������� ������.
void ClientSession::shutdown()
{
	if (!socket_.is_open()) {
		return;
	}

	std::cout << "Shutdown process started. Session: " << session_id << std::endl;

	boost::system::error_code ignore;

	socket_.shutdown(
		boost::asio::ip::tcp::socket::shutdown_both,
		ignore
	);

	socket_.close(ignore);

	std::cout << "Shutdown finished. Session: " << session_id << std::endl;
}

// ������� ������ ��� ������ ������ �� ����.
void ClientSession::clear_data_read()
{
	for (size_t i = 0; i < max_length; ++i) {
		data_read[i] = 0;
	}
}

// ���������� ������ ��� �������� �� ����.
void ClientSession::prepare_data_send(const std::string& data)
{
	for (size_t i = 0; i < max_length; ++i) {
		data_send[i] = 0;
	}

	for (size_t i = 0; i < data.length(); ++i) {
		data_send[i] = data.c_str()[i];
		if (i == (max_length - 1)) {
			break;
		}
	}
}

// ��������� ������� �� �������.
void ClientSession::handle_request()
{

}
