local MatchBtnName = {
    NoMatch = "LvAnNiu_PuTong",
    Matched = "HuangAnNiu_PuTong",
}

AccuSportView = AccuSportView or DeclareMono("AccuSportView",UIWFlushPanel)

function AccuSportView:AccuSportView()
    self.data = TeamSportData.Instance
    self.data_cares = {
        {data = self.data.user_list,func = self.OnFlush},
        {data = self.data.score_data,func = self.SetScore,keys = {"is_change"}},
        {data = self.data.fight_info,func = self.SetFightInfo,keys = {"is_change"}},
        {data = self.data.match_data,func = self.FlushMatch,keys = {"is_match"}},
        {data = self.data.record_time,func = self.FlushMatch,keys = {"time"}},
    }
end

--初始化
function AccuSportView:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:SetCallBack(function() 
        self.data:CancelCountTime()
        SceneCtrl.Instance:RequestLeaveFb()
    end)
    self.Time:CreateActTime(ActType.TeamSport,TimeType.Type_Time_0)      
end

function AccuSportView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht2)
end

function AccuSportView:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(Language.TeamSport.TipString)
end

function AccuSportView:OnClickMatch()
    if self.data.match_data.is_match == 0 then
        TeamSportCtrl.Instance:SendTeamFightMatchReq(TeamSportCtrl.ReqType.Matching)
    else
        TeamSportCtrl.Instance:SendTeamFightMatchReq(TeamSportCtrl.ReqType.CancelMatching)
    end
end

function AccuSportView:OnToggleClick()
    TeamSportCtrl.Instance:SendTeamFightMatchReq(TeamSportCtrl.ReqType[self.Toggle.isOn and "OnlyMyReport" or "BattleReport"])
end

function AccuSportView:OnFlush()
    local user_list = self.data:GetUserList()
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(user_list[i])
    end
end

function AccuSportView:SetScore()
    local data = self.data.score_data.data
    if data.score < 0 then 
        data.score = 0 
    end
    UH.SetText(self.Score,data.score)
    UH.SetText(self.WinCount,Format(Language.TeamSport.WinCount,data.victory_count))
    UH.SetText(self.FightCount,Format(Language.TeamSport.FightCount,data.battle_count))
    UH.SetText(self.StreakCount,Format(Language.TeamSport.WinStreatCount,data.continue_victory_count))
    local count = {data.victory_count,data.battle_count}
    for i=1,self.BoxItem:Length() do
        local config = TeamSportData.Instance:GetBoxRewardConfig(i - 1)
        self.BoxItem[i]:SetData({config = config,count = count[i],reward_list = self:GetRewardData(config), pos = Vector3.New(305 + (i-1) * 50,-110,0)})
    end
end

function AccuSportView:GetRewardData(config)
    local list = {}
    if next(config.reward) ~= nil then
        table.insert(list,Item.New(config.reward))
    end
    table.insert(list,Item.New({item_id = CommonItemId.Coin,num = config.coin}))
    return list
end

function AccuSportView:SetFightInfo()
    local data = self.data.fight_info.data
    self.FightInfoList:SetData(data)
end

function AccuSportView:FlushMatch()
    if self.data.match_data.is_match == 0 then
        self.data.record_time.time = 0
        self.data:CancelCountTime()
        UH.SpriteName(self.MatchImg,MatchBtnName.NoMatch) 
        self.Match.text = Language.TeamSport.StartMatch
        self.Match.color = COLORS.Green1
        self.MatchTime.text = ""
    else
        UH.SpriteName(self.MatchImg,MatchBtnName.Matched) 
        if self.data.record_time.time == 0 then
            self.data:StartCountTime()
        end
        self.Match.text = Language.TeamSport.CancelMatch
        self.MatchTime.text = Language.TeamSport.Matching .. os.date(" %M:%S", self.data.record_time.time)
        self.Match.color = COLORS.Red1
    end
end

TeamSportRoleItem = TeamSportRoleItem or DeclareMono("TeamSportRoleItem")

function TeamSportRoleItem:SetData(data)
    if data then
        UH.SetText(self.Name,data.name)
        UH.SetText(self.Level,data.level .. Language.Common.Level)
        self.Avatar:SetAvatar(data.avatar_type,data.avatar_id,data.avatar_quality)
        UH.SpriteName(self.Prof,RoleData.GetProfSpByProfId(data.prof))
    else
        self.Avatar:Cancel()
    end
    self.Hied:SetActive(data~=nil)
end

TeamSportFightInfoItem = TeamSportFightInfoItem or DeclareMono("TeamSportFightInfoItem",UIWidgetBaseItem)
function TeamSportFightInfoItem:SetData(data)
    self.fight_info.text = Format(Language.TeamSport.FightInfo,data[1],data[2])
end

----------------------------AccuSportBoxItem----------------------------
AccuSportBoxItem = AccuSportBoxItem or DeclareMono("AccuSportBoxItem", UIWidgetBaseItem)

function AccuSportBoxItem:SetData(data)
    self.type = data.config.type or 0
    local fetch_flag = TeamSportData.Instance:GetRewardFetchFlag(self.type) == 1
    local can_fetch = data.count >= (self.type==0 and 1 or 3)

    UH.SetText(self.Name,data.config.box_name)
    self.NotHad:SetActive(not fetch_flag and not can_fetch)
    self.Effect:SetActive(can_fetch and not fetch_flag)

    self.Finish:SetActive(fetch_flag)

    UIWidgetBaseItem.SetData(self, data)
end

function AccuSportBoxItem:OnClickItem()
    if self.Effect.activeSelf then
        TeamSportCtrl.Instance:SendTeamFightMatchReq(6,self.type)
    else
        ViewMgr:OpenView(SilkRoadPreview,{reward_list = self.data.reward_list,pos = self.data.pos})
    end
end

AccuSporPopup = AccuSporPopup or DeclareView("AccuSporPopup", "team_sport/accu_spor_popup")

function AccuSporPopup:AccuSporPopup()
    self.Board:SetData(self:GetType(), "",Vector2.New(664, 516))
end

function AccuSporPopup:LoadCallback(data)
    self.Board:ChangeTitle(data.name)
    self.Grid:SetData(BagData.Instance:GiftItemList(data.item_id))
end

----------------------------AccuSporPopupItem----------------------------
AccuSporPopupItem = AccuSporPopupItem or DeclareMono("AccuSporPopupItem", UIWidgetBaseItem)

function AccuSporPopupItem:SetData(data)
    self.Call:SetData(data)
    UH.SetText(self.Name,data:Name())
end