local _default_val={
    basis_configuration={
        section_start=1,
        section_end=0,
        light_type=1,
        light_condition=0,
        parameter=50,
        reward_group=3
    },
    reward_group={
        reward_group=1
    }
}
local _itemlist={
    i_27402_1_1={
        item_id=27402,
        num=1,
        is_bind=1
    },
    i_22007_30_1={
        item_id=22007,
        num=30,
        is_bind=1
    },
    i_27575_1_1={
        item_id=27575,
        num=1,
        is_bind=1
    },
    i_26812_1_1={
        item_id=26812,
        num=1,
        is_bind=1
    },
    i_27398_3_1={
        item_id=27398,
        num=3,
        is_bind=1
    },
    i_24557_1_1={
        item_id=24557,
        num=1,
        is_bind=1
    },
    i_26692_1_1={
        item_id=26692,
        num=1,
        is_bind=1
    }
}
Config.zongxiangmancheng_auto={
    basis_configuration={
        {
            type=0,
            seq=0
        },
        {
            type=1,
            seq=1,
            parameter=100
        },
        {
            type=2,
            seq=2,
            parameter=120
        },
        {
            type=3,
            seq=3,
            light_condition=1,
            parameter=3,
            reward_group=1
        },
        {
            type=4,
            seq=4,
            light_type=2
        },
        {
            type=5,
            seq=5,
            light_type=2,
            parameter=100
        },
        {
            type=6,
            seq=6,
            light_type=2,
            parameter=120
        },
        {
            type=7,
            seq=7,
            light_type=2,
            light_condition=1,
            parameter=3,
            reward_group=2
        }
    },
    reward_group={
        {
            reward_item={
                [0]=_itemlist.i_27402_1_1
            }
        },
        {
            reward_item={
                [0]=_itemlist.i_22007_30_1
            }
        },
        {
            reward_item={
                [0]=_itemlist.i_27575_1_1
            }
        },
        {
            reward_group=2,
            reward_item={
                [0]=_itemlist.i_26812_1_1
            }
        },
        {
            reward_group=2,
            reward_item={
                [0]=_itemlist.i_27398_3_1
            }
        },
        {
            reward_group=2,
            reward_item={
                [0]=_itemlist.i_24557_1_1
            }
        },
        {
            reward_group=3,
            reward_item={
                [0]=_itemlist.i_26692_1_1
            }
        }
    },
    show_reward={
        {
            type=0,
            section_start=1,
            section_end=0,
            reward_item_1={
                [0]=_itemlist.i_27402_1_1,
                [1]=_itemlist.i_22007_30_1,
                [2]=_itemlist.i_27575_1_1
            },
            reward_item_2={
                [0]=_itemlist.i_26812_1_1,
                [1]=_itemlist.i_27398_3_1,
                [2]=_itemlist.i_24557_1_1
            }
        }
    }
}
SetConfigDefaultVal(Config.zongxiangmancheng_auto,_default_val,nil,nil,nil)