local _default_val={
    team_name={
        team_name="天魁"
    }
}
local _itemlist={
    i_27974_1_1={
        item_id=27974,
        num=1,
        is_bind=1
    },
    i_25542_1_1={
        item_id=25542,
        num=1,
        is_bind=1
    },
    i_24518_1_1={
        item_id=24518,
        num=1,
        is_bind=1
    },
    i_28828_1_1={
        item_id=28828,
        num=1,
        is_bind=1
    },
    i_25855_1_1={
        item_id=25855,
        num=1,
        is_bind=1
    },
    i_24326_3_1={
        item_id=24326,
        num=3,
        is_bind=1
    },
    i_24326_4_1={
        item_id=24326,
        num=4,
        is_bind=1
    },
    i_24326_5_1={
        item_id=24326,
        num=5,
        is_bind=1
    },
    i_24518_2_1={
        item_id=24518,
        num=2,
        is_bind=1
    },
    i_24326_6_1={
        item_id=24326,
        num=6,
        is_bind=1
    },
    i_24326_8_1={
        item_id=24326,
        num=8,
        is_bind=1
    },
    i_24326_10_1={
        item_id=24326,
        num=10,
        is_bind=1
    },
    i_24326_12_1={
        item_id=24326,
        num=12,
        is_bind=1
    },
    i_25542_2_1={
        item_id=25542,
        num=2,
        is_bind=1
    },
    i_27974_150_1={
        item_id=27974,
        num=150,
        is_bind=1
    },
    i_24326_50_1={
        item_id=24326,
        num=50,
        is_bind=1
    },
    i_27974_130_1={
        item_id=27974,
        num=130,
        is_bind=1
    },
    i_24326_30_1={
        item_id=24326,
        num=30,
        is_bind=1
    },
    i_24518_6_1={
        item_id=24518,
        num=6,
        is_bind=1
    },
    i_27974_120_1={
        item_id=27974,
        num=120,
        is_bind=1
    },
    i_24326_20_1={
        item_id=24326,
        num=20,
        is_bind=1
    },
    i_24518_5_1={
        item_id=24518,
        num=5,
        is_bind=1
    },
    i_27974_110_1={
        item_id=27974,
        num=110,
        is_bind=1
    },
    i_27974_100_1={
        item_id=27974,
        num=100,
        is_bind=1
    },
    i_24518_4_1={
        item_id=24518,
        num=4,
        is_bind=1
    },
    i_27974_90_1={
        item_id=27974,
        num=90,
        is_bind=1
    },
    i_27974_80_1={
        item_id=27974,
        num=80,
        is_bind=1
    },
    i_24518_3_1={
        item_id=24518,
        num=3,
        is_bind=1
    },
    i_27974_70_1={
        item_id=27974,
        num=70,
        is_bind=1
    },
    i_27974_60_1={
        item_id=27974,
        num=60,
        is_bind=1
    },
    i_27974_50_1={
        item_id=27974,
        num=50,
        is_bind=1
    },
    i_27303_50_1={
        item_id=27303,
        num=50,
        is_bind=1
    },
    i_27303_45_1={
        item_id=27303,
        num=45,
        is_bind=1
    },
    i_27303_40_1={
        item_id=27303,
        num=40,
        is_bind=1
    },
    i_27303_35_1={
        item_id=27303,
        num=35,
        is_bind=1
    },
    i_27303_30_1={
        item_id=27303,
        num=30,
        is_bind=1
    },
    i_27303_28_1={
        item_id=27303,
        num=28,
        is_bind=1
    },
    i_27303_25_1={
        item_id=27303,
        num=25,
        is_bind=1
    },
    i_27303_23_1={
        item_id=27303,
        num=23,
        is_bind=1
    },
    i_27303_22_1={
        item_id=27303,
        num=22,
        is_bind=1
    },
    i_27303_20_1={
        item_id=27303,
        num=20,
        is_bind=1
    }
}
Config.worldarena_auto={
    other={
        {
            duration=10,
            winner_score=4,
            tie_score=2,
            loser_score=1,
            give_up_score=0,
            buff_appear_numbers=10,
            buff_id=519,
            max_numbers=20,
            item_list={
                [0]=_itemlist.i_27974_1_1,
                [1]=_itemlist.i_25542_1_1,
                [2]=_itemlist.i_24518_1_1
            },
            report_reward={
                [0]=_itemlist.i_28828_1_1
            },
            prepare_scene=316,
            day_reward={
                [0]=_itemlist.i_25855_1_1
            },
            attack_x=140,
            attack_y=68,
            def_x=41,
            def_y=70,
            dec_id=269
        }
    },
    victory_times_reward={
        {
            sn=1,
            box_names="首胜宝箱",
            victory_times=1,
            win_times={
                [0]=_itemlist.i_24518_1_1,
                [1]=_itemlist.i_24326_3_1
            }
        },
        {
            sn=2,
            box_names="3胜宝箱",
            victory_times=3,
            win_times={
                [0]=_itemlist.i_24518_1_1,
                [1]=_itemlist.i_24326_4_1
            }
        },
        {
            sn=3,
            box_names="5胜宝箱",
            victory_times=5,
            win_times={
                [0]=_itemlist.i_24518_1_1,
                [1]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=4,
            box_names="7胜宝箱",
            victory_times=7,
            win_times={
                [0]=_itemlist.i_24518_2_1,
                [1]=_itemlist.i_24326_6_1
            }
        },
        {
            sn=5,
            box_names="10胜宝箱",
            victory_times=10,
            win_times={
                [0]=_itemlist.i_24518_2_1,
                [1]=_itemlist.i_24326_8_1
            }
        },
        {
            sn=6,
            box_names="12胜宝箱",
            victory_times=12,
            win_times={
                [0]=_itemlist.i_24518_2_1,
                [1]=_itemlist.i_24326_10_1
            }
        },
        {
            sn=7,
            box_names="14胜宝箱",
            victory_times=14,
            win_times={
                [0]=_itemlist.i_25542_1_1,
                [1]=_itemlist.i_24326_12_1
            }
        }
    },
    season_rank_reward={
        {
            sn=1,
            score_rank=1,
            rank_reward={
                [0]=_itemlist.i_25542_2_1,
                [1]=_itemlist.i_27974_150_1,
                [2]=_itemlist.i_24326_50_1
            }
        },
        {
            sn=2,
            score_rank=2,
            rank_reward={
                [0]=_itemlist.i_25542_1_1,
                [1]=_itemlist.i_27974_130_1,
                [2]=_itemlist.i_24326_30_1
            }
        },
        {
            sn=3,
            score_rank=3,
            rank_reward={
                [0]=_itemlist.i_24518_6_1,
                [1]=_itemlist.i_27974_120_1,
                [2]=_itemlist.i_24326_20_1
            }
        },
        {
            sn=4,
            score_rank=4,
            rank_reward={
                [0]=_itemlist.i_24518_5_1,
                [1]=_itemlist.i_27974_110_1,
                [2]=_itemlist.i_24326_10_1
            }
        },
        {
            sn=5,
            score_rank=5,
            rank_reward={
                [0]=_itemlist.i_24518_5_1,
                [1]=_itemlist.i_27974_100_1,
                [2]=_itemlist.i_24326_10_1
            }
        },
        {
            sn=6,
            score_rank=6,
            rank_reward={
                [0]=_itemlist.i_24518_4_1,
                [1]=_itemlist.i_27974_90_1,
                [2]=_itemlist.i_24326_10_1
            }
        },
        {
            sn=7,
            score_rank=7,
            rank_reward={
                [0]=_itemlist.i_24518_4_1,
                [1]=_itemlist.i_27974_80_1,
                [2]=_itemlist.i_24326_10_1
            }
        },
        {
            sn=8,
            score_rank=8,
            rank_reward={
                [0]=_itemlist.i_24518_3_1,
                [1]=_itemlist.i_27974_70_1,
                [2]=_itemlist.i_24326_10_1
            }
        },
        {
            sn=9,
            score_rank=9,
            rank_reward={
                [0]=_itemlist.i_24518_3_1,
                [1]=_itemlist.i_27974_60_1,
                [2]=_itemlist.i_24326_10_1
            }
        },
        {
            sn=10,
            score_rank=10,
            rank_reward={
                [0]=_itemlist.i_24518_3_1,
                [1]=_itemlist.i_27974_50_1,
                [2]=_itemlist.i_24326_10_1
            }
        },
        {
            sn=11,
            score_rank=11,
            rank_reward={
                [0]=_itemlist.i_24518_2_1,
                [1]=_itemlist.i_27303_50_1,
                [2]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=12,
            score_rank=12,
            rank_reward={
                [0]=_itemlist.i_24518_2_1,
                [1]=_itemlist.i_27303_45_1,
                [2]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=13,
            score_rank=13,
            rank_reward={
                [0]=_itemlist.i_24518_2_1,
                [1]=_itemlist.i_27303_40_1,
                [2]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=14,
            score_rank=14,
            rank_reward={
                [0]=_itemlist.i_24518_1_1,
                [1]=_itemlist.i_27303_35_1,
                [2]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=15,
            score_rank=15,
            rank_reward={
                [0]=_itemlist.i_24518_1_1,
                [1]=_itemlist.i_27303_30_1,
                [2]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=16,
            score_rank=16,
            rank_reward={
                [0]=_itemlist.i_27303_28_1,
                [1]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=17,
            score_rank=17,
            rank_reward={
                [0]=_itemlist.i_27303_25_1,
                [1]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=18,
            score_rank=18,
            rank_reward={
                [0]=_itemlist.i_27303_23_1,
                [1]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=19,
            score_rank=19,
            rank_reward={
                [0]=_itemlist.i_27303_22_1,
                [1]=_itemlist.i_24326_5_1
            }
        },
        {
            sn=20,
            score_rank=20,
            rank_reward={
                [0]=_itemlist.i_27303_20_1,
                [1]=_itemlist.i_24326_5_1
            }
        }
    },
    team_name={
        {
            sn=1
        },
        {
            sn=2,
            team_name="天罡"
        },
        {
            sn=3,
            team_name="天机"
        },
        {
            sn=4,
            team_name="天闲"
        },
        {
            sn=5,
            team_name="天勇"
        },
        {
            sn=6,
            team_name="天雄"
        },
        {
            sn=7,
            team_name="天猛"
        },
        {
            sn=8,
            team_name="天威"
        },
        {
            sn=9,
            team_name="天英"
        },
        {
            sn=10,
            team_name="天贵"
        },
        {
            sn=11,
            team_name="天福"
        },
        {
            sn=12,
            team_name="天满"
        },
        {
            sn=13,
            team_name="天孤"
        },
        {
            sn=14,
            team_name="天伤"
        },
        {
            sn=15,
            team_name="天立"
        },
        {
            sn=16,
            team_name="天捷"
        },
        {
            sn=17,
            team_name="天暗"
        },
        {
            sn=18,
            team_name="天佑"
        },
        {
            sn=19,
            team_name="天空"
        },
        {
            sn=20,
            team_name="天速"
        },
        {
            sn=21,
            team_name="天异"
        },
        {
            sn=22,
            team_name="天杀"
        },
        {
            sn=23,
            team_name="天微"
        },
        {
            sn=24,
            team_name="天究"
        },
        {
            sn=25,
            team_name="天退"
        },
        {
            sn=26,
            team_name="天寿"
        },
        {
            sn=27,
            team_name="天剑"
        },
        {
            sn=28,
            team_name="天平"
        },
        {
            sn=29,
            team_name="天罪"
        },
        {
            sn=30,
            team_name="天损"
        },
        {
            sn=31,
            team_name="天败"
        },
        {
            sn=32,
            team_name="天牢"
        },
        {
            sn=33,
            team_name="天慧"
        },
        {
            sn=34,
            team_name="天暴"
        },
        {
            sn=35,
            team_name="天哭"
        },
        {
            sn=36,
            team_name="天巧"
        },
        {
            sn=37
        },
        {
            sn=38,
            team_name="天罡"
        },
        {
            sn=39,
            team_name="天机"
        },
        {
            sn=40,
            team_name="天闲"
        },
        {
            sn=41,
            team_name="天勇"
        },
        {
            sn=42,
            team_name="天雄"
        },
        {
            sn=43,
            team_name="天猛"
        },
        {
            sn=44,
            team_name="天威"
        },
        {
            sn=45,
            team_name="天英"
        },
        {
            sn=46,
            team_name="天贵"
        },
        {
            sn=47,
            team_name="天福"
        },
        {
            sn=48,
            team_name="天满"
        },
        {
            sn=49,
            team_name="天孤"
        },
        {
            sn=50,
            team_name="天伤"
        },
        {
            sn=51,
            team_name="天立"
        },
        {
            sn=52,
            team_name="天捷"
        },
        {
            sn=53,
            team_name="天暗"
        },
        {
            sn=54,
            team_name="天佑"
        },
        {
            sn=55,
            team_name="天空"
        },
        {
            sn=56,
            team_name="天速"
        },
        {
            sn=57,
            team_name="天异"
        },
        {
            sn=58,
            team_name="天杀"
        },
        {
            sn=59,
            team_name="天微"
        },
        {
            sn=60,
            team_name="天究"
        },
        {
            sn=61,
            team_name="天退"
        },
        {
            sn=62,
            team_name="天寿"
        },
        {
            sn=63,
            team_name="天剑"
        },
        {
            sn=64,
            team_name="天平"
        },
        {
            sn=65,
            team_name="天罪"
        },
        {
            sn=66,
            team_name="天损"
        },
        {
            sn=67,
            team_name="天败"
        },
        {
            sn=68,
            team_name="天牢"
        },
        {
            sn=69,
            team_name="天慧"
        },
        {
            sn=70,
            team_name="天暴"
        },
        {
            sn=71,
            team_name="天哭"
        },
        {
            sn=72,
            team_name="天巧"
        },
        {
            sn=73
        },
        {
            sn=74,
            team_name="天罡"
        },
        {
            sn=75,
            team_name="天机"
        },
        {
            sn=76,
            team_name="天闲"
        },
        {
            sn=77,
            team_name="天勇"
        },
        {
            sn=78,
            team_name="天雄"
        },
        {
            sn=79,
            team_name="天猛"
        },
        {
            sn=80,
            team_name="天威"
        },
        {
            sn=81,
            team_name="天英"
        },
        {
            sn=82,
            team_name="天贵"
        },
        {
            sn=83,
            team_name="天福"
        },
        {
            sn=84,
            team_name="天满"
        },
        {
            sn=85,
            team_name="天孤"
        },
        {
            sn=86,
            team_name="天伤"
        },
        {
            sn=87,
            team_name="天立"
        },
        {
            sn=88,
            team_name="天捷"
        },
        {
            sn=89,
            team_name="天暗"
        },
        {
            sn=90,
            team_name="天佑"
        },
        {
            sn=91,
            team_name="天空"
        },
        {
            sn=92,
            team_name="天速"
        },
        {
            sn=93,
            team_name="天异"
        },
        {
            sn=94,
            team_name="天杀"
        },
        {
            sn=95,
            team_name="天微"
        },
        {
            sn=96,
            team_name="天究"
        },
        {
            sn=97,
            team_name="天退"
        },
        {
            sn=98,
            team_name="天寿"
        },
        {
            sn=99,
            team_name="天剑"
        },
        {
            sn=100,
            team_name="天平"
        },
        {
            sn=101,
            team_name="天罪"
        },
        {
            sn=102,
            team_name="天损"
        },
        {
            sn=103,
            team_name="天败"
        },
        {
            sn=104,
            team_name="天牢"
        },
        {
            sn=105,
            team_name="天慧"
        },
        {
            sn=106,
            team_name="天暴"
        },
        {
            sn=107,
            team_name="天哭"
        },
        {
            sn=108,
            team_name="天巧"
        }
    },
    battle_cook={
        {
            sn=31610
        },
        {
            sn=31620
        },
        {
            sn=31630
        },
        {
            sn=31640
        },
        {
            sn=31650
        },
        {
            sn=31660
        }
    },
    robot_cfg={},
    attr_adjust_group={}
}
SetConfigDefaultVal(Config.worldarena_auto,_default_val,nil,nil,nil)