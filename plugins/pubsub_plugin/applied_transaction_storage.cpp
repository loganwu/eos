#include "applied_transaction_storage.h"
#include <fc/io/json.hpp>

namespace eosio {

applied_transaction_storage::applied_transaction_storage(std::shared_ptr<backend> be):
    m_be(be)
{

}

void applied_transaction_storage::consume(const std::vector<chain::transaction_trace_ptr>& traces)
{
    for (const auto& trace : traces) {
        for (const auto& atrace : trace->action_traces) {
            on_action_trace(atrace);
        }
    }
}

void applied_transaction_storage::on_action_trace(const chain::action_trace& at ) {
    const std::string &act_str = fc::json::to_pretty_string(at);
    // idump((act_str));
    m_be->publish(act_str);
}

} // namespace
