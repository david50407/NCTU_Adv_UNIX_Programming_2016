#include <signal_handler.hpp>

using Childish::SignalHandler;

std::map<int, std::list<SignalHandler::Callback>> SignalHandler::__subscribers;
