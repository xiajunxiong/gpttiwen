
LunJianSchedulePanel = LunJianSchedulePanel or DeclareMono("LunJianSchedulePanel", UIWFlushPanel)
--WinRewardLbl========Text
--JoinRewardItems==========UIWidgetList[UIWItemCell]
--ScheduleList=============UIWidgetList[LunJianScheduleItem]
--WinRewardLeft============GameObject
--WinRewardRight===========GameObject
--WinRewardBoxs============LunJianScheduleRewardItem[]
--JoinRewardRP=============UIWSubstitute[UIWRedPoint2]
local page_item_count = 3
function LunJianSchedulePanel:LunJianSchedulePanel()
    self.data_cares = {
		{data = LunJianData.Instance:MyInfo(), func = self.FlushSchedlueList,keys = {"schedule_list"},init_call = false},
		{data = LunJianData.Instance:MyInfo(), func = self.flushWinReward,keys = {"win_box_reward_flag"}},
		{data = LunJianData.Instance:MyInfo(), func = self.FlushDailyRewardRedPoint,keys = {"daily_reward_fetch_flag"}},
    }

    local first_show_reward_idx = 1
    local win_reward_data = LunJianData.WinRewardCfg()
    local total_reward_count = #win_reward_data
    for i=1,total_reward_count do
        if LunJianData.Instance:WinRewardGetState(first_show_reward_idx) ~= LunJianData.RewardState.Getted then
            break
        else
            first_show_reward_idx = first_show_reward_idx + 1
        end
    end
    if first_show_reward_idx > total_reward_count then
        first_show_reward_idx = total_reward_count
    end

    self.win_reward_page = math.floor((first_show_reward_idx - 1)/page_item_count + 1)
    -- self.WinRewardList:SetData(win_reward_data)

    local day_reward_item_data = LunJianData.OtherCfg().day_reward
    local day_reward_items = {}
    for _,v in pairs(day_reward_item_data) do
        table.insert(day_reward_items,Item.Create(v))
    end
    self.JoinRewardItems:SetData(day_reward_items)
    -- self:flushWinReward()
    UH.SetText(self.WinRewardLbl,string.format(Language.LunJian.ScheduleWinNum,LunJianData.Instance:MyWinNum()))

    if not LunJianData.Instance:HasScheduleList() then
        LunJianCtrl.Instance:ReqSchedule()
    else
        self:FlushSchedlueList()
    end
end

function LunJianSchedulePanel:FlushDailyRewardRedPoint()
    self.JoinRewardRP:SetNum(LunJianData.Instance:DailyRewardFetchFlag() == LunJianData.RewardState.GetAble and 1 or 0)
end

