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
        // ilog(trace->id.str());

        //  TODO parse the trace and ..
        //  TODO m_be->act
        // m_be->insert(trace);
        
    }
}

void applied_transaction_storage::on_action_trace(const chain::action_trace& at ) {
            if( true ) {
               idump((fc::json::to_pretty_string(at)));
            //    auto& chain = chain_plug->chain();
            //    auto& be = chain.db();

            //    db.create<action_history_object>( [&]( auto& aho ) {
            //       auto ps = fc::raw::pack_size( at );
            //       aho.packed_action_trace.resize(ps);
            //       datastream<char*> ds( aho.packed_action_trace.data(), ps );
            //       fc::raw::pack( ds, at );
            //       aho.action_sequence_num = at.receipt.global_sequence;
            //       aho.block_num = chain.pending_block_state()->block_num;
            //       aho.block_time = chain.pending_block_time();
            //       aho.trx_id     = at.trx_id;
            //    });
               
            //    auto aset = account_set( at );
            //    for( auto a : aset ) {
            //       record_account_action( a, at );
            //    }
            }
            // if( at.receipt.receiver == chain::config::system_account_name )
            //    on_system_action( at );
            // for( const auto& iline : at.inline_traces ) {
            //    on_action_trace( iline );
            // }
         }

} // namespace
