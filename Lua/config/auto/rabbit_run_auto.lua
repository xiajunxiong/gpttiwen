local _default_val={
    encourage={
        succ_rate_1=35,
        succ_rate_2=65,
        succ_rate_3=100,
        effect=0,
        carrot_speed=0,
        succ_talk=147
    },
    track={
        start_x0=130,
        start_x1=130,
        end_x0=432,
        end_x1=432,
        fireworks_x0=438,
        fireworks_x1=438
    }
}
local _itemlist={
    i_31324_2_1={
        item_id=31324,
        num=2,
        is_bind=1
    },
    i_26232_1_1={
        item_id=26232,
        num=1,
        is_bind=1
    },
    i_26232_50_1={
        item_id=26232,
        num=50,
        is_bind=1
    },
    i_31323_2_1={
        item_id=31323,
        num=2,
        is_bind=1
    }
}
Config.rabbit_run_auto={
    rabbit={
        [1]={
            rabbit=1,
            name="呆兔",
            desc="慢得可爱",
            speed=3,
            rest_space_min=8,
            rest_space_max=15,
            rest_time=20
        },
        [2]={
            rabbit=2,
            name="萌兔",
            desc="寻常速度",
            speed=4,
            rest_space_min=12,
            rest_space_max=20,
            rest_time=15
        },
        [3]={
            rabbit=3,
            name="脱兔",
            desc="迅捷如风",
            speed=6,
            rest_space_min=16,
            rest_space_max=25,
            rest_time=10
        }
    },
    encourage={
        [0]={
            encourage=0,
            name="赞扬",
            fail_talk=145
        },
        [1]={
            encourage=1,
            name="激将",
            fail_talk=146
        },
        [2]={
            encourage=2,
            name="喂胡萝卜",
            succ_rate_1=100,
            succ_rate_2=100,
            effect=1,
            carrot_speed=2,
            succ_talk=148,
            fail_talk=148
        }
    },
    track={
        [0]={
            track=0,
            start_y0=112,
            start_y1=102,
            end_y0=112,
            end_y1=102,
            fireworks_y0=286,
            fireworks_y1=284
        },
        [1]={
            track=1,
            start_y0=84,
            start_y1=74,
            end_y0=84,
            end_y1=74,
            fireworks_y0=280.8,
            fireworks_y1=278.8
        },
        [2]={
            track=2,
            start_y0=55,
            start_y1=46,
            end_y0=55,
            end_y1=46,
            fireworks_y0=275,
            fireworks_y1=273
        }
    },
    other={
        {
            carrot_num=6,
            carrot_times=5,
            carrot_speed=2,
            reward_times=2,
            succ_talk=144,
            rest_talk=142,
            rest_end_talk=143,
            reward_show={
                [0]=_itemlist.i_31324_2_1,
                [1]=_itemlist.i_26232_1_1
            },
            desc1=297,
            desc2=298,
            npc_id=73704,
            scene_id=326,
            succ_reward={
                [0]=_itemlist.i_31324_2_1,
                [1]=_itemlist.i_26232_50_1
            },
            fail_reward={
                [0]=_itemlist.i_31323_2_1,
                [1]=_itemlist.i_26232_50_1
            },
            target_id=244
        }
    }
}
SetConfigDefaultVal(Config.rabbit_run_auto,_default_val,nil,nil,nil)