function LunJianSchedulePanel:maxPage()
    local re = math.floor((#LunJianData.WinRewardCfg() - 1)/page_item_count + 1)
    return re
end

function LunJianSchedulePanel:FlushSchedlueList()
    local sch = LunJianData.Instance:ScheduleList()
    if sch == nil then
        return
    end
    self.ScheduleList:SetData(sch)
end

function LunJianSchedulePanel:flushWinReward()
    self.WinRewardLeft:SetActive(self.win_reward_page > 1)
    self.WinRewardRight:SetActive(self.win_reward_page < self:maxPage())
    local rewards_cfg = LunJianData.WinRewardCfg()
    for i=1,page_item_count do
        local data = rewards_cfg[(self.win_reward_page-1) * page_item_count + i]
        if data == nil then
            self.WinRewardBoxs[i]:SetObjActive(false)
        else
            self.WinRewardBoxs[i]:SetObjActive(true)
            self.WinRewardBoxs[i]:SetData(data)
        end
    end

    -- local cur_win_reward_getted_num = LunJianData.Instance:WinRewardGettedCount()
    -- local win_reward_data = LunJianData.WinRewardCfg()

end
function LunJianSchedulePanel:OnWinRewardLeftClick()
    self.win_reward_page = self.win_reward_page - 1
    self:flushWinReward()
end

function LunJianSchedulePanel:OnWinRewardRightClick()
    self.win_reward_page = self.win_reward_page + 1
    self:flushWinReward()
end

function LunJianSchedulePanel:IsLoaded()
    return self.ScheduleList:IsLoaded()
end

function CellClicks.BagGridNoButtonLunJian(cell)
    if LunJianData.Instance:DailyRewardFetchFlag() == LunJianData.RewardState.GetAble then
        LunJianCtrl.Instance:ReqFetchDailyReward()
    else
        CellClicks.BagGridNoButton(cell)
    end
end



----------------------------------------------------------
-- {
--     plat_type,
--     uid,
--     name,
--     level,
--     avatar_type,
--     headshot_id,
--     profession,
--     guild_name,
--     rank,
--     round_idx,
--     result,
-- }
LunJianScheduleItem = LunJianScheduleItem or DeclareMono("LunJianScheduleItem", UIWidgetBaseItem)
--TimeTxt====Text
--HeadIcon===UIWSubstitute[UIWAvatarCell]
--Name=======Text
--Prof=======Text
--Guild======Text
--Rank=======Text
--RetFlag====UIImageSpriteSet
--TodayFlag==GameObject

local ret_sp = {
    [0] = "",
    [1] = "_LocSheng",
    [2] = "_LocBai",
    [3] = "_LocPing",
}
function LunJianScheduleItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local idx = data.round_idx or 0
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Prof,string.format(Language.LunJian.ScheduleProf,RolePromoteData.GetProfName(data.profession)))
    UH.SetText(self.Guild,string.format(Language.LunJian.ScheduleGuild,
        (StringIsEmpty(data.guild_name) and Language.LunJian.ScheduleGuildNone) or data.guild_name))
    if data.rank <= 0 then
        UH.SetText(self.Rank,Language.LunJian.ScheduleRankNone)
    else
        UH.SetText(self.Rank,string.format(Language.LunJian.ScheduleRank,data.rank))
    end
    local month = os.date("%m")
    local day = LunJianData.BATTLE_BEGIN_DAY + idx - 1
    UH.SetText(self.TimeTxt,string.format(Language.LunJian.ScheduleTime,month,day,LunJianData.BATTLE_HOUR))
    local today = tonumber(os.date("%d"))
    if today == day then
        self.ParList:AutoSlide(self.data,true)
        self.TodayFlag:SetActive(true)
    else
        self.TodayFlag:SetActive(false)
    end
    local name_idx_offset = 0
    local max_day = LunJianData.BATTLE_BEGIN_DAY + LunJianData.SCHEDULE_COUNT - 1
    if max_day - today <= 1 then
        name_idx_offset = 4
    else
        name_idx_offset = 2
    end
    self.gameObject.name = string.format("item (%d)",day - today + name_idx_offset)
    local ret_spname = ret_sp[data.result]
    if StringIsEmpty(ret_spname) then
        self.RetFlag:SetObjActive(false)
    else
        self.RetFlag:SetObjActive(true)
        UH.SpriteName(self.RetFlag,ret_spname)
    end
    self.HeadIcon:SetData({
        avatar_type   = data.avatar_type,
        avatar_id     = data.headshot_id,
    -- avatar_quality= 头像品质 飞升1 ~ 2
    -- top_level     = 巅峰头像框
        avatar_level  = data.level,
    -- image_type    = 头像背景类型 可不传  
    })
end

function LunJianScheduleItem:OnClickDetails()
    if self.data.plat_type == 0 then
        PublicPopupCtrl.Instance:Center(Language.ItemInfo.RoleInfoDesc.RobotInfoHint)
    else
        RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.uid)
        RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.uid)
    end
end

----------------------------------------------------------

LunJianScheduleRewardItem = LunJianScheduleRewardItem or DeclareMono("LunJianScheduleRewardItem", UIWidgetBaseItem)
--WinCountTxt======Text
--BoxObj===========GameObject
--BoxOpenedObj=====GameObject
--CanGetEff========GameObject
function LunJianScheduleRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.WinCountTxt,self.data.box_names)
    local idx = self.data.sn or 0
    local reward_st = LunJianData.Instance:WinRewardGetState(idx)
    self.BoxObj:SetActive(reward_st <= LunJianData.RewardState.GetAble)
    self.BoxOpenedObj:SetActive(reward_st == LunJianData.RewardState.Getted)
    self.CanGetEff:SetActive(reward_st == LunJianData.RewardState.GetAble)
    
    -- self.gameObject.name = string.format("item (%d)",idx)
end

function LunJianScheduleRewardItem:OnClickReward()
    local idx = self.data.sn or 0
    local reward_st = LunJianData.Instance:WinRewardGetState(idx)
    if reward_st == LunJianData.RewardState.GetAble then
        LunJianCtrl.Instance:ReqGetWinReward(idx)
    else
        local obj_pos = self.gameObject:GetPosition()
        local p = Vector3.New(obj_pos.x/self.gameObject.transform.lossyScale.x,obj_pos.y/self.gameObject.transform.lossyScale.y,0)
        p.y  = p.y + 160
        ViewMgr:OpenView(SilkRoadPreview,{pos = p,reward_list = DataHelper.FormatItemList(self.data.win_times)})
    end
end


