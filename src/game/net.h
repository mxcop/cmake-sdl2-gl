#include <atomic>
#include <vector>
#include <string>

enum class NetType {
    CLIENT,
    SERVER
};

void net_thread(std::atomic_bool* should_exit, std::atomic_bool* msg_queued, std::vector<std::string>* msg_log, const char* msg, NetType type, const char* addr, const char* port);
