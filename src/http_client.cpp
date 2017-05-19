#include "http_client.h"

#include "protocol/handler_http1.h"
#include "http2/stream.h"
#include "http/client/client_connection.h"
#include "http/client/response.h"

namespace client
{
namespace detail
{
//using connect_callback_t = cra
using connector_ptr = std::shared_ptr<server::connector_interface>;


handler_factory::handler_factory(http::proto_version v, connect_callback_t cb) noexcept
	: v_{v}
	, cb_{std::move(cb)}
{}

void handler_factory::operator()(connector_ptr c)
{
	if(v_ == http::proto_version::HTTP20)
	{
		auto h = std::make_shared<http2::session>();
		c->handler(h);
		c->start(true);
//		cb_(std::move(h));
		assert(0 && "We don't support http2 afan");
	}
	else
	{
		auto h = std::make_shared<server::handler_http1<http::client_traits>>(http::proto_version::HTTP11);
		h->connector(c);
		c->handler(h);
		c->start(true);
		cb_(std::move(h));
	}
}

}
}
