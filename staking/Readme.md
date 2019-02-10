STAKING CONTRACT
-----------------


#### Prerequisites


* Please use following accounts to test

etpcontract1 -  EOS5ZwsT4k5szwhgbxyoWa5D2tHYAPxEwQKrHxWtU3xnPeJkudLfF
thebeantoken - EOS5ZwsT4k5szwhgbxyoWa5D2tHYAPxEwQKrHxWtU3xnPeJkudLfF

* Endpoint network:

http://jungle2.cryptolions.io:80

* Contract name: etpcontract1


##### Set contract to etpcontract1 account
````bash
$ cleos -u http://jungle2.cryptolions.io:80 set contract etpcontract1 ./staking/ -p etpcontract1
````

##### Check contract ram
````bash
cleos -u http://jungle2.cryptolions.io:80 get account etpcontract1
...
memory: 
 quota:      1021 KiB    used:     347.3 KiB
````

##### eosio.code permission, remember change publickey


````bash

$ cleos -u http://jungle2.cryptolions.io:80 set account permission etpcontract1 active '{"threshold": 1,"keys": [{"key": "EOS5ZwsT4k5szwhgbxyoWa5D2tHYAPxEwQKrHxWtU3xnPeJkudLfF","weight": 1}],"accounts": [{"permission":{"actor":"etpcontract1","permission":"eosio.code"},"weight":1}]}' owner -p etpcontract1

##### Register Enterprice

````bash
$ cleos -u http://jungle2.cryptolions.io:80 push action etpcontract1 regetp '["enterprice12", "Cafe In The Moon", "moon.cafe", 123, "0.0000 MOON"]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 push action etpcontract1 regetp '["notokenfree1", "Get discount from abcdef", "abcdef.cafe", 213, "0.0000 NOT"]' -p notokenfree1

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract1 etpcontract1 enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": 1313820493,
      "total_stake": "0.0000 EOS",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-10T15:55:35",
      "is_approve": 0
    },{
      "owner": "notokenfree1",
      "name": "Get discount from abcdef",
      "url": "abcdef.cafe",
      "location": 213,
      "coupon_name": 5525326,
      "total_stake": "0.0000 EOS",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-10T15:59:21",
      "is_approve": 0
    }
  ],
  "more": false
}


````


##### Enterprice start staking, at least 10.000.000 BEAN to becomean enterprice
````bash


$ cleos -u http://jungle2.cryptolions.io:80 push action thebeantoken transfer '["notokenfree1", "etpcontract1", "10000001.0000 BEAN", "notokenfree1"]' -p notokenfree1

$ cleos -u http://jungle2.cryptolions.io:80push action thebeantoken transfer '["enterprice12", "etpcontract1", "3000000.0000 BEAN", "enterprice12"]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract1 etpcontract1 enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": 1313820493,
      "total_stake": "8000000.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-10T16:20:18",
      "is_approve": 0
    },{
      "owner": "notokenfree1",
      "name": "Get discount from abcdef",
      "url": "abcdef.cafe",
      "location": 213,
      "coupon_name": 5525326,
      "total_stake": "10000001.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-10T16:14:35",
      "is_approve": 1
    }
  ],
  "more": false
}

$ cleos -u http://jungle2.cryptolions.io:80 push action thebeantoken transfer '["enterprice12", "etpcontract1", "2000000.0000 BEAN", "enterprice12"]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract1 etpcontract1 enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": 1313820493,
      "total_stake": "10000000.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-10T16:21:34",
      "is_approve": 1
    },{
      "owner": "notokenfree1",
      "name": "Get discount from abcdef",
      "url": "abcdef.cafe",
      "location": 213,
      "coupon_name": 5525326,
      "total_stake": "10000001.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-10T16:14:35",
      "is_approve": 1
    }
  ],
  "more": false
}

````

