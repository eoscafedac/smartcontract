/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/symbol.hpp>
#include <eosiolib/types.hpp>

using namespace std;

#define BEAN_SYMBOL S(4, BEAN)
#define NOTHING_SYMBOL S(4, NA)

class staking : public eosio::contract
{
 public:
   staking(account_name self);

   void transfer(uint64_t sender, uint64_t receiver);
 // @abi action
   void regetp(account_name enterprise, std::string name, std::string url, uint16_t location, eosio::asset coupon);
 // @abi action
   void setoffer(account_name owner, std::string offer_head, std::string offer_details, eosio::asset min_stake, eosio::asset max_stake, uint64_t duration_sec, eosio::asset coupon_quantity, bool is_active);
 // @abi action
   void claimrewards(account_name enterprise);
 // @abi action
   void docoupon(account_name account, uint64_t staker_id);
 // @abi action
   void refund(account_name staker, uint64_t staker_id);
    // @abi action
  void initialize();
 // @abi action
  void setglobal(uint64_t id, uint64_t val);

   struct transfer_args
   {
      account_name from;
      account_name to;
      eosio::asset quantity;
      std::string memo;
   };

 private:

 enum stake_status : uint8_t
  {
    STAKING = 0, // in staking
    REFUND = 1, // staker already claim fund back
    DONE = 2, // enterprice already do counpon
  };

   //@abi table globals
  struct global
  {
    uint64_t id;
    uint64_t val;

    uint64_t primary_key() const { return id; }
  };
  typedef eosio::multi_index<N(globals), global> globals_table;


   //@abi table enterprises
   struct enterprise
   {
      account_name owner;
      std::string name;
      std::string url;
      uint16_t location = 0;
      eosio::asset coupon_name;
      eosio::asset deposit = eosio::asset(0, BEAN_SYMBOL);
      eosio::asset total_stake = eosio::asset(0, BEAN_SYMBOL);
      eosio::asset total_unpaid = eosio::asset(0, BEAN_SYMBOL);
      eosio::time_point_sec last_claim_time;
      bool is_approve = false;

      uint64_t primary_key() const { return owner; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE(enterprise, (owner)(name)(url)(location)(coupon_name)(deposit)(total_stake)(total_unpaid)(last_claim_time)(is_approve))
   };
   typedef eosio::multi_index<N(enterprises), enterprise> enterprises_table;

   //@abi table stakerinfos
   struct staker_info
   {
      int64_t id;
      account_name staker;
      account_name enterprise;
      eosio::asset stake_num = eosio::asset(0, BEAN_SYMBOL);
      eosio::asset reward_etp = eosio::asset(0, BEAN_SYMBOL);
      eosio::asset coupon = eosio::asset(0, NOTHING_SYMBOL);
      eosio::time_point_sec start_at;
      eosio::time_point_sec end_at;
      uint8_t status = STAKING;

      uint64_t primary_key() const { return id; }
      uint64_t by_staker() const { return staker; }
      uint64_t by_enterprise() const { return enterprise; }

      EOSLIB_SERIALIZE(staker_info, (id)(staker)(enterprise)(stake_num)(reward_etp)(coupon)(start_at)(end_at)(status))
   };

   typedef eosio::multi_index<N(stakerinfos), staker_info,
   eosio::indexed_by<N(by_staker), eosio::const_mem_fun<staker_info, uint64_t, &staker_info::by_staker>>,
   eosio::indexed_by<N(by_enterprise), eosio::const_mem_fun<staker_info, uint64_t, &staker_info::by_enterprise>>> staker_infos_table;

   //@abi table etpoffers
   struct etp_offer
   {
      account_name owner;
      bool is_active = false;
      std::string offer_head;
      std::string offer_details;
      eosio::asset min_stake = eosio::asset(0, BEAN_SYMBOL);
      eosio::asset max_stake = eosio::asset(0, BEAN_SYMBOL);
      uint64_t duration_sec = 0;
      eosio::asset coupon_quantity;

      uint64_t primary_key() const { return owner; }

      EOSLIB_SERIALIZE(etp_offer, (owner)(is_active)(offer_head)(offer_details)(min_stake)(max_stake)(duration_sec)(coupon_quantity))
   };
   typedef eosio::multi_index<N(etpoffers), etp_offer> etp_offers_table;

   enterprises_table _enterprises;
   etp_offers_table _etp_offer;
   staker_infos_table _staker_infos;
   globals_table _globals;
};
