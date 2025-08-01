local _default_val={
    heritage_treasure={
        clue1="玄都",
        clue3="玄都",
        weight=100
    },
    item_show={
        section_start=1,
        section_end=7,
        seq=0
    },
    reward_pool={
        section_start=1,
        section_end=7,
        weight=330,
        is_hearsay=0
    }
}
local _itemlist={
    i_26344_20_1={
        item_id=26344,
        num=20,
        is_bind=1
    },
    i_26344_2_1={
        item_id=26344,
        num=2,
        is_bind=1
    },
    i_65561_1_1={
        item_id=65561,
        num=1,
        is_bind=1
    },
    i_24425_1_1={
        item_id=24425,
        num=1,
        is_bind=1
    },
    i_29018_1_1={
        item_id=29018,
        num=1,
        is_bind=1
    },
    i_24426_1_1={
        item_id=24426,
        num=1,
        is_bind=1
    },
    i_24427_1_1={
        item_id=24427,
        num=1,
        is_bind=1
    },
    i_22020_2_1={
        item_id=22020,
        num=2,
        is_bind=1
    },
    i_27671_2_1={
        item_id=27671,
        num=2,
        is_bind=1
    },
    i_24428_1_1={
        item_id=24428,
        num=1,
        is_bind=1
    },
    i_27654_2_1={
        item_id=27654,
        num=2,
        is_bind=1
    },
    i_29060_1_1={
        item_id=29060,
        num=1,
        is_bind=1
    },
    i_28611_1_1={
        item_id=28611,
        num=1,
        is_bind=1
    }
}
Config.treasure_activity_auto={
    other={
        {
            section_start=1,
            section_end=0,
            reward_item=_itemlist.i_26344_20_1,
            consume_item_id=27401,
            time=1,
            coin_num=25000,
            gold_num=10,
            is_complement=0
        }
    },
    heritage_treasure={
        {
            type=1,
            clue1="千手观音",
            clue2="玄都",
            clue3="莲花座",
            npc_id=40015,
            weight=50
        },
        {
            type=2,
            clue1="风度翩翩",
            clue2="气宇轩昂",
            clue3="日轮坊",
            npc_id=40400
        },
        {
            type=3,
            clue1="玄都东郊",
            clue2="舍我其谁",
            clue3="震慑一方",
            npc_id=40102,
            weight=500
        },
        {
            type=4,
            clue1="宛村",
            clue2="风平浪静",
            clue3="百川归海",
            npc_id=40136,
            weight=30
        },
        {
            type=5,
            clue1="左钳右锤",
            clue2="千锤百炼",
            npc_id=40027
        },
        {
            type=6,
            clue1="樱花枫叶",
            clue2="玄都南郊",
            clue3="爱花名花",
            npc_id=40404,
            weight=300
        },
        {
            type=7,
            clue2="惩恶扬善",
            clue3="窃贼克星",
            npc_id=40107,
            weight=200
        },
        {
            type=8,
            clue1="广陵郊外",
            clue2="清清静静",
            clue3="如履薄冰",
            npc_id=46562,
            weight=155
        },
        {
            type=9,
            clue2="岁近黄花",
            clue3="芳柳似画",
            npc_id=40132,
            weight=133
        },
        {
            type=10,
            clue1="广陵郊外",
            clue2="剑眉星目",
            clue3="锋芒逼人",
            npc_id=46564,
            weight=154
        },
        {
            type=11,
            clue1="草帽蓑衣",
            clue2="浪里白条",
            npc_id=46522,
            weight=133
        },
        {
            type=12,
            clue1="人小鬼大",
            clue2="冰糖葫芦",
            clue3="青云村",
            npc_id=40023,
            weight=152
        },
        {
            type=13,
            clue2="巾帼不让须眉",
            clue3="相忘于江湖",
            npc_id=46500,
            weight=144
        }
    },
    item_show={
        {
            item=_itemlist.i_26344_2_1
        },
        {
            seq=1,
            item=_itemlist.i_65561_1_1
        },
        {
            section_start=8,
            section_end=21,
            item=_itemlist.i_26344_2_1
        },
        {
            section_start=8,
            section_end=21,
            seq=1,
            item=_itemlist.i_65561_1_1
        },
        {
            section_start=22,
            section_end=0,
            item=_itemlist.i_26344_2_1
        },
        {
            section_start=22,
            section_end=0,
            seq=1,
            item=_itemlist.i_65561_1_1
        }
    },
    reward_pool={
        {
            seq=0,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_24425_1_1,
                [2]=_itemlist.i_29018_1_1
            },
            weight=340
        },
        {
            seq=1,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_24426_1_1,
                [2]=_itemlist.i_29018_1_1
            }
        },
        {
            seq=2,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_24427_1_1,
                [2]=_itemlist.i_29018_1_1
            }
        },
        {
            seq=3,
            section_start=8,
            section_end=21,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_24426_1_1,
                [2]=_itemlist.i_22020_2_1
            },
            weight=340
        },
        {
            seq=4,
            section_start=8,
            section_end=21,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_24427_1_1,
                [2]=_itemlist.i_27671_2_1
            }
        },
        {
            seq=5,
            section_start=8,
            section_end=21,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_24428_1_1,
                [2]=_itemlist.i_27654_2_1
            }
        },
        {
            seq=6,
            section_start=22,
            section_end=0,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_29060_1_1,
                [2]=_itemlist.i_28611_1_1
            },
            weight=340
        },
        {
            seq=7,
            section_start=22,
            section_end=0,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_29060_1_1,
                [2]=_itemlist.i_27671_2_1
            }
        },
        {
            seq=8,
            section_start=22,
            section_end=0,
            reward_item={
                [0]=_itemlist.i_26344_2_1,
                [1]=_itemlist.i_29060_1_1,
                [2]=_itemlist.i_27654_2_1
            }
        }
    }
}
SetConfigDefaultVal(Config.treasure_activity_auto,_default_val,nil,nil,nil)