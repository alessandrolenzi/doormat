#pragma once

#include "src/protocol/http_handler.h"

class connector_interface;

class mock_handler : public server::http_handler
{
	void do_write() override;
	void on_connector_nulled() override;

public:
	bool start() noexcept override;
	bool should_stop() const noexcept override;
	bool on_read(const char*, unsigned long) override;
	bool on_write(std::string& chunk) override;
	void trigger_timeout_event() override;
	virtual std::vector<std::pair<std::function<void()>, std::function<void()>>> write_feedbacks() override;

};
