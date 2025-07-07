ConcentricKnotData = ConcentricKnotData or BaseClass()
function ConcentricKnotData:__init()
    if ConcentricKnotData.Instance ~= nil then
        Debuger.LogError("[ConcentricKnotData] attempt to create singleton twice!")
        return
    end
    ConcentricKnotData.Instance = self

    self.act_info = SmartData.New({day_count = 0})
    self.game_info = SmartData.New({cards_group = 1, begin_time = 0, end_time = 0, is_finish = -1})
    self.card_succ = SmartData.New({num = 0, camp_list = {}})
    self.card_change = SmartData.New({camp = 0, index = 0})
    self.team_info = SmartData.New({team_list = {}})
end

function ConcentricKnotData:__delete()
    ConcentricKnotData.Instance = nil
end

--活动是否开启
function ConcentricKnotData:IsActOpen()
    return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.ConcentricKnot)
end

--活动次数
function ConcentricKnotData:GetRemainTimes()
    return self:GetMaxTimes() - self.act_info.day_count
end
function ConcentricKnotData:GetMaxTimes()
    return 1
end

--基础配置
function ConcentricKnotData:Config()
    return Config.concentric_knot_auto
end
function ConcentricKnotData:OtherConfig()
    return self:Config().other[1]
end
function ConcentricKnotData:GetNpcSeq()
    return self:OtherConfig().npc_seq
end

--准备时间 游戏时间
function ConcentricKnotData:RedayTime()
    return 5
end
function ConcentricKnotData:GameTime()
    return self.game_info.end_time - self.game_info.begin_time
end

