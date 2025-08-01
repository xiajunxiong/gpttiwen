local _default_val={
    leitai={
        leitai_level=0,
        leitai_type=1,
        reward_id=0,
        x=211,
        y=324,
        position=0
    },
    reward={
        reward_id=0,
        min_rank=1,
        max_rank=2,
        bind_coin=250000,
        points=500
    },
    pos={
        pet_num=5,
        parnter_num=5,
        scene_id=341,
        enter_x=304,
        enter_y=152
    },
    leitai_achieve={
        ach_group=2,
        type=6,
        seq=0,
        platform_list=-1,
        param=3,
        progress_way=1,
        price_type=8,
        price_num=60
    }
}
local _itemlist={
    i_27974_80_1={
        item_id=27974,
        num=80,
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
    i_27974_40_1={
        item_id=27974,
        num=40,
        is_bind=1
    },
    i_27974_30_1={
        item_id=27974,
        num=30,
        is_bind=1
    },
    i_27974_25_1={
        item_id=27974,
        num=25,
        is_bind=1
    },
    i_27974_20_1={
        item_id=27974,
        num=20,
        is_bind=1
    },
    i_27974_15_1={
        item_id=27974,
        num=15,
        is_bind=1
    },
    i_27974_10_1={
        item_id=27974,
        num=10,
        is_bind=1
    },
    i_27576_80_1={
        item_id=27576,
        num=80,
        is_bind=1
    },
    i_27576_70_1={
        item_id=27576,
        num=70,
        is_bind=1
    },
    i_27576_60_1={
        item_id=27576,
        num=60,
        is_bind=1
    },
    i_27576_50_1={
        item_id=27576,
        num=50,
        is_bind=1
    },
    i_27576_40_1={
        item_id=27576,
        num=40,
        is_bind=1
    },
    i_27576_30_1={
        item_id=27576,
        num=30,
        is_bind=1
    },
    i_27576_25_1={
        item_id=27576,
        num=25,
        is_bind=1
    },
    i_27576_20_1={
        item_id=27576,
        num=20,
        is_bind=1
    },
    i_27576_15_1={
        item_id=27576,
        num=15,
        is_bind=1
    },
    i_27576_10_1={
        item_id=27576,
        num=10,
        is_bind=1
    }
}
Config.leitai_cfg_auto={
    leitai={
        {
            seq=1,
            leitai_name="火神·壹",
            x=212,
            y=40,
            npc_seq=5000
        },
        {
            seq=2,
            leitai_name="火神·贰",
            x=383,
            y=125,
            npc_seq=5001
        },
        {
            seq=3,
            leitai_name="火神·叁",
            x=382,
            npc_seq=5002
        },
        {
            seq=4,
            leitai_name="火神·肆",
            y=406,
            npc_seq=5003
        },
        {
            seq=5,
            leitai_name="火神·伍",
            x=39,
            npc_seq=5004
        },
        {
            seq=6,
            leitai_name="火神·陆",
            x=41,
            y=124,
            npc_seq=5005
        },
        {
            seq=7,
            leitai_name="风神·壹",
            reward_id=1,
            x=382,
            position=1,
            npc_seq=5078
        },
        {
            seq=8,
            leitai_name="风神·贰",
            reward_id=1,
            x=383,
            y=123,
            position=1,
            npc_seq=5079
        },
        {
            seq=9,
            leitai_name="风神·叁",
            reward_id=1,
            y=39,
            position=1,
            npc_seq=5084
        },
        {
            seq=10,
            leitai_name="风神·肆",
            reward_id=1,
            y=407,
            position=1,
            npc_seq=5083
        },
        {
            seq=11,
            leitai_name="风神·伍",
            reward_id=1,
            x=39,
            y=322,
            position=1,
            npc_seq=5082
        },
        {
            seq=12,
            leitai_name="风神·陆",
            reward_id=1,
            x=40,
            y=123,
            position=1,
            npc_seq=5085
        }
    },
    other={
        {
            leitai_scene=108,
            robot_level=65
        }
    },
    reward={
        {
            reward={
                [0]=_itemlist.i_27974_80_1
            }
        },
        {
            min_rank=2,
            max_rank=3,
            reward={
                [0]=_itemlist.i_27974_70_1
            },
            bind_coin=200000
        },
        {
            min_rank=3,
            max_rank=4,
            reward={
                [0]=_itemlist.i_27974_60_1
            },
            bind_coin=180000
        },
        {
            min_rank=4,
            max_rank=5,
            reward={
                [0]=_itemlist.i_27974_50_1
            },
            bind_coin=160000
        },
        {
            min_rank=5,
            max_rank=6,
            reward={
                [0]=_itemlist.i_27974_40_1
            },
            bind_coin=140000
        },
        {
            min_rank=6,
            max_rank=7,
            reward={
                [0]=_itemlist.i_27974_30_1
            },
            bind_coin=120000
        },
        {
            min_rank=7,
            max_rank=8,
            reward={
                [0]=_itemlist.i_27974_25_1
            },
            bind_coin=100000
        },
        {
            min_rank=8,
            max_rank=9,
            reward={
                [0]=_itemlist.i_27974_20_1
            },
            bind_coin=80000
        },
        {
            min_rank=9,
            max_rank=10,
            reward={
                [0]=_itemlist.i_27974_15_1
            },
            bind_coin=60000
        },
        {
            min_rank=10,
            max_rank=11,
            reward={
                [0]=_itemlist.i_27974_10_1
            },
            bind_coin=40000
        },
        {
            reward_id=1,
            reward={
                [0]=_itemlist.i_27576_80_1
            }
        },
        {
            reward_id=1,
            min_rank=2,
            max_rank=3,
            reward={
                [0]=_itemlist.i_27576_70_1
            },
            bind_coin=200000
        },
        {
            reward_id=1,
            min_rank=3,
            max_rank=4,
            reward={
                [0]=_itemlist.i_27576_60_1
            },
            bind_coin=180000
        },
        {
            reward_id=1,
            min_rank=4,
            max_rank=5,
            reward={
                [0]=_itemlist.i_27576_50_1
            },
            bind_coin=160000
        },
        {
            reward_id=1,
            min_rank=5,
            max_rank=6,
            reward={
                [0]=_itemlist.i_27576_40_1
            },
            bind_coin=140000
        },
        {
            reward_id=1,
            min_rank=6,
            max_rank=7,
            reward={
                [0]=_itemlist.i_27576_30_1
            },
            bind_coin=120000
        },
        {
            reward_id=1,
            min_rank=7,
            max_rank=8,
            reward={
                [0]=_itemlist.i_27576_25_1
            },
            bind_coin=100000
        },
        {
            reward_id=1,
            min_rank=8,
            max_rank=9,
            reward={
                [0]=_itemlist.i_27576_20_1
            },
            bind_coin=80000
        },
        {
            reward_id=1,
            min_rank=9,
            max_rank=10,
            reward={
                [0]=_itemlist.i_27576_15_1
            },
            bind_coin=60000
        },
        {
            reward_id=1,
            min_rank=10,
            max_rank=11,
            reward={
                [0]=_itemlist.i_27576_10_1
            },
            bind_coin=40000
        }
    },
    pos={
        {
            seq=1,
            position=0,
            desc="火神擂台"
        },
        {
            seq=2,
            position=1,
            desc="风神擂台"
        }
    },
    robot_level={
        {
            seq=1
        },
        {
            seq=2
        },
        {
            seq=3
        },
        {
            seq=4
        },
        {
            seq=5
        },
        {
            seq=6
        },
        {
            seq=7
        },
        {
            seq=8
        },
        {
            seq=9
        },
        {
            seq=10
        },
        {
            seq=11
        },
        {
            seq=12
        },
        {
            seq=13
        },
        {
            seq=14
        },
        {
            seq=15
        },
        {
            seq=16
        },
        {
            seq=17
        },
        {
            seq=18
        },
        {
            seq=19
        },
        {
            seq=20
        },
        {
            seq=21
        },
        {
            seq=22
        },
        {
            seq=23
        },
        {
            seq=24
        },
        {
            seq=25
        },
        {
            seq=26
        },
        {
            seq=27
        },
        {
            seq=28
        },
        {
            seq=29
        },
        {
            seq=30
        },
        {
            seq=31
        },
        {
            seq=32
        },
        {
            seq=33
        },
        {
            seq=34
        },
        {
            seq=35
        },
        {
            seq=36
        }
    },
    leitai_achieve={
        {
            sn=1,
            ach_group=1,
            ach_name="勇猛·壹",
            ach_dsc="任何一个擂台，进入前10名",
            type=0,
            param=10,
            progress_way=2,
            price_num=30
        },
        {
            sn=2,
            ach_group=1,
            ach_name="勇猛·贰",
            ach_dsc="任何一个擂台，进入前5名",
            type=0,
            seq=1,
            param=5,
            progress_way=2,
            price_num=45
        },
        {
            sn=3,
            ach_group=1,
            ach_name="勇猛·叁",
            ach_dsc="任何一个擂台，进入前3名",
            type=0,
            seq=2,
            progress_way=2
        },
        {
            sn=4,
            ach_group=1,
            ach_name="勇猛·肆",
            ach_dsc="任何一个擂台，登顶第1名",
            type=0,
            seq=3,
            param=1,
            progress_way=2,
            price_num=75
        },
        {
            sn=5,
            ach_name="连捷·壹",
            ach_dsc="在任意擂台挑战获胜1次",
            param=1,
            price_num=20
        },
        {
            sn=6,
            ach_name="连捷·贰",
            ach_dsc="在任意擂台挑战获胜3次",
            seq=1,
            price_num=40
        },
        {
            sn=7,
            ach_name="连捷·叁",
            ach_dsc="在任意擂台挑战获胜5次",
            seq=2,
            param=5
        },
        {
            sn=8,
            ach_name="连捷·肆",
            ach_dsc="在任意擂台挑战获胜7次",
            seq=3,
            param=7,
            price_num=80
        },
        {
            sn=9,
            ach_name="连捷·伍",
            ach_dsc="在任意擂台挑战获胜10次",
            seq=4,
            param=10,
            price_num=100
        },
        {
            sn=10,
            ach_group=3,
            ach_name="荣耀·壹",
            ach_dsc="在任意擂台持续登顶超过2天",
            type=8,
            param=2,
            price_num=20
        },
        {
            sn=11,
            ach_group=3,
            ach_name="荣耀·贰",
            ach_dsc="在任意擂台持续登顶超过3天",
            type=8,
            seq=1,
            price_num=40
        },
        {
            sn=12,
            ach_group=3,
            ach_name="荣耀·叁",
            ach_dsc="在任意擂台持续登顶超过4天",
            type=8,
            seq=2,
            param=4
        },
        {
            sn=13,
            ach_group=3,
            ach_name="荣耀·肆",
            ach_dsc="在任意擂台持续登顶超过5天",
            type=8,
            seq=3,
            param=5,
            price_num=80
        },
        {
            sn=14,
            ach_group=3,
            ach_name="荣耀·伍",
            ach_dsc="在任意擂台持续登顶超过6天",
            type=8,
            seq=4,
            param=6,
            price_num=100
        },
        {
            sn=23,
            ach_group=6,
            ach_name="无双·壹",
            ach_dsc="战胜阵容评分高于自己的阵容1次",
            type=11,
            param=1,
            price_num=20
        },
        {
            sn=24,
            ach_group=6,
            ach_name="无双·贰",
            ach_dsc="战胜阵容评分高于自己的阵容3次",
            type=11,
            seq=1,
            price_num=40
        },
        {
            sn=25,
            ach_group=6,
            ach_name="无双·叁",
            ach_dsc="战胜阵容评分高于自己的阵容5次",
            type=11,
            seq=2,
            param=5
        },
        {
            sn=26,
            ach_group=6,
            ach_name="无双·肆",
            ach_dsc="战胜阵容评分高于自己的阵容7次",
            type=11,
            seq=3,
            param=7,
            price_num=80
        },
        {
            sn=27,
            ach_group=6,
            ach_name="无双·伍",
            ach_dsc="战胜阵容评分高于自己的阵容10次",
            type=11,
            seq=4,
            param=10,
            price_num=100
        },
        {
            sn=15,
            ach_group=4,
            ach_name="群起·壹",
            ach_dsc="上阵擂台伙伴总数超过3人",
            type=9,
            price_num=30
        },
        {
            sn=16,
            ach_group=4,
            ach_name="群起·贰",
            ach_dsc="上阵擂台伙伴总数超过6人",
            type=9,
            seq=1,
            param=6,
            price_num=45
        },
        {
            sn=17,
            ach_group=4,
            ach_name="群起·叁",
            ach_dsc="上阵擂台伙伴总数超过9人",
            type=9,
            seq=2,
            param=9
        },
        {
            sn=18,
            ach_group=4,
            ach_name="群起·肆",
            ach_dsc="上阵擂台伙伴总数超过12人",
            type=9,
            seq=3,
            param=12,
            price_num=75
        },
        {
            sn=19,
            ach_group=5,
            ach_name="灵兽·壹",
            ach_dsc="上阵擂台宠物总数超过3只",
            type=10,
            price_num=30
        },
        {
            sn=20,
            ach_group=5,
            ach_name="灵兽·贰",
            ach_dsc="上阵擂台宠物总数超过6只",
            type=10,
            seq=1,
            param=6,
            price_num=45
        },
        {
            sn=21,
            ach_group=5,
            ach_name="灵兽·叁",
            ach_dsc="上阵擂台宠物总数超过9只",
            type=10,
            seq=2,
            param=9
        },
        {
            sn=22,
            ach_group=5,
            ach_name="灵兽·肆",
            ach_dsc="上阵擂台宠物总数超过12只",
            type=10,
            seq=3,
            param=12,
            price_num=75
        }
    }
}
SetConfigDefaultVal(Config.leitai_cfg_auto,_default_val,nil,nil,nil)