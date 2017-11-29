#pragma once

#include <scorum/chain/dbs_base_impl.hpp>

namespace scorum {
namespace chain {

using scorum::protocol::asset;

class reward_pool_object;

class dbs_reward : public dbs_base
{
    friend class dbservice_dbs_factory;

protected:
    explicit dbs_reward(database& db);

public:
    // return actual balance after increasing
    const asset& increase_pool_ballance(reward_pool_object& pool, const asset& delta);

    const asset& take_block_reward(reward_pool_object& pool);
};

} // namespace chain
} // namespace scorum