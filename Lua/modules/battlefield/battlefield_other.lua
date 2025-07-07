--开战
----------------------------BattlefieldCombatView----------------------------
BattlefieldCombatView = BattlefieldCombatView or DeclareView("BattlefieldCombatView", "battlefield/battlefield_combat")

VIEW_DECLARE_MASK(BattlefieldCombatView,ViewMask.BlockClose)

function BattlefieldCombatView:LoadCallback(param_t)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MatchSuccess)
    local param1 = BattlefieldData.Instance:GetRoleData()
    local param2 = param_t.opponent_data or {}
    local ramdon_num = GameMath.Ramdon(1,2)
    local info = ramdon_num == 1 and {param1,param2} or {param2,param1}
    for i=1,self.CellList:Length() do
        if info[i] ~= nil then
            self.CellList[i]:SetData(info[i])
        end
    end
    self.show_time = TimeHelper:AddDelayTimer(function()
		ViewMgr:CloseView(BattlefieldCombatView)
    end, 2)
end

function BattlefieldCombatView:PlayCountDown(end_time)
    local show_num = end_time
    self.count_time = TimeHelper:AddCountDownTT(function()
        if show_num >= 1 then
            self.RoundCount.text = show_num
            self.CountDownAnim:SetTrigger("tri_321")
        end
        show_num = show_num - 1
	end,
    function()
        self.RoundCount.text = ""
        self.CountDownAnim:SetTrigger("tri_go")
        ViewMgr:CloseView(BattlefieldCombatView)
	end,end_time,1,true)
end

function BattlefieldCombatView:CloseCallback()
    TimeHelper:CancelTimer(self.count_time)
end

----------------------------BattlefieldCombatItem----------------------------
BattlefieldCombatItem = BattlefieldCombatItem or DeclareMono("BattlefieldCombatItem", UIWidgetBaseItem)

function BattlefieldCombatItem:SetData(data)
    local top_race = BattlefieldData.Instance:GetTopRace()
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Rank,top_race == 0 and "" or data.rank_name)
    UH.SetAvatar(self.Head,data.avatar_type,data.avatar_id,data.avatar_quality)
end

----------------------------BattlefieldFunOpenView----------------------------
BattlefieldFunOpenView = BattlefieldFunOpenView or DeclareView("BattlefieldFunOpenView", "battlefield/battlefield_funopen",Mod.FunOpenNotice.Battlefield)
VIEW_DECLARE_LEVEL(BattlefieldFunOpenView, ViewLevel.L1)
function BattlefieldFunOpenView:LoadCallback()
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.Battlefield)
    self.Grid:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local notice = GuideData.Instance:NoticeInfo(self.config.notice_id)
    self.BtnInter.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level
    self.notice_id = self.config.notice_id
end

function BattlefieldFunOpenView:OnClickClose()
    ViewMgr:CloseView(BattlefieldFunOpenView)
end

function BattlefieldFunOpenView:OnClickReward()
    if GuideData.Instance:NoticeInfo(self.notice_id) == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
        return
    end
    FunOpen.Instance:SendFuncPreviewClaimReward(self.notice_id)
    ViewMgr:CloseView(BattlefieldFunOpenView)
    GuideData.Instance:CheckNext(self.config.sn)
end

----------------------------BattlefieldGainView----------------------------
BattlefieldGainView = BattlefieldGainView or DeclareView("BattlefieldGainView", "battlefield/battlefield_gain")

VIEW_DECLARE_MASK(BattlefieldGainView,ViewMask.BgBlockClose)

function BattlefieldGainView:BattlefieldGainView()
    self.data = BattlefieldData.Instance
end

function BattlefieldGainView:CloseCallback()
	TimeHelper:CancelTimer(self.show_time)
    TimeHelper:CancelTimer(self.star_time)
    BattlefieldCtrl.Instance:OnToBattlefieldMatch()
end

function BattlefieldGainView:OnClickComfirm()
    ViewMgr:CloseView(BattlefieldGainView)
end

function BattlefieldGainView:FlushRankView(config,num)
    for i=1,self.StarList:Length() do
        self.StarList[i]:SetActive(i <= num)
    end
    self.StarParent:SetActive(config.level ~= 0)
    UH.SpriteName(self.Rank,"Rank" .. config.rank)
    UH.SetText(self.Ranking,config.ranking_list)
    self.Gradient.buttomColor = BattlefieldConfig.BattlefieldRankingColors[config.rank]
