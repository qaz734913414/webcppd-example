#include <Poco/DeflatingStream.h>



#include "gzip.hpp"

namespace webcpp {
	namespace helloworld {

		void gzip::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
		{
			response.set("Content-Encoding", "gzip");
			response.setContentType("text/plain");
			Poco::DeflatingOutputStream out(response.send(), Poco::DeflatingStreamBuf::STREAM_GZIP);
			out << "hello world .";
			out.close();

		}


	}
}