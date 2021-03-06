#ifndef DOORMAT_REQUEST_H
#define DOORMAT_REQUEST_H

#include <memory>
#include <functional>
#include <boost/asio/io_service.hpp>

#include "../http_request.h"
#include "../message_error.h"

namespace http2
{
class stream;
}


namespace server
{
template<typename handler_traits>
class handler_http1;
}

namespace http 
{
/** \brief the request class provides to the library user a mean through which it can subscribe to events related
 *  to the request.
 * */
class server_connection;
struct server_traits;

class request : public std::enable_shared_from_this<request> 
{
	friend class server::handler_http1<server_traits>;
	friend class http2::stream;
public:
	request(std::shared_ptr<server_connection>, boost::asio::io_service&io);
    //non-copiable object.
    request(const request&) = delete;
    request& operator=(const request&) = delete;

    /** Callback types. */
	using data_t = std::unique_ptr<const char[]>;
    using headers_callback_t = std::function<void(std::shared_ptr<request>)>;
	using body_callback_t = std::function<void(std::shared_ptr<request>, data_t char_array, size_t size)>;
    using trailer_callback_t = std::function<void(std::shared_ptr<request>, std::string k, std::string v)>;
    using finished_callback_t = std::function<void(std::shared_ptr<request>)>;
    using error_callback_t = std::function<void(std::shared_ptr<request>, const http::connection_error &err)>;

    void on_headers(headers_callback_t);
    void on_body(body_callback_t);
    void on_error(error_callback_t);
    void on_trailer(trailer_callback_t);
    void on_finished(finished_callback_t);

	/** Preamble manipulation methods */
	http::http_request & preamble() { return _preamble; }
	void clear_preamble() { _preamble = http::http_request{}; }

	/** Connection retrieve method*/
	std::shared_ptr<http::server_connection> get_connection();

private:

    /** External handlers allowing to trigger events to be communicated to the user */
    void headers(http::http_request &&);
	void body(data_t d, size_t);
	void error(http::connection_error err);
	void trailer(std::string&&, std::string&&);
    void finished();

	bool ended() { return request_terminated;  }

	bool request_terminated{false};

	/* User registered events */
	headers_callback_t headers_callback;
	body_callback_t body_callback;
	error_callback_t error_callback;
	trailer_callback_t trailer_callback;
	finished_callback_t finished_callback;

	std::shared_ptr<server_connection> connection_keepalive;
    http::connection_error conn_error{http::error_code::success};
	http::http_request _preamble;

	boost::asio::io_service &io;
};


}


#endif //DOORMAT_REQUEST_H
