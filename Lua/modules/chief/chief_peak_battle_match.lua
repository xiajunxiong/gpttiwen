ChiefPeakBattleMatch = ChiefPeakBattleMatch or DeclareView("ChiefPeakBattleMatch", "chief/ChiefPeakBattleMatch")

VIEW_DECLARE_LEVEL(ChiefPeakBattleMatch,ViewLevel.L0)

function ChiefPeakBattleMatch:OpenCallback()
    ChiefCtrl.Instance:SendPeakBattleReq()
    ChiefCtrl.Instance:SendPeakBattleReq(CHIEFPEAKBATTLE_REQ.CAMP_REQ)
    if ChiefPeakBattleData.Instance:IsAbandon() then
        ChiefCtrl.Instance:ChiefPeakBattleSubscribe()
    end
end

function ChiefPeakBattleMatch:OnClickClose()
    ViewMgr:CloseView(ChiefPeakBattleMatch)
end

--===========================ChiefPeakBattleMatchPanel===========================
ChiefPeakBattleMatchPanel = ChiefPeakBattleMatchPanel or DeclareMono("ChiefPeakBattleMatchPanel", UIWFlushPanel)

function ChiefPeakBattleMatchPanel:ChiefPeakBattleMatchPanel()
    self.data = ChiefPeakBattleData.Instance
    self.data_cares = {
        {data = self.data.match_data, func = self.FlushMatchview,keys = {"curr_round"}},
    }
end

function ChiefPeakBattleMatchPanel:FlushMatchview()
    local match_round = self.data:GetCurrRound()
    for round=1,match_round > 3 and 3 or match_round do
        for i,info in ipairs(self.data:GetMatchList(round)) do
            if self["MatchList"..round][i] then
                self["MatchList"..round][i]:SetData(info)
            end
            if round == 3 and self.data:IsFinish() then --设置冠军
                local data = info[1].match_round > info[2].match_round and info[1] or info[2]
                if tonumber(data.role_id) ~= 0 then
                    UH.SetText(self.Winner,data.role_name)
                    self.WinnerObj:SetActive(true)
                end
            end
        end
    end    
    --设置状态文字
    if match_round <= 3 then
        local watch_tip = Language.ChiefPeakBattle.WatchTip
        UH.SetText(self.WatchTip,Format(watch_tip[0],watch_tip[match_round]))
    end
    --设置蒙版
    if match_round > 1 then
        self.Mask:SetParent(self.RoundList[match_round-1])
    end    
    self.WatchObj:SetActive(not self.data:IsFinish())
    --底部文字
    UH.SetText(self.MatchTip,Language.ChiefPeakBattle.MatchTip[self.data:IsFinish() and 2 or 1])
    --设置特效
    self:FlushEffectView()
end

--显示成功特效
function ChiefPeakBattleMatchPanel:FlushEffectView()
    local match_round = self.data:GetCurrRound()
    if match_round == 2 then
        for i=1,8 / math.pow(2,match_round) do
			self["MatchList"..match_round][i]:FlushSuccView()
		end
    end
end

function ChiefPeakBattleMatchPanel:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Language.ChiefPeakBattle.Help)
end

--=================================ChiefPeakBattleMatchItem=================================
ChiefPeakBattleMatchItem = ChiefPeakBattleMatchItem or DeclareMono("ChiefPeakBattleMatchItem", UIWidgetBaseItem)

function ChiefPeakBattleMatchItem:ChiefPeakBattleMatchItem()
    self.data = ChiefPeakBattleData.Instance
end

function ChiefPeakBattleMatchItem:SetData(data)
    local num = 0
    for i,v in ipairs(data) do
        local role_name = v.role_id == 0 and "          --" or v.role_name
        UH.SetText(self.Name[i],Format("<color=#%s>%s</color>",v.color,role_name))
        if v.role_id ~= 0 then
            num = num + 1
            UH.SpriteName(self.Prof[i], ProfSettingB[v.prof])
        end
        self.Prof[i]:SetObjActive(v.role_id ~= 0)
        if self.Round == 3 then --特殊处理
            self.Name[i]:SetObjActive(v.role_id ~= 0)
            local index = self.data:GetMatchIndex(v.role_id)
            local pos = self.Rect[i].localPosition
            pos.y = ChiefConfig.PeakBattlePos[index]
            UH.LocalPos(self.Rect[i],pos)
        end
    end
    local is_round = self.Round == self.data:GetCurrRound()
    and not self.data:IsFinish() and num >= 1
    self.Effect:SetActive(is_round and self.Round ~= 3)
    self.Watch:SetActive(is_round and self.data:IsAbandon())
    self.view_data,self.num = data,num
end

function ChiefPeakBattleMatchItem:OnClickWatch()
    if self.data:GetRoundState() == 0 or self.num == 1 then
        PublicPopupCtrl.Instance:Center(Language.ChiefElection.ErrorLog[9])
        return
    end
    --是否已经打完
    -- local is_flish = false
    -- for i,v in ipairs(self.view_data) do
    --     if self.data:IsWinerFinsh(self.Round,v.role_id)then
    --         is_flish = true
    --     end
    -- end
    -- if is_flish then
    --     PublicPopupCtrl.Instance:Center(Language.ChiefElection.ErrorLog[10])
    --     return
    -- end    
    ViewMgr:OpenView(ChiefWatchView,self.view_data)
end

function ChiefPeakBattleMatchItem:FlushSuccView()
    if self.SuccEffect and self.Line and self.num >= 1 then
        local index = 0
        for i=1, 2 do
            self.Line[i]:SetObjActive(true)
        end
        TimeHelper:AddCountDownTT(function()
            if index < 8 then
                index = index + 1
            end
            for i=1, 2 do
                if self.Line[i] then
                    UH.SpriteName(self.Line[i],self.LineName[i] .. index)
                end
            end
        end,
        function()
            for i=1, 2 do
                self.Line[i]:SetObjActive(false)
            end
        end,0.8,0.1,true)
        self.show_time = TimeHelper:AddDelayTimer(function()
            self.SuccEffect:Play("10010273")
        end, 0.5)
    end
end

function ChiefPeakBattleMatchItem:OnDestroy()
	TimeHelper:CancelTimer(self.show_time)
end