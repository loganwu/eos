#ifndef TRANSACTIONS_TABLE_H
#define TRANSACTIONS_TABLE_H

#include <soci/soci.h>
#include <eosio/chain/block_state.hpp>

namespace eosio {

class transactions_table
{
public:
    transactions_table(std::shared_ptr<soci::session> session);

    void drop();
    void create();
    void insert(const chain::block_state_ptr& b);
    
private:
    std::shared_ptr<soci::session> m_session;
};

} // namespace

#endif // TRANSACTIONS_TABLE_H
