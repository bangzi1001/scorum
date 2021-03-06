#pragma once

#include <boost/multi_index/composite_key.hpp>

#include <fc/shared_containers.hpp>

#include <scorum/chain/schema/scorum_object_types.hpp>
#include <scorum/account_statistics/schema/metrics.hpp>
#include <scorum/common_statistics/base_bucket_object.hpp>

//
// Plugins should #define their SPACE_ID's so plugins with
// conflicting SPACE_ID assignments can be compiled into the
// same binary (by simply re-assigning some of the conflicting #defined
// SPACE_ID's in a build script).
//
// Assignment of SPACE_ID's cannot be done at run-time because
// various template automagic depends on them being known at compile
// time.
//
#ifndef ACCOUNT_STATISTICS_SPACE_ID
#define ACCOUNT_STATISTICS_SPACE_ID 10
#endif

namespace scorum {
namespace account_statistics {

using namespace scorum::chain;

enum account_statistics_plugin_object_types
{
    bucket_object_type = (ACCOUNT_STATISTICS_SPACE_ID << 8),
    activity_bucket_object_type
};

struct bucket_object : public common_statistics::base_bucket_object, public object<bucket_object_type, bucket_object>
{
    CHAINBASE_DEFAULT_DYNAMIC_CONSTRUCTOR(bucket_object, (account_statistic))

    id_type id;

    fc::shared_map<account_name_type, account_metric> account_statistic;
};
typedef bucket_object::id_type bucket_id_type;

struct activity_bucket_object : public common_statistics::base_bucket_object,
                                public object<activity_bucket_object_type, activity_bucket_object>
{
    CHAINBASE_DEFAULT_CONSTRUCTOR(activity_bucket_object)

    id_type id;

    uint32_t active_market_accounts = 0; ///< Active market accounts in the bucket
    uint32_t active_forum_accounts = 0; ///< Active forum accounts in the bucket
    uint32_t active_market_and_forum_accounts = 0; ///< Active accounts in both the market and the forum
};
typedef activity_bucket_object::id_type activity_bucket_id_type;

struct by_id;
typedef shared_multi_index_container<bucket_object,
                                     indexed_by<ordered_unique<tag<by_id>,
                                                               member<bucket_object,
                                                                      bucket_id_type,
                                                                      &bucket_object::id>>,
                                                ordered_unique<tag<common_statistics::by_bucket>,
                                                               composite_key<bucket_object,
                                                                             member<common_statistics::
                                                                                        base_bucket_object,
                                                                                    uint32_t,
                                                                                    &common_statistics::
                                                                                        base_bucket_object::seconds>,
                                                                             member<common_statistics::
                                                                                        base_bucket_object,
                                                                                    fc::time_point_sec,
                                                                                    &common_statistics::
                                                                                        base_bucket_object::open>>>>>
    bucket_index;
} // namespace account_statistics
} // namespace scorum

FC_REFLECT_DERIVED(scorum::account_statistics::bucket_object,
                   (scorum::common_statistics::base_bucket_object)(scorum::account_statistics::account_metric),
                   (id))

CHAINBASE_SET_INDEX_TYPE(scorum::account_statistics::bucket_object, scorum::account_statistics::bucket_index)

// clang-format off

FC_REFLECT_DERIVED(
    scorum::account_statistics::activity_bucket_object, (scorum::common_statistics::base_bucket_object),
    (id)
    (active_market_accounts)
    (active_forum_accounts)
    (active_market_and_forum_accounts)
)

// clang-format on
