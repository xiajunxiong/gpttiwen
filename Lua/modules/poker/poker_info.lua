--===========================翻牌使用===========================
--1.翻牌协议 参考2475协议
--2.协议下发 打开翻牌 PokerCtrl.Instance:OnOpenPokerView(翻牌列表mypoker,type)
--3.协议转发 队伍中替换翻牌信息 PokerData.Instance:GetMypokerByrIndex(info,protocol)
--4.定义 PokerInfo 类 传other_config 及 smart_data （参考WildBossPoker）

PokerInfo = PokerInfo or BaseClass()

PokerInfo.Effect = {SR = "10010868",SSR = "10010869"}

function PokerInfo.Create(poker_type)
    if poker_type == POKER_REQ_OPER_TYPE.TYPE_MI_YI then
        return MiYiPokerInfo.New(poker_type)
    elseif poker_type == POKER_REQ_OPER_TYPE.TYPE_WILD_BOSS then
        return WildBossPoker.New(poker_type)
    elseif poker_type == POKER_REQ_OPER_TYPE.TYPE_SHXY_FB then
        return HillSeaPoker.New(poker_type)
    elseif poker_type == POKER_REQ_OPER_TYPE.CRYSTAL then
        return CrystalPoker.New(poker_type)
    elseif poker_type == POKER_REQ_OPER_TYPE.CRYSTAL_SWEEP then
        return CrystalSweepPoker.New(poker_type)
    elseif poker_type == POKER_REQ_OPER_TYPE.PET_GOD then
        return PetGodPoker.New(poker_type)
    elseif poker_type == POKER_REQ_OPER_TYPE.PET_GOD_SWEEP then
        return PetGodSweepPoker.New(poker_type)
    elseif poker_type == POKER_REQ_OPER_TYPE.GHOST_NIGHT then
        return GhostNightPoker.New(poker_type)
    elseif poker_type == POKER_REQ_OPER_TYPE.NORMAL_FALSE then
        return NormalFalsePoker.New(poker_type)
    end
    return nil
end

function PokerInfo:__init(poker_type)
    self.poker_type = poker_type
    self.smart_data = PokerData.Instance:GetPokerInfo() --数据监听
    self.time_close = 3 --翻牌自动关闭时间
end

function PokerInfo:__delete()
    self.smart_data:Set({})
end

function PokerInfo:GetShowList()
    local mypoker = self:GetMyPoker()
    if mypoker == nil then
        return {}
    end
    local show_list = {}
    local t1, t2 = {}, {}
    for i = 1, SIDE_MAX_ROLE_NUM do
        local info = self:Conver(mypoker[i])
        if info.index ~= -1 and t1[info.index + 1] == nil then
            t1[info.index + 1] = info
        else
            table.insert(t2, info)
        end
    end
    for i = 1, SIDE_MAX_ROLE_NUM do
        show_list[i] = t1[i] or table.remove(t2, 1)
    end
    return show_list
end
function PokerInfo:SmartData()
    return self.smart_data
end

function PokerInfo:GetTimeClose()
    return self.config and self.config.close_countdown or self.time_close
end

--计时结束时间
function PokerInfo:GetTimeChoose()
    return self.smart_data.timeout ~= nil and self.smart_data.timeout - TimeCtrl.Instance:GetServerTime() or 10
end

function PokerInfo:EffectId(item_id)
    if self:IsSR(item_id) then
        return PokerInfo.Effect.SR
    end
    if self:IsSSR(item_id) then
        return PokerInfo.Effect.SSR
    end
end

function PokerInfo:IsSR(id)
    if self.special_reward == nil then
        self.special_reward = DataHelper.SplitList(self.config["special_reward_1"],":")
    end
    for k, v in pairs(self.special_reward) do
        if v == id then
            return true
        end
    end
    return false
end

function PokerInfo:IsSSR(id)
    if self.special_reward2 == nil then
        self.special_reward2 = DataHelper.SplitList(self.config["special_reward_2"],":")
    end
    for k, v in pairs(self.special_reward2) do
        if v == id then
            return true
        end
    end
    return false
end

function PokerInfo:Conver(info)
    local t = {}
    t.index = info.index
    t.role_id = info.role_uid
    t.role_name = info.role_name
    t.item = info.item_info
    return t
end

function PokerInfo:GetMyPoker()
    return self.smart_data.mypoker
end

function PokerInfo:GetGroupId()
    return self.smart_data.draw_group_id
end

function PokerInfo:CloseCall()
end
----------密医/试炼之地/远征魔冢
MiYiPokerInfo = MiYiPokerInfo or BaseClass(PokerInfo)
function MiYiPokerInfo:__init(poker_type)
    self.config = Config.dexter_auto.other[1]
    self.smart_data = ConspiracyData.Instance:GetFinishReward()
end

function MiYiPokerInfo:Conver(info)
    local t = {}
    t.index = info.index
    t.role_id = info.role_uid
    t.role_name = info.role_name
    t.item = info.item_info
    if info.fixed_reward.reward_id ~= 0 then
        t.other_item = {item_id = info.fixed_reward.reward_id, num = info.fixed_reward.num, is_bind = info.fixed_reward.is_bind}
    end
    return t
end
----------水晶副本
CrystalPoker = CrystalPoker or BaseClass(PokerInfo)
function CrystalPoker:__init(poker_type)
    self.config = Config.crystal_scene_auto.other[1]
end
----------水晶副本扫荡
CrystalSweepPoker = CrystalSweepPoker or BaseClass(CrystalPoker)
--野外首领 只需要这样继承就好，不需要全部重写一遍!!!!!!
--init 只需要 other_config 及 smart_data
--不需要在自己的data定义 IsSR IsSSR special_reward 和 special_reward2
--===========================WildBossPoker===========================
WildBossPoker = WildBossPoker or BaseClass(PokerInfo)
function WildBossPoker:__init(poker_type)
    self.config = Config.monster_yewaiboss_auto.other[1]
end
--===========================HillSeaPoker===========================
HillSeaPoker = HillSeaPoker or BaseClass(PokerInfo)
function HillSeaPoker:__init(poker_type)
    self.config = Config.monster_shxyboss_auto.other[1]
end
--===========================PetGodPoker===========================
PetGodPoker = PetGodPoker or BaseClass(PokerInfo)
function PetGodPoker:__init(poker_type)
    self.config = Config.pet_god_scene_auto.other[1]
end

function PetGodPoker:CloseCall()
    SceneCtrl.Instance:RequestLeaveFb()
end
--===========================PetGodSweepPoker===========================
PetGodSweepPoker = PetGodSweepPoker or BaseClass(PokerInfo)
function PetGodSweepPoker:__init(poker_type)
    self.config = Config.pet_god_scene_auto.other[1]
end
--===========================GhostNightPoker===========================
GhostNightPoker = GhostNightPoker or BaseClass(PokerInfo)
function GhostNightPoker:__init(poker_type)
    self.config = Config.ghosts_at_night_auto.other[1]
end

function GhostNightPoker:CloseCall()
    SceneCtrl.Instance:RequestLeaveFb()
end
--假翻牌
NormalFalsePoker = NormalFalsePoker or BaseClass(PokerInfo)
function NormalFalsePoker:__init(poker_type)
    self.config = Config.crystal_scene_auto.other[1]
end
function NormalFalsePoker:GetTimeChoose()
    return 10
end