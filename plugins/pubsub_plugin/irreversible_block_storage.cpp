#include "irreversible_block_storage.h"
#include <fc/io/json.hpp>

namespace eosio {

irreversible_block_storage::irreversible_block_storage(std::shared_ptr<backend> be):
    m_be(be)
{

}

void irreversible_block_storage::consume(const std::vector<chain::block_state_ptr>& blocks)
{
    for (const auto& block : blocks)
    {
        idump((fc::json::to_pretty_string(block)));
        // ilog(block->id.str());

        //  TODO parse the block and ..
        //  TODO m_be->act
        // m_be->insert(block);
        
    }
}

} // namespace
