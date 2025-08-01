local _default_val={
    battle_reward={
        reward_type=1,
        min_level=1,
        max_level=34,
        winner_exp=12373,
        winner_coin=25000,
        loser_exp=6186,
        loser_coin=12500
    },
    participation_reward={
        exp=0,
        coin=10000
    },
    random_entrance={
        x=280,
        y=152
    }
}
local _itemlist={
    i_24557_1_0={
        item_id=24557,
        num=1,
        is_bind=0
    },
    i_24518_1_0={
        item_id=24518,
        num=1,
        is_bind=0
    },
    i_24518_2_0={
        item_id=24518,
        num=2,
        is_bind=0
    },
    i_24514_2_0={
        item_id=24514,
        num=2,
        is_bind=0
    },
    i_24514_3_0={
        item_id=24514,
        num=3,
        is_bind=0
    },
    i_24515_3_0={
        item_id=24515,
        num=3,
        is_bind=0
    },
    i_24515_5_0={
        item_id=24515,
        num=5,
        is_bind=0
    },
    i_24516_3_0={
        item_id=24516,
        num=3,
        is_bind=0
    },
    i_24516_4_0={
        item_id=24516,
        num=4,
        is_bind=0
    },
    i_24516_5_0={
        item_id=24516,
        num=5,
        is_bind=0
    },
    i_25751_1_0={
        item_id=25751,
        num=1,
        is_bind=0
    },
    i_25542_1_0={
        item_id=25542,
        num=1,
        is_bind=0
    },
    i_25752_1_0={
        item_id=25752,
        num=1,
        is_bind=0
    },
    i_25753_1_0={
        item_id=25753,
        num=1,
        is_bind=0
    },
    i_25754_1_0={
        item_id=25754,
        num=1,
        is_bind=0
    },
    i_25755_1_0={
        item_id=25755,
        num=1,
        is_bind=0
    },
    i_25756_1_0={
        item_id=25756,
        num=1,
        is_bind=0
    },
    i_25757_1_0={
        item_id=25757,
        num=1,
        is_bind=0
    },
    i_25758_1_0={
        item_id=25758,
        num=1,
        is_bind=0
    }
}
Config.chief_election_auto={
    waiting_room={
        {
            seq=0,
            scene_id=194
        },
        {
            seq=1,
            scene_id=188
        },
        {
            seq=2,
            scene_id=189
        },
        {
            seq=3,
            scene_id=190
        }
    },
    battle_reward={
        {
            seq=1,
            winner_coin=10000,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=2,
            min_level=35,
            max_level=39,
            winner_exp=14107,
            winner_coin=10000,
            loser_exp=7053,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=3,
            min_level=40,
            max_level=44,
            winner_exp=15842,
            winner_coin=10000,
            loser_exp=7921,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=4,
            min_level=45,
            max_level=49,
            winner_exp=17576,
            winner_coin=10000,
            loser_exp=8788,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=5,
            min_level=50,
            max_level=54,
            winner_exp=19311,
            winner_coin=15000,
            loser_exp=9655,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=6,
            min_level=55,
            max_level=59,
            winner_exp=21046,
            winner_coin=15000,
            loser_exp=10523,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=7,
            min_level=60,
            max_level=64,
            winner_exp=22780,
            winner_coin=15000,
            loser_exp=11390,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=8,
            min_level=65,
            max_level=69,
            winner_exp=24515,
            winner_coin=15000,
            loser_exp=12257,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=9,
            min_level=70,
            max_level=74,
            winner_exp=26250,
            winner_coin=20000,
            loser_exp=13125,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=10,
            min_level=75,
            max_level=79,
            winner_exp=27984,
            winner_coin=20000,
            loser_exp=13992,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=11,
            min_level=80,
            max_level=84,
            winner_exp=29719,
            winner_coin=20000,
            loser_exp=14859,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=12,
            min_level=85,
            max_level=89,
            winner_exp=31454,
            winner_coin=20000,
            loser_exp=15727,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=13,
            min_level=90,
            max_level=94,
            winner_exp=33188,
            loser_exp=16594,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=14,
            min_level=95,
            max_level=99,
            winner_exp=34923,
            loser_exp=17461,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=15,
            min_level=100,
            max_level=104,
            winner_exp=36658,
            loser_exp=18329,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=16,
            min_level=105,
            max_level=109,
            winner_exp=38392,
            loser_exp=19196,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=17,
            min_level=110,
            max_level=114,
            winner_exp=40127,
            loser_exp=20063,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=18,
            min_level=115,
            max_level=119,
            winner_exp=41861,
            loser_exp=20930,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=19,
            min_level=120,
            max_level=120,
            winner_exp=43596,
            loser_exp=21798,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=20,
            reward_type=2,
            winner_coin=10000,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=21,
            reward_type=2,
            min_level=35,
            max_level=39,
            winner_exp=14107,
            winner_coin=10000,
            loser_exp=7053,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=22,
            reward_type=2,
            min_level=40,
            max_level=44,
            winner_exp=15842,
            winner_coin=10000,
            loser_exp=7921,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=23,
            reward_type=2,
            min_level=45,
            max_level=49,
            winner_exp=17576,
            winner_coin=10000,
            loser_exp=8788,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=24,
            reward_type=2,
            min_level=50,
            max_level=54,
            winner_exp=19311,
            winner_coin=15000,
            loser_exp=9655,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=25,
            reward_type=2,
            min_level=55,
            max_level=59,
            winner_exp=21046,
            winner_coin=15000,
            loser_exp=10523,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=26,
            reward_type=2,
            min_level=60,
            max_level=64,
            winner_exp=22780,
            winner_coin=15000,
            loser_exp=11390,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=27,
            reward_type=2,
            min_level=65,
            max_level=69,
            winner_exp=24515,
            winner_coin=15000,
            loser_exp=12257,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=28,
            reward_type=2,
            min_level=70,
            max_level=74,
            winner_exp=26250,
            winner_coin=20000,
            loser_exp=13125,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=29,
            reward_type=2,
            min_level=75,
            max_level=79,
            winner_exp=27984,
            winner_coin=20000,
            loser_exp=13992,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=30,
            reward_type=2,
            min_level=80,
            max_level=84,
            winner_exp=29719,
            winner_coin=20000,
            loser_exp=14859,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=31,
            reward_type=2,
            min_level=85,
            max_level=89,
            winner_exp=31454,
            winner_coin=20000,
            loser_exp=15727,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=32,
            reward_type=2,
            min_level=90,
            max_level=94,
            winner_exp=33188,
            loser_exp=16594,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=33,
            reward_type=2,
            min_level=95,
            max_level=99,
            winner_exp=34923,
            loser_exp=17461,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=34,
            reward_type=2,
            min_level=100,
            max_level=104,
            winner_exp=36658,
            loser_exp=18329,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=35,
            reward_type=2,
            min_level=105,
            max_level=109,
            winner_exp=38392,
            loser_exp=19196,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=36,
            reward_type=2,
            min_level=110,
            max_level=114,
            winner_exp=40127,
            loser_exp=20063,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=37,
            reward_type=2,
            min_level=115,
            max_level=119,
            winner_exp=41861,
            loser_exp=20930,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=38,
            reward_type=2,
            min_level=120,
            max_level=120,
            winner_exp=43596,
            loser_exp=21798,
            loser_reward={
                [0]=_itemlist.i_24557_1_0
            }
        },
        {
            seq=39,
            reward_type=3,
            winner_coin=10000,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=40,
            reward_type=3,
            min_level=35,
            max_level=39,
            winner_exp=14107,
            winner_coin=10000,
            loser_exp=7053,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=41,
            reward_type=3,
            min_level=40,
            max_level=44,
            winner_exp=15842,
            winner_coin=10000,
            loser_exp=7921,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=42,
            reward_type=3,
            min_level=45,
            max_level=49,
            winner_exp=17576,
            winner_coin=10000,
            loser_exp=8788,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=43,
            reward_type=3,
            min_level=50,
            max_level=54,
            winner_exp=19311,
            winner_coin=15000,
            loser_exp=9655,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=44,
            reward_type=3,
            min_level=55,
            max_level=59,
            winner_exp=21046,
            winner_coin=15000,
            loser_exp=10523,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=45,
            reward_type=3,
            min_level=60,
            max_level=64,
            winner_exp=22780,
            winner_coin=15000,
            loser_exp=11390,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=46,
            reward_type=3,
            min_level=65,
            max_level=69,
            winner_exp=24515,
            winner_coin=15000,
            loser_exp=12257,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=47,
            reward_type=3,
            min_level=70,
            max_level=74,
            winner_exp=26250,
            winner_coin=20000,
            loser_exp=13125,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=48,
            reward_type=3,
            min_level=75,
            max_level=79,
            winner_exp=27984,
            winner_coin=20000,
            loser_exp=13992,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=49,
            reward_type=3,
            min_level=80,
            max_level=84,
            winner_exp=29719,
            winner_coin=20000,
            loser_exp=14859,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=50,
            reward_type=3,
            min_level=85,
            max_level=89,
            winner_exp=31454,
            winner_coin=20000,
            loser_exp=15727,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=51,
            reward_type=3,
            min_level=90,
            max_level=94,
            winner_exp=33188,
            loser_exp=16594,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=52,
            reward_type=3,
            min_level=95,
            max_level=99,
            winner_exp=34923,
            loser_exp=17461,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=53,
            reward_type=3,
            min_level=100,
            max_level=104,
            winner_exp=36658,
            loser_exp=18329,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=54,
            reward_type=3,
            min_level=105,
            max_level=109,
            winner_exp=38392,
            loser_exp=19196,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=55,
            reward_type=3,
            min_level=110,
            max_level=114,
            winner_exp=40127,
            loser_exp=20063,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=56,
            reward_type=3,
            min_level=115,
            max_level=119,
            winner_exp=41861,
            loser_exp=20930,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=57,
            reward_type=3,
            min_level=120,
            max_level=120,
            winner_exp=43596,
            loser_exp=21798,
            loser_reward={
                [0]=_itemlist.i_24518_1_0
            }
        },
        {
            seq=58,
            reward_type=4,
            winner_coin=10000,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=59,
            reward_type=4,
            min_level=35,
            max_level=39,
            winner_exp=14107,
            winner_coin=10000,
            loser_exp=7053,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=60,
            reward_type=4,
            min_level=40,
            max_level=44,
            winner_exp=15842,
            winner_coin=10000,
            loser_exp=7921,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=61,
            reward_type=4,
            min_level=45,
            max_level=49,
            winner_exp=17576,
            winner_coin=10000,
            loser_exp=8788,
            loser_coin=5000,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=62,
            reward_type=4,
            min_level=50,
            max_level=54,
            winner_exp=19311,
            winner_coin=15000,
            loser_exp=9655,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=63,
            reward_type=4,
            min_level=55,
            max_level=59,
            winner_exp=21046,
            winner_coin=15000,
            loser_exp=10523,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=64,
            reward_type=4,
            min_level=60,
            max_level=64,
            winner_exp=22780,
            winner_coin=15000,
            loser_exp=11390,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=65,
            reward_type=4,
            min_level=65,
            max_level=69,
            winner_exp=24515,
            winner_coin=15000,
            loser_exp=12257,
            loser_coin=7500,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=66,
            reward_type=4,
            min_level=70,
            max_level=74,
            winner_exp=26250,
            winner_coin=20000,
            loser_exp=13125,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=67,
            reward_type=4,
            min_level=75,
            max_level=79,
            winner_exp=27984,
            winner_coin=20000,
            loser_exp=13992,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=68,
            reward_type=4,
            min_level=80,
            max_level=84,
            winner_exp=29719,
            winner_coin=20000,
            loser_exp=14859,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=69,
            reward_type=4,
            min_level=85,
            max_level=89,
            winner_exp=31454,
            winner_coin=20000,
            loser_exp=15727,
            loser_coin=10000,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=70,
            reward_type=4,
            min_level=90,
            max_level=94,
            winner_exp=33188,
            loser_exp=16594,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=71,
            reward_type=4,
            min_level=95,
            max_level=99,
            winner_exp=34923,
            loser_exp=17461,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=72,
            reward_type=4,
            min_level=100,
            max_level=104,
            winner_exp=36658,
            loser_exp=18329,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=73,
            reward_type=4,
            min_level=105,
            max_level=109,
            winner_exp=38392,
            loser_exp=19196,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=74,
            reward_type=4,
            min_level=110,
            max_level=114,
            winner_exp=40127,
            loser_exp=20063,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=75,
            reward_type=4,
            min_level=115,
            max_level=119,
            winner_exp=41861,
            loser_exp=20930,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        },
        {
            seq=76,
            reward_type=4,
            min_level=120,
            max_level=120,
            winner_exp=43596,
            loser_exp=21798,
            loser_reward={
                [0]=_itemlist.i_24518_2_0
            }
        }
    },
    participation_reward={
        {
            level_min=1,
            level_max=29,
            join_reward={
                [0]=_itemlist.i_24514_2_0
            }
        },
        {
            level_min=30,
            level_max=34,
            join_reward={
                [0]=_itemlist.i_24514_2_0
            }
        },
        {
            level_min=35,
            level_max=39,
            join_reward={
                [0]=_itemlist.i_24514_2_0
            }
        },
        {
            level_min=40,
            level_max=44,
            coin=20000,
            join_reward={
                [0]=_itemlist.i_24514_3_0
            }
        },
        {
            level_min=45,
            level_max=49,
            coin=20000,
            join_reward={
                [0]=_itemlist.i_24514_3_0
            }
        },
        {
            level_min=50,
            level_max=54,
            coin=30000,
            join_reward={
                [0]=_itemlist.i_24515_3_0
            }
        },
        {
            level_min=55,
            level_max=59,
            coin=30000,
            join_reward={
                [0]=_itemlist.i_24515_3_0
            }
        },
        {
            level_min=60,
            level_max=64,
            coin=40000,
            join_reward={
                [0]=_itemlist.i_24515_5_0
            }
        },
        {
            level_min=65,
            level_max=69,
            coin=40000,
            join_reward={
                [0]=_itemlist.i_24515_5_0
            }
        },
        {
            level_min=70,
            level_max=74,
            coin=50000,
            join_reward={
                [0]=_itemlist.i_24516_3_0
            }
        },
        {
            level_min=75,
            level_max=79,
            coin=50000,
            join_reward={
                [0]=_itemlist.i_24516_3_0
            }
        },
        {
            level_min=80,
            level_max=84,
            coin=60000,
            join_reward={
                [0]=_itemlist.i_24516_3_0
            }
        },
        {
            level_min=85,
            level_max=89,
            coin=60000,
            join_reward={
                [0]=_itemlist.i_24516_3_0
            }
        },
        {
            level_min=90,
            level_max=94,
            coin=70000,
            join_reward={
                [0]=_itemlist.i_24516_4_0
            }
        },
        {
            level_min=95,
            level_max=99,
            coin=70000,
            join_reward={
                [0]=_itemlist.i_24516_4_0
            }
        },
        {
            level_min=100,
            level_max=104,
            coin=80000,
            join_reward={
                [0]=_itemlist.i_24516_4_0
            }
        },
        {
            level_min=105,
            level_max=109,
            coin=80000,
            join_reward={
                [0]=_itemlist.i_24516_4_0
            }
        },
        {
            level_min=110,
            level_max=114,
            coin=90000,
            join_reward={
                [0]=_itemlist.i_24516_5_0
            }
        },
        {
            level_min=115,
            level_max=119,
            coin=90000,
            join_reward={
                [0]=_itemlist.i_24516_5_0
            }
        },
        {
            level_min=120,
            level_max=120,
            coin=100000,
            join_reward={
                [0]=_itemlist.i_24516_5_0
            }
        }
    },
    title={
        {
            seq=0,
            type=100,
            reward_show={
                [0]=_itemlist.i_25751_1_0,
                [1]=_itemlist.i_25542_1_0
            },
            title_id=22,
            npc_id=40520,
            reward={
                [0]=_itemlist.i_25542_1_0
            }
        },
        {
            seq=1,
            type=200,
            reward_show={
                [0]=_itemlist.i_25752_1_0,
                [1]=_itemlist.i_25542_1_0
            },
            title_id=23,
            npc_id=40528,
            reward={
                [0]=_itemlist.i_25542_1_0
            }
        },
        {
            seq=2,
            type=300,
            reward_show={
                [0]=_itemlist.i_25753_1_0,
                [1]=_itemlist.i_25542_1_0
            },
            title_id=24,
            npc_id=40534,
            reward={
                [0]=_itemlist.i_25542_1_0
            }
        },
        {
            seq=3,
            type=400,
            reward_show={
                [0]=_itemlist.i_25754_1_0,
                [1]=_itemlist.i_25542_1_0
            },
            title_id=25,
            npc_id=40526,
            reward={
                [0]=_itemlist.i_25542_1_0
            }
        },
        {
            seq=4,
            type=500,
            reward_show={
                [0]=_itemlist.i_25755_1_0,
                [1]=_itemlist.i_25542_1_0
            },
            title_id=26,
            npc_id=40532,
            reward={
                [0]=_itemlist.i_25542_1_0
            }
        },
        {
            seq=5,
            type=600,
            reward_show={
                [0]=_itemlist.i_25756_1_0,
                [1]=_itemlist.i_25542_1_0
            },
            title_id=27,
            npc_id=40522,
            reward={
                [0]=_itemlist.i_25542_1_0
            }
        },
        {
            seq=6,
            type=700,
            reward_show={
                [0]=_itemlist.i_25757_1_0,
                [1]=_itemlist.i_25542_1_0
            },
            title_id=28,
            npc_id=40524,
            reward={
                [0]=_itemlist.i_25542_1_0
            }
        },
        {
            seq=7,
            type=800,
            reward_show={
                [0]=_itemlist.i_25758_1_0,
                [1]=_itemlist.i_25542_1_0
            },
            title_id=29,
            npc_id=40530,
            reward={
                [0]=_itemlist.i_25542_1_0
            }
        }
    },
    other={
        {
            max_round=20,
            countdown=60,
            competition_scene_1=1601,
            competition_scene_2=1602,
            competition_scene_3=1603,
            competition_scene_4=1604,
            competition_scene_5=1605,
            competition_scene_6=1606,
            competition_scene_7=1607,
            competition_scene_8=1608,
            ready_time=300,
            win_score=3,
            lose_score=0,
            draw_score=1,
            max_fail_num=3,
            competition_time=1200,
            field_buff_id=5
        }
    },
    random_entrance={
        {
            seq=0
        },
        {
            seq=1,
            x=414
        },
        {
            seq=2,
            x=174
        },
        {
            seq=3,
            x=64,
            y=154
        },
        {
            seq=4,
            y=60
        }
    }
}
SetConfigDefaultVal(Config.chief_election_auto,_default_val,nil,nil,nil)