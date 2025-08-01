local _default_val={
    task={
        section=6,
        seq1=0,
        describe="每日登录",
        type=1,
        parameter=1
    },
    reward_buy={
        seq=1,
        section_start=1,
        section_end=2,
        seq1=0,
        task=2
    }
}
local _itemlist={
    i_22014_1_1={
        item_id=22014,
        num=1,
        is_bind=1
    },
    i_27847_5_1={
        item_id=27847,
        num=5,
        is_bind=1
    },
    i_26813_1_1={
        item_id=26813,
        num=1,
        is_bind=1
    },
    i_28522_2_1={
        item_id=28522,
        num=2,
        is_bind=1
    },
    i_27541_1_1={
        item_id=27541,
        num=1,
        is_bind=1
    },
    i_27187_3_1={
        item_id=27187,
        num=3,
        is_bind=1
    },
    i_29077_1_1={
        item_id=29077,
        num=1,
        is_bind=1
    },
    i_26416_2_1={
        item_id=26416,
        num=2,
        is_bind=1
    },
    i_26813_2_1={
        item_id=26813,
        num=2,
        is_bind=1
    },
    i_27541_2_1={
        item_id=27541,
        num=2,
        is_bind=1
    },
    i_27187_5_1={
        item_id=27187,
        num=5,
        is_bind=1
    },
    i_26416_5_1={
        item_id=26416,
        num=5,
        is_bind=1
    }
}
Config.fenxiangjiabei_auto={
    task={
        {
            seq=0,
            section=1
        },
        {
            seq=1,
            section=1,
            seq1=1,
            describe="探索云泽画境7次",
            type=2,
            parameter=7
        },
        {
            seq=2,
            section=2
        },
        {
            seq=3,
            section=2,
            seq1=1,
            describe="挑战再战云泽首领1次",
            type=3
        },
        {
            seq=4,
            section=3
        },
        {
            seq=5,
            section=3,
            seq1=1,
            describe="探索云泽画境7次",
            type=2,
            parameter=7
        },
        {
            seq=6,
            section=4
        },
        {
            seq=7,
            section=4,
            seq1=1,
            describe="云泽繁花录获得30积分",
            type=5,
            parameter=30
        },
        {
            seq=8,
            section=5
        },
        {
            seq=9,
            section=5,
            seq1=1,
            describe="云泽繁花录获得30积分",
            type=5,
            parameter=30
        },
        {
            seq=10
        },
        {
            seq=11,
            seq1=1,
            describe="云泽繁花录获得30积分",
            type=5,
            parameter=30
        },
        {
            seq=12,
            seq1=2,
            describe="云泽繁花录获得50积分",
            type=5,
            parameter=50
        },
        {
            seq=13,
            seq1=3,
            describe="挑战再战云泽首领1次",
            type=3
        },
        {
            seq=14,
            seq1=4,
            describe="挑战再战云泽首领2次",
            type=3,
            parameter=2
        },
        {
            seq=15,
            seq1=5,
            describe="挑战再战云泽首领3次",
            type=3,
            parameter=3
        },
        {
            seq=16,
            seq1=6,
            describe="挑战再战云泽首领5次",
            type=3,
            parameter=5
        }
    },
    reward={
        {
            section_start=1,
            section_end=0,
            reward_item={
                [0]=_itemlist.i_22014_1_1
            }
        }
    },
    reward_buy={
        {
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26813_1_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            section_start=3,
            section_end=14,
            seq1=1,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27541_1_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            section_start=15,
            section_end=28,
            seq1=2,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27187_3_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            section_start=29,
            section_end=0,
            seq1=3,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26416_2_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=2,
            task=3,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26813_2_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=2,
            section_start=3,
            section_end=14,
            seq1=1,
            task=3,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27541_2_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=2,
            section_start=15,
            section_end=28,
            seq1=2,
            task=3,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27187_5_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=2,
            section_start=29,
            section_end=0,
            seq1=3,
            task=3,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26416_5_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=3,
            task=4,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26813_1_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=3,
            section_start=3,
            section_end=14,
            seq1=1,
            task=4,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27541_1_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=3,
            section_start=15,
            section_end=28,
            seq1=2,
            task=4,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27187_3_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=3,
            section_start=29,
            section_end=0,
            seq1=3,
            task=4,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26416_2_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=4,
            task=5,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26813_2_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=4,
            section_start=3,
            section_end=14,
            seq1=1,
            task=5,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27541_2_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=4,
            section_start=15,
            section_end=28,
            seq1=2,
            task=5,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27187_5_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=4,
            section_start=29,
            section_end=0,
            seq1=3,
            task=5,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26416_5_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=5,
            task=6,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26813_1_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=5,
            section_start=3,
            section_end=14,
            seq1=1,
            task=6,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27541_1_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=5,
            section_start=15,
            section_end=28,
            seq1=2,
            task=6,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27187_3_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=5,
            section_start=29,
            section_end=0,
            seq1=3,
            task=6,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26416_2_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=6,
            task=7,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26813_2_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=6,
            section_start=3,
            section_end=14,
            seq1=1,
            task=7,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27541_2_1,
                [2]=_itemlist.i_28522_2_1
            }
        },
        {
            seq=6,
            section_start=15,
            section_end=28,
            seq1=2,
            task=7,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_27187_5_1,
                [2]=_itemlist.i_29077_1_1
            }
        },
        {
            seq=6,
            section_start=29,
            section_end=0,
            seq1=3,
            task=7,
            reward_item={
                [0]=_itemlist.i_27847_5_1,
                [1]=_itemlist.i_26416_5_1,
                [2]=_itemlist.i_29077_1_1
            }
        }
    },
    other={
        {
            price_type=1,
            price=300,
            buy_type=2,
            cfg_ver=4,
            must_type=2,
            currency_must=50000,
            desc="50000灵玉"
        }
    }
}
SetConfigDefaultVal(Config.fenxiangjiabei_auto,_default_val,nil,nil,nil)