#ifdef _WIN32
#define _WIN32_WINNT 0xA00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <iostream>

std::vector<char> vBuffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket)
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length)
		{
			if(!ec)
			{
				std::cout << "\n\nRead " << length << " bytes\n\n";

				for (int i = 0; i < length; i++)
					std::cout << vBuffer[i];

				GrabSomeData(socket);
			}
		}
	);
}

int main()
{
	setlocale(LC_ALL, "");

	asio::error_code ec;
	asio::io_context context;

	asio::io_context::work idlework(context);
	std::thread thrContext = std::thread([&]() {context.run(); });

	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("142.250.217.78", ec), 80);

	asio::ip::tcp::socket socket(context);
	socket.connect(endpoint, ec);

	if (!ec)
	{
		std::cout << "Connected" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
	}

	if (socket.is_open())
	{
		std::string sRequest =
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close \r\n\r\n";

		GrabSomeData(socket);

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		/*socket.wait(socket.wait_read);

		size_t bytes = socket.available();
		std::cout << "Bytes Available: " << bytes << std::endl;

		if (bytes > 0)
		{
			std::vector<char> vBuffer(bytes);
			socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);

			for (auto c : vBuffer)
				std::cout << c;
		}*/

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2000ms);
	}

	system("pause");
	return 0;
}