end

function BattlefieldGainView:LoadCallback(param_t)
    local old_data = param_t.old_data or {}
    local new_data = param_t.new_data or {}
    self.count = old_data.level
    self.max_count = new_data.level
    self:FlushRankView(old_data,old_data.level)
    if old_data.rank == new_data.rank then
        self:PlayStarAmin()
        self.GainAnim:Play()
        UH.SetText(self.Ranking,new_data.ranking_list)
    else
        self.is_upgrade = true
        self.new_data = new_data
        self.max_count = self.StarList:Length()
        self:PlayStarAmin()
    end
    if param_t.is_top_race then
        self:FlushWingView()
    end
    self.WinDesc:SetObjActive(param_t.is_top_race ~= nil)
end

--播放特效
function BattlefieldGainView:PlayEffect()
    if self.is_upgrade == true then
        self.is_upgrade = nil
        if self.EffectTool.isActiveAndEnabled then
            self.EffectTool:Play(10010894)
        end
        self.count = 0
        self.max_count = self.new_data.level
        self:FlushRankView(self.new_data,self.count)
        self:PlayStarAmin()
        self.GainAnim:Play()
    end
end

--动画逻辑
function BattlefieldGainView:PlayStarAmin()
    if self.count > self.max_count then
        self:PlayEffect()
        return
    end
    if self.count <= self.StarList:Length() then
        if self.StarList[self.count] then
            self.StarList[self.count]:SetActive(true)
        end
		self.count = self.count + 1
		self.star_time = TimeHelper:AddDelayTimer(function()
			self:PlayStarAmin()
        end, 0.2)
	end
end

function BattlefieldGainView:FlushWingView()
    local win_info = self.data:GetRanksInfo()
    local lose_times = win_info.tolal_times - win_info.win_times - win_info.draw_times
    UH.SetText(self.WinDesc,Format(Language.Battlefield.WinDescGainTip,win_info.win_times,lose_times,win_info.draw_times))
end

----------------------------BattlefieldGainItem----------------------------
BattlefieldGainItem = BattlefieldGainItem or DeclareMono("BattlefieldGainItem", UIWidgetBaseItem)

function BattlefieldGainItem:SetActive(flag)
    self.Effect:SetActive(flag)
end

BattlefieldRoleItem = BattlefieldRoleItem or DeclareMono("BattlefieldRoleItem")

function BattlefieldRoleItem:SetData(data)
    if data ~= nil then
        local info = data.info or {}
        local role_info = BattlefieldData.Instance:GetSearchData(info.uid) or {}
        local rank_config = info.rank_config
        if role_info.battlefield_score and role_info.is_top_race == 1 then
            rank_config = BattlefieldData.Instance:GetCurRankConfig(role_info.battlefield_score)
        end
        if BattlefieldData.Instance:GetTopRace() == 1 and info.uid == RoleData.Instance:GetRoleId() then
            rank_config = BattlefieldData.Instance:GetCurRankConfig()
        end
        UH.SetText(self.Name,info.name)
        UH.SpriteName(self.Prof,RoleData.GetProfSpByProfId(info.prof))
        UH.SetText(self.Level,BattlefieldData.QuaName(rank_config))
        UH.SetAvatar(self.Icon,info.appearance.avatar_type,info.appearance.avatar_id,info.appearance.avatar_quality)
        self.Image:SetActive(rank_config ~= nil)
    else
        UH.SetAvatar(self.Icon)
    end
	self.Panel:SetActive(data ~= nil)
	self.ItemTip:SetActive(data == nil)
	self.Captain:SetActive(data ~= nil and data.is_leader)
    UIWidgetBaseItem.SetData(self, data)
end

function BattlefieldRoleItem:OnClickItem()
	BattlefieldCtrl.Instance:OnRoleInfo(self.data)
end

function BattlefieldRoleItem:OnClickInvite()
    TeamData.Instance.is_world_notice = true
	ViewMgr:OpenView(TeamInviteView)
end

function BattlefieldRoleItem:OnDestroy()
    TeamData.Instance.is_world_notice = nil
end