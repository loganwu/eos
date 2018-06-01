#include "backend.h"

namespace eosio {

backend::backend(const std::string &uri)
{
}

void backend::wipe()
{
    std::unique_lock<std::mutex> lock(m_mux);
}

void backend::publish(const std::string &msg) {

}

} // namespace