##### enterprice set offer
````bash
$ cleos -u http://jungle2.cryptolions.io:80 push action etpcontract1 setoffer '["enterprice12", "Get 3 Free Cafe in Moon", "All detial offer here", 123450000, 123450000, 7776000, "3.0000 MOON", true]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 push action etpcontract1 setoffer '["notokenfree1", "Create Video to get Discount", "All detail offer in here", 10000, 100000, 7776000, "0.0000 NOT", true]' -p notokenfree1

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract1 etpcontract1 etpoffers
{
  "rows": [{
      "owner": "enterprice12",
      "is_active": 1,
      "offer_head": "Get 3 Free Cafe in Moon",
      "offer_details": "All detial offer here",
      "min_stake": 123450000,
      "max_stake": 123450000,
      "duration_sec": 7776000,
      "coupon_quantity": "3.0000 MOON"
    },{
      "owner": "notokenfree1",
      "is_active": 1,
      "offer_head": "Create Video to get Discount",
      "offer_details": "All detail offer in here",
      "min_stake": 10000,
      "max_stake": 100000,
      "duration_sec": 7776000,
      "coupon_quantity": "0.0000 NOT"
    }
  ],
  "more": false
}


````

##### User stake to get free Cafe or get deal from ETP

````bash

$ cleos -u http://jungle2.cryptolions.io:80  push action thebeantoken transfer '["wantfreecafe", "etpcontract1", "12345.0000 BEAN", "enterprice12"]' -p wantfreecafe
$ cleos -u http://jungle2.cryptolions.io:80  push action thebeantoken transfer '["getdiscount1", "etpcontract1", "5.0000 BEAN", "notokenfree1"]' -p getdiscount1

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract1 etpcontract1 stakerinfos
{
  "rows": [{
      "id": 0,
      "staker": "wantfreecafe",
      "enterprise": "enterprice12",
      "stake_num": "1.2345 BEAN",
      "coupon": "3.0000 MOON",
      "start_at": "2019-02-10T16:41:52",
      "end_at": "2019-05-11T16:41:52",
      "updated_at": "2019-02-10T16:41:52",
      "is_done": 0
    },{
      "id": 1,
      "staker": "wantfreecafe",
      "enterprise": "enterprice12",
      "stake_num": "12345.0000 BEAN",
      "coupon": "3.0000 MOON",
      "start_at": "2019-02-10T16:42:51",
      "end_at": "2019-05-11T16:42:51",
      "updated_at": "2019-02-10T16:42:51",
      "is_done": 0
    },{
      "id": 2,
      "staker": "getdiscount1",
      "enterprise": "notokenfree1",
      "stake_num": "5.0000 BEAN",
      "coupon": "0.0000 NOT",
      "start_at": "2019-02-10T16:46:03",
      "end_at": "2019-05-11T16:46:03",
      "updated_at": "2019-02-10T16:46:03",
      "is_done": 0
    }
  ],
  "more": false
}


````
##### Get refund:

````bash
$ cleos -u http://jungle2.cryptolions.io:80 get table thebeantoken wantfreecafe accounts
{
  "rows": [{
      "balance": "975208.5310 BEAN"
    }
  ],
  "more": false
}

$ cleos -u http://jungle2.cryptolions.io:80  push action etpcontract1  refund '["wantfreecafe", 3]' -p wantfreecafe

$ cleos -u http://jungle2.cryptolions.io:80 get table thebeantoken wantfreecafe accounts
{
  "rows": [{
      "balance": "987553.5310 BEAN"
    }
  ],
  "more": false
}

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract1 etpcontract1 stakerinfos
{
  "rows": [{{
      "id": 3,
      "staker": "wantfreecafe",
      "enterprise": "enterprice12",
      "stake_num": "12345.0000 BEAN",
      "coupon": "3.0000 MOON",
      "start_at": "2019-02-10T16:56:42",
      "end_at": "2019-02-10T16:56:47",
      "updated_at": "2019-02-10T16:56:42",
      "is_done": 1
    }
  ],
  "more": false
}

````

##### Claim reward:

````bash
$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract1 etpcontract1 enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": 1313820493,
      "total_stake": "10012346.2345 BEAN",
      "total_unpaid": "41.3583 BEAN",
      "last_claim_time": "2019-02-10T17:04:49",
      "is_approve": 1
    }
  ],
  "more": false
}

$ cleos -u http://jungle2.cryptolions.io:80  push action etpcontract1  claimrewards '["enterprice12"]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract1 etpcontract1 enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": 1313820493,
      "total_stake": "10012346.2345 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-10T17:04:49",
      "is_approve": 1
    }
  ],
  "more": false
}

````
