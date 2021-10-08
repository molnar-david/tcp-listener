#include "TcpListener.h"

int main()
{
	TcpListener listener{ "127.0.0.1", 54000 };
	if (listener.init() != 0)
	{
		return 0;
	}

	listener.run();

	return 0;
}
