#pragma once
#include <scorum/protocol/base.hpp>
#include <scorum/protocol/block_header.hpp>
#include <scorum/protocol/asset.hpp>

#include <fc/utf8.hpp>

namespace scorum {
namespace protocol {

struct author_reward_operation : public virtual_operation
{
    author_reward_operation()
    {
    }
    author_reward_operation(const account_name_type& a, const std::string& p, const asset& reward_)
        : author(a)
        , permlink(p)
        , reward(reward_)
    {
    }

    account_name_type author;
    std::string permlink;
    asset reward; // in SCR or SP
};

struct curation_reward_operation : public virtual_operation
{
    curation_reward_operation()
    {
    }
    curation_reward_operation(const std::string& c, const asset& r, const std::string& a, const std::string& p)
        : curator(c)
        , reward(r)
        , comment_author(a)
        , comment_permlink(p)
    {
    }

    account_name_type curator;
    asset reward; // in SCR or SP
    account_name_type comment_author;
    std::string comment_permlink;
};

struct comment_reward_operation : public virtual_operation
{
    comment_reward_operation()
    {
    }
    comment_reward_operation(const account_name_type& a, const std::string& pl, const asset& p)
        : author(a)
        , permlink(pl)
        , payout(p)
    {
    }

    account_name_type author;
    std::string permlink;
    asset payout; // in SCR or SP
};

struct fill_vesting_withdraw_operation : public virtual_operation
{
    fill_vesting_withdraw_operation()
    {
    }
    fill_vesting_withdraw_operation(const std::string& f, const std::string& t, const asset& w)
        : from_account(f)
        , to_account(t)
        , withdrawn(w)
    {
    }

    account_name_type from_account;
    account_name_type to_account;
    asset withdrawn = asset(0, SP_SYMBOL);
};

struct shutdown_witness_operation : public virtual_operation
{
    shutdown_witness_operation()
    {
    }
    shutdown_witness_operation(const std::string& o)
        : owner(o)
    {
    }

    account_name_type owner;
};

struct witness_miss_block_operation : public virtual_operation
{
    witness_miss_block_operation()
    {
    }
    witness_miss_block_operation(const std::string& o, uint32_t num)
        : owner(o)
        , block_num(num)
    {
    }

    account_name_type owner;
    uint32_t block_num = 0;
};

struct hardfork_operation : public virtual_operation
{
    hardfork_operation()
    {
    }
    hardfork_operation(uint32_t hf_id)
        : hardfork_id(hf_id)
    {
    }

    uint32_t hardfork_id = 0;
};

struct comment_payout_update_operation : public virtual_operation
{
    comment_payout_update_operation()
    {
    }
    comment_payout_update_operation(const account_name_type& a, const std::string& p)
        : author(a)
        , permlink(p)
    {
    }

    account_name_type author;
    std::string permlink;
};

struct return_scorumpower_delegation_operation : public virtual_operation
{
    return_scorumpower_delegation_operation()
    {
    }
    return_scorumpower_delegation_operation(const account_name_type& a, const asset& v)
        : account(a)
        , scorumpower(v)
    {
    }

    account_name_type account;
    asset scorumpower = asset(0, SP_SYMBOL);
};

struct comment_benefactor_reward_operation : public virtual_operation
{
    comment_benefactor_reward_operation()
    {
    }
    comment_benefactor_reward_operation(const account_name_type& b,
                                        const account_name_type& a,
                                        const std::string& p,
                                        const asset& r)
        : benefactor(b)
        , author(a)
        , permlink(p)
        , reward(r)
    {
    }

    account_name_type benefactor;
    account_name_type author;
    std::string permlink;
    asset reward; // in SCR or SP
};

struct producer_reward_operation : public virtual_operation
{
    producer_reward_operation()
    {
    }
    producer_reward_operation(const std::string& p, const asset& v)
        : producer(p)
        , reward(v)
    {
    }

    account_name_type producer;
    asset reward; // in SCR or SP
};

struct expired_contract_refund_operation : public virtual_operation
{
    expired_contract_refund_operation()
    {
    }
    expired_contract_refund_operation(const std::string& o, const asset& v)
        : owner(o)
        , refund(v)
    {
    }

    account_name_type owner;
    asset refund = asset(0, SCORUM_SYMBOL);
};
}
} // scorum::protocol

FC_REFLECT(scorum::protocol::author_reward_operation, (author)(permlink)(reward))
FC_REFLECT(scorum::protocol::curation_reward_operation, (curator)(reward)(comment_author)(comment_permlink))
FC_REFLECT(scorum::protocol::comment_reward_operation, (author)(permlink)(payout))
FC_REFLECT(scorum::protocol::fill_vesting_withdraw_operation, (from_account)(to_account)(withdrawn))
FC_REFLECT(scorum::protocol::shutdown_witness_operation, (owner))
FC_REFLECT(scorum::protocol::witness_miss_block_operation, (owner)(block_num))
FC_REFLECT(scorum::protocol::hardfork_operation, (hardfork_id))
FC_REFLECT(scorum::protocol::comment_payout_update_operation, (author)(permlink))
FC_REFLECT(scorum::protocol::return_scorumpower_delegation_operation, (account)(scorumpower))
FC_REFLECT(scorum::protocol::comment_benefactor_reward_operation, (benefactor)(author)(permlink)(reward))
FC_REFLECT(scorum::protocol::producer_reward_operation, (producer)(reward))
FC_REFLECT(scorum::protocol::expired_contract_refund_operation, (owner)(refund))
