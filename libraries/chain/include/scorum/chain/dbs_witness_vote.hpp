#pragma once

#include <scorum/chain/dbs_base_impl.hpp>

#include <scorum/chain/witness_objects.hpp>

namespace scorum {
namespace chain {

struct witness_vote_service_i
{
    virtual void create(witness_id_type witness_id, account_id_type vouter_id) = 0;
    virtual bool is_exists(witness_id_type witness_id, account_id_type vouter_id) const = 0;
    virtual void remove(witness_id_type witness_id, account_id_type vouter_id) = 0;
};

class dbs_witness_vote : public witness_vote_service_i, public dbs_base
{
    friend class dbservice_dbs_factory;

protected:
    explicit dbs_witness_vote(database& db);

public:
    virtual void create(witness_id_type witness_id, account_id_type vouter_id) override;
    virtual bool is_exists(witness_id_type witness_id, account_id_type vouter_id) const override;
    virtual void remove(witness_id_type witness_id, account_id_type vouter_id) override;
};

} // namespace chain
} // namespace scorum
