local _default_val={
    gift_configure={
        section_start=1,
        section_end=7,
        seq=0,
        activity_days=1,
        show_times="腊月二十七"
    }
}
local _itemlist={
    i_26356_18_1={
        item_id=26356,
        num=18,
        is_bind=1
    },
    i_26813_1_1={
        item_id=26813,
        num=1,
        is_bind=1
    },
    i_26813_2_1={
        item_id=26813,
        num=2,
        is_bind=1
    },
    i_27541_1_1={
        item_id=27541,
        num=1,
        is_bind=1
    },
    i_27665_1_1={
        item_id=27665,
        num=1,
        is_bind=1
    },
    i_24807_1_1={
        item_id=24807,
        num=1,
        is_bind=1
    }
}
Config.yinhubaisui_auto={
    gift_configure={
        {
            type=0,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_26813_1_1
            }
        },
        {
            type=1,
            seq=1,
            activity_days=2,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_26813_1_1
            },
            show_times="腊月二十八"
        },
        {
            type=2,
            seq=2,
            activity_days=3,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_26813_2_1
            },
            show_times="腊月二十九"
        },
        {
            type=3,
            seq=3,
            activity_days=4,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_26813_1_1
            },
            show_times="正月初一"
        },
        {
            type=4,
            seq=4,
            activity_days=5,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_26813_1_1
            },
            show_times="正月初二"
        },
        {
            type=5,
            seq=5,
            activity_days=6,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_26813_2_1
            },
            show_times="正月初三"
        },
        {
            type=6,
            section_start=8,
            section_end=21,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27541_1_1
            }
        },
        {
            type=7,
            section_start=8,
            section_end=21,
            seq=1,
            activity_days=2,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27541_1_1
            },
            show_times="腊月二十八"
        },
        {
            type=8,
            section_start=8,
            section_end=21,
            seq=2,
            activity_days=3,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27541_1_1
            },
            show_times="腊月二十九"
        },
        {
            type=9,
            section_start=8,
            section_end=21,
            seq=3,
            activity_days=4,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27541_1_1
            },
            show_times="正月初一"
        },
        {
            type=10,
            section_start=8,
            section_end=21,
            seq=4,
            activity_days=5,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27541_1_1
            },
            show_times="正月初二"
        },
        {
            type=11,
            section_start=8,
            section_end=21,
            seq=5,
            activity_days=6,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27541_1_1
            },
            show_times="正月初三"
        },
        {
            type=12,
            section_start=22,
            section_end=0,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27665_1_1
            }
        },
        {
            type=13,
            section_start=22,
            section_end=0,
            seq=1,
            activity_days=2,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27665_1_1
            },
            show_times="腊月二十八"
        },
        {
            type=14,
            section_start=22,
            section_end=0,
            seq=2,
            activity_days=3,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27665_1_1
            },
            show_times="腊月二十九"
        },
        {
            type=15,
            section_start=22,
            section_end=0,
            seq=3,
            activity_days=4,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27665_1_1
            },
            show_times="正月初一"
        },
        {
            type=16,
            section_start=22,
            section_end=0,
            seq=4,
            activity_days=5,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27665_1_1
            },
            show_times="正月初二"
        },
        {
            type=17,
            section_start=22,
            section_end=0,
            seq=5,
            activity_days=6,
            reward_item={
                [0]=_itemlist.i_26356_18_1,
                [1]=_itemlist.i_27665_1_1
            },
            show_times="正月初三"
        }
    },
    buy_configure={
        {
            type=0,
            section_start=1,
            section_end=0,
            reward_item=_itemlist.i_24807_1_1,
            price=980,
            is_show=1
        }
    },
    other={
        {
            cfg_ver=2
        }
    }
}
SetConfigDefaultVal(Config.yinhubaisui_auto,_default_val,nil,nil,nil)