#include "irreversible_block_storage.h"
#include <fc/io/json.hpp>

namespace eosio {

irreversible_block_storage::irreversible_block_storage(std::shared_ptr<backend> be):
    m_be(be)
{

}

void irreversible_block_storage::consume(const std::vector<chain::block_state_ptr>& blocks)
{
    for (const auto& block : blocks) {
        const std::string &block_str = fc::json::to_pretty_string(block);
        // idump((block_str));
        m_be->publish(block_str);
    }
}

} // namespace
