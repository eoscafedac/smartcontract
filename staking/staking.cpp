/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "staking.hpp"
#include <eosiolib/transaction.hpp>

#define BEAN_TOKEN_CONTRACT N(thebeantoken)
#define BEAN_SYMBOL S(4, BEAN)

const uint64_t token_supply_amount = 4'000'000'000'0000;
const uint32_t seconds_per_year = 52 * 7 * 24 * 3600;
const uint64_t min_enterprise_stake = 10'000'000'0000;
const double continuous_rate = 0.05; // 5% annual rate

staking::staking(account_name _self)
    : eosio::contract(_self),
      _enterprises(_self, _self),
      _etp_offer(_self, _self),
      _staker_infos(_self, _self)
{
}

// to stake send amount of bean token with message is account "enterprise"
void staking::transfer(uint64_t sender, uint64_t receiver)
{
    print("\n>>> sender >>>", sender, " - name: ", eosio::name{sender});
    print("\n>>> receiver >>>", receiver, " - name: ", eosio::name{receiver});

    auto transfer_data = eosio::unpack_action_data<transfer_args>();
    if (transfer_data.from == _self || transfer_data.to != _self)
    {
        return;
    }
    eosio_assert(transfer_data.quantity.symbol == eosio::string_to_symbol(4, "BEAN"), "Only accepts BEAN");
    eosio_assert(transfer_data.quantity.is_valid(), "Invalid token transfer");
    eosio_assert(transfer_data.quantity.amount > 0, "Quantity must be positive");

    const account_name enterprise = eosio::string_to_name(transfer_data.memo.c_str());
    eosio_assert(is_account(enterprise), "stake to account does not exist");

    auto etp = _enterprises.find(enterprise);
    eosio_assert(etp != _enterprises.end(), "enterprises does not exist");

    if (transfer_data.from == enterprise)
    {
        // enterprise is stake to itself

        auto total_staked = etp->total_stake.amount + transfer_data.quantity.amount;
        bool approve = total_staked >= min_enterprise_stake ? true : false;
        _enterprises.modify(etp, _self, [&](auto &info) {
            info.total_stake = eosio::asset(total_staked, BEAN_SYMBOL);
            info.is_approve = approve;
            info.last_claim_time = eosio::time_point_sec(now());
        });
    }
    else
    {
        // user stake for enterprise
        eosio_assert(etp->is_approve, "enterprise doesn't meet requirement");

        auto offer = _etp_offer.find(enterprise);
        eosio_assert((offer != _etp_offer.end()) && (offer->is_active == true), "no offer from this enterprise");
        eosio_assert((offer->min_stake <= transfer_data.quantity.amount) && (offer->max_stake >= transfer_data.quantity.amount), "your tranfered amount doesn't match with enterprise offered amount");

        _staker_infos.emplace(_self, [&](auto &info) {
            info.id = _staker_infos.available_primary_key();
            info.staker = transfer_data.from;
            info.enterprise = enterprise;
            info.stake_num = transfer_data.quantity;
            info.coupon = offer->coupon_quantity;
            info.start_at = eosio::time_point_sec(now());
            info.end_at = eosio::time_point_sec(now()) + offer->duration_sec;
            info.updated_at = eosio::time_point_sec(now());
            info.is_done = false;
        });
        _enterprises.modify(etp, _self, [&](auto &info) {
            info.total_stake += transfer_data.quantity;
        });
    }
}

void staking::regetp(account_name enterprise, std::string name, std::string url, uint16_t location, eosio::asset coupon)
{
    require_auth(enterprise);

    eosio_assert(name.size() < 128 && url.size() < 128, "url too long");

    auto etp = _enterprises.find(enterprise);

    if (etp != _enterprises.end())
    {
        _enterprises.modify(etp, enterprise, [&](auto &info) {
            info.name = name;
            info.url = url;
            info.location = location;
            info.coupon_name = eosio::symbol_type(coupon.symbol).name();
        });
    }
    else
    {
        _enterprises.emplace(enterprise, [&](auto &info) {
            info.owner = enterprise;
            info.name = name;
            info.is_approve = false;
            info.url = url;
            info.location = location;
            info.coupon_name = eosio::symbol_type(coupon.symbol).name();
            info.total_unpaid = eosio::asset(0, BEAN_SYMBOL);
            info.last_claim_time = eosio::time_point_sec(now());
        });
    }
}

