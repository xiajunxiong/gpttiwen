NpcHelper = {}

function NpcHelper.NpcCo(npc_id,miss_report)
    return Cfg.NpcById(npc_id,miss_report)
end

function NpcHelper.NpcType(npc_co)
    return npc_co.npc_type or 0
end

function NpcHelper.NpcSeqToId(npc_seq)
    return (Cfg.NpcBySeq(npc_seq) or {}).id
end

----------------------------------------------------------------------------------------------------
------------------------------------ npc_type start ------------------------------------------------
----------------------------------------------------------------------------------------------------

-- npc_type
-- 类型id      npc类型     区间    例如(没有继续细分的就是类型id 比如说世界boss就是1101 篝火npc就是6000)
-- 0        逻辑Npc         0-999
    -- 1        自定义Npc       1-9         行为服务端赋予的Npc
    -- 100       商店Npc        100-19      声望商店Npc
-- 1000     怪物Npc         1000-1999   怪物Npc
    -- 1100     Boss            1100-1199   Boss
        -- 1101     世界Boss        1101-1199   世界Boss
    -- 1200     精英怪          1200-1299   精英怪
-- 2000     采集物Npc       2000-2999   采集副本采集物、水晶副本采集物
        -- 2010     屏蔽名字框        2010-2019    任务采集物
    -- 2100     植物             2100-2199    植物
    -- 2200     木材             2200-2299    木材
    -- 2300     矿物             2200-2299    矿物
-- 3000     宝箱Npc         3000-3999   水晶副本宝箱
    -- 3001     宝箱Npc        3001        宝箱屏蔽图标
    -- 3100     铜宝箱         3100-3199   水晶副本铜宝箱
    -- 3200     银宝箱         3200-3299   水晶副本银宝箱
    -- 3300     金宝箱         3300-3399   水晶副本金宝箱
-- 4000     入口Npc         4000-4999   野外场景副本传送点
-- 5000     货币Npc         5000-5999   水晶副本铜币
-- 6000     篝火Npc         6000-6999   水晶副本篝火
-- 7000     擂台Npc         7000-7999   擂台战场景的擂台

function NpcHelper.NpcTypeStruct()
    return {
        normal = false,
            custom = false,
            shop = false,

        monster = false,
            boss = false,
                world_boss = false,
            elite_monster = false,
        
        gather = false,
                gather_block = false,
            plant = false,
            wood = false,
            mineral = false,  

        box = false,
            box_bi = false,
            box_t = false,
            box_y = false,
            box_j = false,
        door = false,
        coin = false,
        bonfire = false,
        arena = false,
    }
end

-- 根据NpcId查找Npc类型
function NpcHelper.NpcTypesById(npc_id)
    local npc_co = NpcHelper.NpcCo(npc_id, false)
    return nil ~= npc_co and NpcHelper.NpcTypesByCo(npc_co) or NpcHelper.NpcTypeStruct()
end

-- 根据NpcCo查找Npc类型
function NpcHelper.NpcTypesByCo(npc_co)
    local npc_type = NpcHelper.NpcType(npc_co)

    local nts = NpcHelper.NpcTypeStruct()

    nts.normal = NpcHelper.IsNormalNpc(npc_type)
    nts.custom = NpcHelper.IsCustomNpc(npc_type)
    nts.shop = NpcHelper.IsShopNpc(npc_type)
    
    nts.monster = NpcHelper.IsMonsterNpc(npc_type)
    nts.boss = NpcHelper.IsBossNpc(npc_type)
    nts.world_boss = NpcHelper.IsWorldBossNpc(npc_type)
    nts.elite_monster = NpcHelper.IsEliteMonsterNpc(npc_type)
    
    nts.gather = NpcHelper.IsGatherNpc(npc_type)
    nts.gather_block = NpcHelper.IsGatherNpcBlock(npc_type)
    nts.plant = NpcHelper.IsPlantNpc(npc_type)
    nts.wood = NpcHelper.IsWoodNpc(npc_type)
    nts.mineral = NpcHelper.IsMineralNpc(npc_type)
    
    nts.box = NpcHelper.IsBoxNpc(npc_type)
    nts.box_bi = NpcHelper.IsBoxBlockIconNpc(npc_type)
    nts.box_t = NpcHelper.IsBoxTNpc(npc_type)
    nts.box_y = NpcHelper.IsBoxYNpc(npc_type)
    nts.box_j = NpcHelper.IsBoxJNpc(npc_type)

    nts.door = NpcHelper.IsDoorNpc(npc_type)
    nts.coin = NpcHelper.IsCoinNpc(npc_type)
    nts.bonfire = NpcHelper.IsBonfireNpc(npc_type)
    nts.arena = NpcHelper.IsArenaNpc(npc_type)

    nts.luck_box_c = NpcHelper.IsLuckBoxCNpc(npc_type)
    nts.luck_box_z = NpcHelper.IsLuckBoxZNpc(npc_type)
    nts.luck_box_g = NpcHelper.IsLuckBoxGNpc(npc_type)

    return nts
