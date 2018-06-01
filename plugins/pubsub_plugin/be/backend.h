#ifndef BACKEND_H
#define BACKEND_H

#include <memory>
#include <mutex>
#include <soci/soci.h>

namespace eosio {

class backend
{
public:
    backend(const std::string& uri);

    void wipe();

    void publish(const std::string &msg);
private:
    mutable std::mutex m_mux;
    // std::shared_ptr<soci::session> m_session;
    // std::unique_ptr<accounts_table> m_accounts_table;
};

} // namespace

#endif // BACKEND_H
