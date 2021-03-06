#include <scorum/chain/services/reward_balancer.hpp>
#include <scorum/chain/database/database.hpp>

#include <scorum/chain/schema/reward_balancer_object.hpp>
#include <scorum/chain/schema/scorum_objects.hpp>

namespace scorum {
namespace chain {

dbs_reward::dbs_reward(database& db)
    : base_service_type(db)
{
}

const reward_balancer_object& dbs_reward::create_balancer(const asset& initial_supply)
{
    // clang-format off
    FC_ASSERT(db_impl().find<reward_balancer_object>() == nullptr, "recreation of reward_balancer_object is not allowed");
    FC_ASSERT(initial_supply.symbol() == SCORUM_SYMBOL, "initial supply for reward_pool must in ${1}", ("1", asset(0, SCORUM_SYMBOL).symbol_name()));

    return db_impl().create<reward_balancer_object>([&](reward_balancer_object& rp) {
        rp.balance = initial_supply;
    });
    // clang-format on
}

const asset& dbs_reward::increase_ballance(const asset& delta)
{
    update([&](reward_balancer_object& pool) {
        switch (delta.symbol())
        {
        case SCORUM_SYMBOL:
            FC_ASSERT(delta >= asset(0, SCORUM_SYMBOL));
            pool.balance += delta;
            break;
        default:
            FC_ASSERT(false, "invalid symbol");
        }
    });

    return get().balance;
}

// clang-format off
const asset dbs_reward::take_block_reward()
{
    const auto& pool = get();

    const auto current_per_day_reward = pool.current_per_block_reward * SCORUM_BLOCKS_PER_DAY;

    asset real_per_block_reward(0, SCORUM_SYMBOL);

    update([&](reward_balancer_object& pool) 
    {
        asset delta = pool.current_per_block_reward * SCORUM_ADJUST_REWARD_PERCENT / 100;

        delta = std::max(SCORUM_MIN_PER_BLOCK_REWARD, delta);

        if (pool.balance > current_per_day_reward * SCORUM_REWARD_INCREASE_THRESHOLD_IN_DAYS)
        {
            // recalculate
            pool.current_per_block_reward += delta;
        }
        else if (pool.balance < current_per_day_reward * SCORUM_GUARANTED_REWARD_SUPPLY_PERIOD_IN_DAYS)
        {
            // recalculate
            pool.current_per_block_reward = std::max(SCORUM_MIN_PER_BLOCK_REWARD, pool.current_per_block_reward - delta);
        }
        else
        {
            // use current_perblock_reward
        }

        if (pool.balance >= pool.current_per_block_reward)//balance must not be negative
        {
            pool.balance -= pool.current_per_block_reward;
            real_per_block_reward = pool.current_per_block_reward;
        }
        else if(pool.balance.amount > 0)
        {
            real_per_block_reward = pool.balance;
            pool.balance.amount = 0;
        }
    });

    return real_per_block_reward;
}
// clang-format on

} // namespace chain
} // namespace scorum
