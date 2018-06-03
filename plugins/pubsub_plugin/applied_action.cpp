#include "applied_action.h"
#include <fc/io/json.hpp>

namespace eosio {

applied_action::applied_action(std::shared_ptr<backend> be):
    m_be(be)
{

}

void applied_action::consume(const std::vector<pubsub_message::actions_result_ptr>& results)
{
    for (const auto& result : results) {
        const std::string &result_str = fc::json::to_pretty_string(result);
        // idump((result_str));
        m_be->publish(result_str);
    }
}

} // namespace
