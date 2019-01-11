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

class staking : public eosio::contract
{
 public:
   staking(account_name self);

   void transfer(uint64_t sender, uint64_t receiver);

   void regetp(account_name enterprise, std::string name, std::string &url, uint16_t location);

   void claimrewards(account_name enterprise);

   void cupreceived(account_name account, account_name enterprise);

   void refund( const account_name owner, account_name enterprise);
   struct transfer_args
   {
      account_name from;
      account_name to;
      eosio::asset quantity;
      std::string memo;
   };

 private:
   struct enterprise
   {
      account_name owner;
      uint64_t total_stake = 0;
      std::string name;
      bool is_approve = false;
      std::string url;
      uint64_t total_unpaid = 0;
      eosio::time_point_sec last_claim_time;
      uint16_t location = 0;

      uint64_t primary_key() const { return owner; }
      uint64_t by_stakes() const { return total_stake; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE(enterprise, (owner)(name)(total_stake)(is_approve)(url)(last_claim_time)(location))
   };
   typedef eosio::multi_index<N(enterprises), enterprise,
                              eosio::indexed_by<N(by_stakes), eosio::const_mem_fun<enterprise, uint64_t, &enterprise::by_stakes>>>
       enterprises_table;

   // was stored by enterprise

   struct staker_info
   {
      account_name staker;
      uint64_t stake_num = 0;
      eosio::time_point_sec start_at;
      eosio::time_point_sec end_at;
      eosio::time_point_sec updated_at;
      uint64_t free_cup;

      uint64_t primary_key() const { return staker; }

      EOSLIB_SERIALIZE(staker_info, (staker)(stake_num)(start_at)(end_at)(updated_at)(free_cup))
   };
   typedef eosio::multi_index<N(staker_infos), staker_info> staker_infos;

   struct etp_offer
   {
      account_name enterprise;
      bool is_active = false;
      uint64_t stake_num = 0;
      uint64_t duration_sec = 0;
      uint64_t free_cup = 0;

      uint64_t primary_key() const { return enterprise; }

      EOSLIB_SERIALIZE(etp_offer, (enterprise)(is_active)(stake_num)(duration_sec)(free_cup))
   };
   typedef eosio::multi_index<N(etp_offers), etp_offer> etp_offers_table;

   enterprises_table _enterprises;
   etp_offers_table _etp_offer;
};
