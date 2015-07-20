#include "Dispatch.h"
#include <boost/asio.hpp>

boost::asio::io_service _dispatchIo;

void dispatch_main(const std::function<void()>& handler) {
    _dispatchIo.post(handler);
}

void dispatch() {
    _dispatchIo.run();
    _dispatchIo.reset();
}