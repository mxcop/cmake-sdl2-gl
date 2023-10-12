#include <atomic>

enum class NetType {
    CLIENT,
    SERVER
};

void net_thread(std::atomic_bool* should_exit, NetType type, const char* addr, const char* port);