--随机卡组
function ConcentricKnotData:RandomCardsGroup()
    return GameMath.Ramdon(1, #self:Config().cards_group)
end
function ConcentricKnotData:GetShowList()
    local cards_group = self.game_info.cards_group or 1
    local cards_list = self:Config().cards_group[cards_group]
    if cards_list.t_list == nil then
        local t = {[1] = {}, [2] = {}}
        for i = 1, 9 do
            t[1][i] = cards_list["left_" .. i]
            t[2][i] = cards_list["right_" .. i]
        end
        cards_list.t_list = t
    end
    if self:GetLeaderUID() == RoleData.Instance:GetRoleId() then
        self.show_list = {[1] = cards_list.t_list[2], [2] = cards_list.t_list[1]}
    else
        self.show_list = {[1] = cards_list.t_list[1], [2] = cards_list.t_list[2]}
    end
    return self.show_list
end

-----队伍信息
function ConcentricKnotData:TeamList()
    return {[246] = 1, [247] = 1, [248] = 1}
end
function ConcentricKnotData:GetStuffList()
    local show_list = {}
    for i = 0, #self:OtherConfig().item_list_first do
        local data = {item = self:OtherConfig().item_list_first[i]}
        data.item.num = 0
        data.target_id = 246 + i
        table.insert(show_list, data)
    end
    return show_list
end

function ConcentricKnotData:IsSelfItem(item_id)
    for i, v in pairs(self:GetStuffList()) do
        if v.item.item_id == item_id then
            return true 
        end
    end
    return false
end

function ConcentricKnotData:SetAgreeNotice(protocol)
    self.team_info.team_list[protocol.uid] = protocol.is_agree
    self.team_info:Notify()
end

function ConcentricKnotData:ClearTeamInfo()
    self.team_info.team_list = {}
end

--获取队长UID
function ConcentricKnotData:GetLeaderUID()
    local leader_info = TeamData.Instance:GetLeaderInfo()
    return leader_info ~= nil and leader_info.uid or nil
end

function ConcentricKnotData:GetIsAgreeByUID(uid)
    return self.team_info.team_list[uid] == 1
end
function ConcentricKnotData:IsAllAgree()
    return table.nums(self.team_info.team_list) == 2
end

-----游戏信息
function ConcentricKnotData:SetStartInfo(protocol)
    self.game_info.cards_group = protocol.cards_group
    self.game_info.begin_time = protocol.begin_time
    self.game_info.end_time = protocol.end_time
    self.cur_cards = {[1] = 0, [2] = 0}
    
    self.anim_cards = {[1] = {}, [2] = {}}
    self.game_anim_list = {[1] = {}, [2] = {}}
    self.card_succ:Set({num = 0})
end

function ConcentricKnotData:OnCardChange(protocol)
    if self.cur_cards == nil then
        return
    end
    local camp = protocol.uid == RoleData.Instance:GetRoleId() and 2 or 1
    if self.cur_cards[camp] == protocol.card_id then
        self:ChangeCard(camp, self.cur_cards[camp])
        self.cur_cards[camp] = 0
    else
        self.card_change:Set({camp = camp, index = protocol.card_id})
        self:CheckCard(camp, protocol.card_id)
    end
end

function ConcentricKnotData:OnGameOver(protocol)
    self.game_info.is_finish = protocol.is_finish
    self.game_info:SetDirty("is_finish")
    
    self.result_info = {is_finish = self.game_info.is_finish}
    self.result_info.has_reward = self:GetRemainTimes() > 0
end
function ConcentricKnotData:GetResultInfo()
    return self.result_info
end

--自己在右，对方在左
function ConcentricKnotData:GetCamp()
    return 2
end

--判断左右卡牌是否相同
function ConcentricKnotData:GetCardId(camp, index)
    return self.show_list[camp][index]
end

function ConcentricKnotData:CheckCard(camp, card_index)
    if self.cur_cards[camp] ~= 0 then --当前的上次展开了，翻回去
        self:ChangeCard(camp, self.cur_cards[camp])
    end
    self.cur_cards[camp] = card_index
    if self.cur_cards[1] == 0 or self.cur_cards[2] == 0 then
        return
    end
    local card_l = self:GetCardId(1, self.cur_cards[1])
    local card_r = self:GetCardId(2, self.cur_cards[2])
    local is_hide = card_l == card_r
    self.anim_cards[1][self.cur_cards[1]] = {other_index = self.cur_cards[2], showed = self:IsAnimOver(1, self.cur_cards[1]), hide = is_hide}
    self.anim_cards[2][self.cur_cards[2]] = {other_index = self.cur_cards[1], showed = self:IsAnimOver(2, self.cur_cards[2]), hide = is_hide}
    self.anim_cards[camp][card_index].showed = false
    self.cur_cards = {[1] = 0, [2] = 0}
end

function ConcentricKnotData:ChangeCard(camp, index, hide)
    local t_table = {num = self.card_succ.num, camp_list = {[camp] = {index = index, hide = hide or false}}}
    if hide then
        t_table.num = self.card_succ.num + 1
    end
    local function func_set()
        self.card_succ:Set(t_table)
    end
    TimeHelper:AddDelayFrameTimer(func_set, 1)
end
function ConcentricKnotData:ShowCardEffect(camp, index)
    local t_table = {num = self.card_succ.num, camp_effect = {[camp] = {index = index}}}
    local function func_set()
        self.card_succ:Set(t_table)
    end
    TimeHelper:AddDelayFrameTimer(func_set, 1)
end

function ConcentricKnotData:CardShow(camp, index, value)
    local data = self.anim_cards[camp][index]
    if data ~= nil and data.hide then
        local other_camp = 3 - camp
        if self.anim_cards[other_camp][data.other_index].showed then
            self:ShowCardEffect(other_camp, data.other_index)
        end
        return true
    end
    return false
end

function ConcentricKnotData:CardShowed(camp, index, value)
    local to_behind, hide = false, false
    local data = self.anim_cards[camp][index]
    if data ~= nil then
        data.showed = true

        local other_camp = 3 - camp
        local other_data = self.anim_cards[other_camp][data.other_index]
        if other_data ~= nil and other_data.showed then
            hide = data.hide
            to_behind = not hide
            self:ChangeCard(other_camp, data.other_index, hide)
            self.anim_cards[camp][index] = nil
            self.anim_cards[other_camp][data.other_index] = nil
        end
    end
    return to_behind, hide
end

function ConcentricKnotData:IsAnimOver(camp, index)
    return self.game_anim_list[camp][index] == true
end
function ConcentricKnotData:SetIsAnimOver(camp, index, is_anim)
    self.game_anim_list[camp][index] = is_anim
    -- if self.anim_cards[camp] ~= nil and self.anim_cards[camp][index] then
    --     self.anim_cards[camp][index].showed = true
    -- end
end

--卡牌间隔时间
function ConcentricKnotData:CanClickCard(card_index)
    if self.anim_cards[2][card_index] then
        return false
    end
    if self.last_click_time == nil or self.last_click_time + 0.4 < TimeHelper.GetServerTime() then
        self.last_click_time = TimeHelper.GetServerTime()
        return true
    end
    return false
end

-----红点
function ConcentricKnotData:GetRemindNum()
    return self:GetRemainTimes() > 0 and 1 or 0
end

function ConcentricKnotData:SetActInfo(protocol)
    self.act_info.day_count = protocol.day_count
    self.act_info:Notify()
end

function ConcentricKnotData:GetMoQiCfg(use_time)
    local cfg = self:Config().moqi_num[use_time]
    if cfg == nil then
        return self:Config().moqi_num[#self:Config().moqi_num]
    end
    return cfg
end

function ConcentricKnotData:GetRewardShow()
    return self:OtherConfig().reward_1
end