void staking::setoffer(account_name owner, std::string offer_head, std::string offer_details, uint64_t min_stake, uint64_t max_stake, uint64_t duration_sec, eosio::asset coupon_quantity, bool is_active)
{
    require_auth(owner);
    auto etp = _enterprises.find(owner);

    eosio_assert((etp != _enterprises.end()) && (etp->is_approve == true), "enterprise does not exist");
    eosio_assert(offer_head.size() < 128, "header is too long");
    eosio_assert(min_stake <= max_stake, "min stake should be smaller than max stake");
    eosio_assert(etp->coupon_name == eosio::symbol_type(coupon_quantity.symbol).name(), "symbol mismatch");

    auto offer = _etp_offer.find(owner);

    if (offer != _etp_offer.end())
    {
        _etp_offer.modify(offer, _self, [&](auto &offer) {
            offer.offer_head = offer_head;
            offer.offer_details = offer_details;
            offer.min_stake = min_stake;
            offer.max_stake = max_stake;
            offer.duration_sec = duration_sec;
            offer.is_active = is_active;
        });
    }
    else
    {
        _etp_offer.emplace(_self, [&](auto &offer) {
            offer.owner = owner;
            offer.offer_head = offer_head;
            offer.offer_details = offer_details;
            offer.min_stake = min_stake;
            offer.max_stake = max_stake;
            offer.duration_sec = duration_sec;
            offer.coupon_quantity = coupon_quantity;
            offer.is_active = is_active;
        });
    }
}

// confirm received free cafe from shop
void staking::docoupon(account_name enterprise, account_name account)
{
    require_auth(enterprise);

    eosio_assert(false, "This action will be actived soon");
    // remove stake id once it've done
}

void staking::claimrewards(account_name enterprise)
{
    require_auth(enterprise);

    auto etp = _enterprises.find(enterprise);
    eosio_assert((etp != _enterprises.end()) && (etp->is_approve), "enterprise doesn't exist");

    _enterprises.modify(etp, _self, [&](auto &info) {
        info.total_unpaid = eosio::asset(0, BEAN_SYMBOL);
    });

    eosio::transaction transfer;
    transfer.actions.emplace_back(eosio::permission_level{_self, N(active)}, BEAN_TOKEN_CONTRACT, N(transfer), std::make_tuple(_self, enterprise, etp->total_unpaid, std::string("Staking Reward from eos.cafe")));
    transfer.send(1, _self, false);
}

void staking::refund(account_name staker, uint64_t staker_id)
{
    require_auth(staker);

    auto stake_info = _staker_infos.find(staker_id);
    eosio_assert(stake_info != _staker_infos.end(), "stake id does not exist");
    eosio_assert(stake_info->end_at <= eosio::time_point_sec(now()), "Stake is still in offer duration");
    eosio_assert(stake_info->is_done == false, "Stake id already claim back");

    auto etp = _enterprises.find(stake_info->enterprise);
    eosio_assert((etp != _enterprises.end()) && (etp->is_approve == true), "enterprise does not exist");

    auto staked_duration = eosio::time_point_sec(now()).sec_since_epoch() - etp->last_claim_time.sec_since_epoch();
    // take care with this, the formulate will be changed later to get exactly staked reward
    uint64_t reward_amount = continuous_rate * etp->total_stake.amount * staked_duration / seconds_per_year;

    _enterprises.modify(etp, _self, [&](auto &info) {
        info.total_stake -= stake_info->stake_num;
        info.total_unpaid += eosio::asset(reward_amount, BEAN_SYMBOL);
        info.last_claim_time = eosio::time_point_sec(now());
    });

    _staker_infos.modify(stake_info, _self, [&](auto &info) {
        info.is_done = true;
    });

    eosio::transaction transfer;
    transfer.actions.emplace_back(eosio::permission_level{_self, N(active)}, BEAN_TOKEN_CONTRACT, N(transfer), std::make_tuple(_self, staker, stake_info->stake_num, std::string("Refund staking token from eos.cafe")));
    transfer.send(1, _self, false);
}

#define EOSIO_ABI_EX(TYPE, MEMBERS)                                                                             \
    extern "C"                                                                                                  \
    {                                                                                                           \
        void apply(uint64_t receiver, uint64_t code, uint64_t action)                                           \
        {                                                                                                       \
            auto self = receiver;                                                                               \
            if (code == self || code == N(eosio.token) || code == BEAN_TOKEN_CONTRACT || action == N(onerror))  \
            {                                                                                                   \
                if (action == N(transfer))                                                                      \
                {                                                                                               \
                    eosio_assert(code == N(eosio.token) || code == BEAN_TOKEN_CONTRACT, "Must transfer Token"); \
                }                                                                                               \
                TYPE thiscontract(self);                                                                        \
                switch (action)                                                                                 \
                {                                                                                               \
                    EOSIO_API(TYPE, MEMBERS)                                                                    \
                }                                                                                               \
                /* does not allow destructor of thiscontract to run: eosio_exit(0); */                          \
            }                                                                                                   \
        }                                                                                                       \
    }

EOSIO_ABI_EX(staking, (transfer)(regetp)(setoffer)(docoupon)(claimrewards)(refund))
