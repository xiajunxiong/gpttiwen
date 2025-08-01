local _default_val={
    task={
        section=1,
        seq1=0,
        describe="每日登录",
        type=0,
        parameter=1
    },
    reward={
        section_start=1,
        section_end=2,
        seq=0,
        task=1
    }
}
local _itemlist={
    i_27847_1_1={
        item_id=27847,
        num=1,
        is_bind=1
    },
    i_26813_1_1={
        item_id=26813,
        num=1,
        is_bind=1
    },
    i_27847_4_1={
        item_id=27847,
        num=4,
        is_bind=1
    },
    i_27847_2_1={
        item_id=27847,
        num=2,
        is_bind=1
    },
    i_27541_1_1={
        item_id=27541,
        num=1,
        is_bind=1
    },
    i_27187_1_1={
        item_id=27187,
        num=1,
        is_bind=1
    }
}
Config.chaozhifenxiang_auto={
    task={
        {},
        {
            seq1=1,
            describe="完成魔族在哪里1次",
            type=5
        },
        {
            seq1=2,
            describe="挑战再战云泽首领1次",
            type=2
        },
        {
            section=2
        },
        {
            section=2,
            seq1=1,
            describe="完成魔族在哪里1次",
            type=5
        },
        {
            section=2,
            seq1=2,
            describe="挑战再战云泽首领1次",
            type=2
        },
        {
            section=3
        },
        {
            section=3,
            seq1=1,
            describe="完成魔族在哪里1次",
            type=5
        },
        {
            section=3,
            seq1=2,
            describe="挑战再战云泽首领1次",
            type=2
        },
        {
            section=4
        },
        {
            section=4,
            seq1=1,
            describe="参与精灵大抓捕1次",
            type=6
        },
        {
            section=4,
            seq1=2,
            describe="挑战再战云泽首领1次",
            type=2
        },
        {
            section=5
        },
        {
            section=5,
            seq1=1,
            describe="参与精灵大抓捕1次",
            type=6
        },
        {
            section=5,
            seq1=2,
            describe="挑战再战云泽首领1次",
            type=2
        },
        {
            section=6
        },
        {
            section=6,
            seq1=1,
            describe="参与精灵大抓捕1次",
            type=6
        },
        {
            section=6,
            seq1=2,
            describe="挑战再战云泽首领1次",
            type=2
        }
    },
    reward={
        {
            type=0,
            reward_item={
                [0]=_itemlist.i_27847_1_1
            }
        },
        {
            type=1,
            seq=1,
            task=2,
            reward_item={
                [0]=_itemlist.i_26813_1_1
            }
        },
        {
            type=2,
            seq=2,
            task=3,
            reward_item={
                [0]=_itemlist.i_27847_4_1
            }
        },
        {
            type=3,
            section_start=3,
            section_end=14,
            reward_item={
                [0]=_itemlist.i_27847_2_1
            }
        },
        {
            type=4,
            section_start=3,
            section_end=14,
            seq=1,
            task=2,
            reward_item={
                [0]=_itemlist.i_27541_1_1
            }
        },
        {
            type=5,
            section_start=3,
            section_end=14,
            seq=2,
            task=3,
            reward_item={
                [0]=_itemlist.i_27847_4_1
            }
        },
        {
            type=6,
            section_start=15,
            section_end=0,
            reward_item={
                [0]=_itemlist.i_27847_2_1
            }
        },
        {
            type=7,
            section_start=15,
            section_end=0,
            seq=1,
            task=2,
            reward_item={
                [0]=_itemlist.i_27187_1_1
            }
        },
        {
            type=8,
            section_start=15,
            section_end=0,
            seq=2,
            task=3,
            reward_item={
                [0]=_itemlist.i_27847_4_1
            }
        }
    },
    other={
        {
            price_type=3,
            price=30,
            buy_type=1,
            cfg_ver=4,
            desc="每日重置购买，立享云泽门票",
            reward_item=_itemlist.i_27847_1_1
        }
    }
}
SetConfigDefaultVal(Config.chaozhifenxiang_auto,_default_val,nil,nil,nil)