end

------------------------------------ npc_type start

-- 是否是逻辑Npc
function NpcHelper.IsNormalNpc(npc_type)
    return 0 <= npc_type and npc_type < 1000
end

-- 是否是自定义Npc
function NpcHelper.IsCustomNpc(npc_type)
    return 1 == npc_type
end

-- 是否是商店Npc
function NpcHelper.IsShopNpc(npc_type)
    return 100 == npc_type
end

-- 是否是怪物Npc
function NpcHelper.IsMonsterNpc(npc_type)
    return 1000 <= npc_type and npc_type < 1999
end

-- 是否是BossNpc
function NpcHelper.IsBossNpc(npc_type)
    return 1100 <= npc_type and npc_type < 1199
end

-- 是否是WorldBossNpc
function NpcHelper.IsWorldBossNpc(npc_type)
    return 1101 == npc_type
end

-- 是否是精英怪
function NpcHelper.IsEliteMonsterNpc(npc_type)
    return 1200 <= npc_type and npc_type < 1299
end

-- 是否是采集物Npc
function NpcHelper.IsGatherNpc(npc_type)
    return 2000 <= npc_type and npc_type < 2999
end

-- 是否是屏蔽名字框采集物Npc
function NpcHelper.IsGatherNpcBlock(npc_type)
    return 2010 <= npc_type and npc_type < 2019
end

-- 是否是植物Npc
function NpcHelper.IsPlantNpc(npc_type)
    return 2100 <= npc_type and npc_type < 2199
end

-- 是否是木材Npc
function NpcHelper.IsWoodNpc(npc_type)
    return 2200 <= npc_type and npc_type < 2299
end

-- 是否是矿物Npc
function NpcHelper.IsMineralNpc(npc_type)
    return 2300 <= npc_type and npc_type < 2399
end

-- 是否是宝箱Npc
function NpcHelper.IsBoxNpc(npc_type)
    return 3000 <= npc_type and npc_type < 3999
end

-- 是否是屏蔽图标宝箱Npc
function NpcHelper.IsBoxBlockIconNpc(npc_type)
    return 3001 == npc_type
end

-- 是否是铜宝箱Npc
function NpcHelper.IsBoxTNpc(npc_type)
    return 3100 <= npc_type and npc_type < 3199
end

-- 是否是银宝箱Npc
function NpcHelper.IsBoxYNpc(npc_type)
    return 3200 <= npc_type and npc_type < 3299
end

-- 是否是金宝箱Npc
function NpcHelper.IsBoxJNpc(npc_type)
    return 3300 <= npc_type and npc_type < 3399
end

-- 是否是传送点Npc
function NpcHelper.IsDoorNpc(npc_type)
    return 4000 <= npc_type and npc_type < 4999
end

-- 是否是货币Npc
function NpcHelper.IsCoinNpc(npc_type)
    return 5000 <= npc_type and npc_type < 5999
end

-- 是否是篝火Npc
function NpcHelper.IsBonfireNpc(npc_type)
    return 6000 <= npc_type and npc_type < 6999
end

-- 是否是擂台Npc
function NpcHelper.IsArenaNpc(npc_type)
    return 7000 <= npc_type and npc_type < 7999
end

-- 是否是迷宫初级宝箱
function NpcHelper.IsLuckBoxCNpc(npc_type)
    return 3400 <= npc_type and npc_type < 3499
end
-- 是否是迷宫中级宝箱
function NpcHelper.IsLuckBoxZNpc(npc_type)
    return 3500 <= npc_type and npc_type < 3599
end
-- 是否是迷宫高级宝箱
function NpcHelper.IsLuckBoxGNpc(npc_type)
    return 3600 <= npc_type and npc_type < 3699
end

------------------------------------ npc_type end

----------------------------------------------------------------------------------------------------
------------------------------------ npc_type end ------------------------------------------------
----------------------------------------------------------------------------------------------------