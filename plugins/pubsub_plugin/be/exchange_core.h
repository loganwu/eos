/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#pragma once

#include <vector>

namespace eosio {

template<typename T>
class exchange_core {
public:
    virtual ~exchange_core() {}
    virtual void publish(const std::string &msg) = 0;
};

} // namespace


