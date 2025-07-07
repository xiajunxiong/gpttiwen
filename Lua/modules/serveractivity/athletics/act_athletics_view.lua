--新服竞技
ActAthleticsView = ActAthleticsView or DeclareMono("ActAthleticsView",UIWFlushPanel)

function ActAthleticsView:ActAthleticsView()
	self.data = AthleticsData.Instance
	self.history_data = self.data.historyData
	self.role_data = ArenaData.Instance.RoleInfo
	self.data_cares = {
		{data = self.history_data,func = self.FlushHistory, init_call = false},
		{data = self.role_data,func = self.FlushHistory, init_call = false, keys = {"rank"}},
	}
end

function ActAthleticsView:OnDestroy()
	if self.red_point ~= nil then
		self.red_point:DeleteMe()
		self.red_point = nil
	end
	UIWFlushPanel.OnDestroy(self)
end

function ActAthleticsView:Awake()
	UIWFlushPanel.Awake(self)
	self.Time:CloseCountDownTime()
	self.Time:CreateActTime(ACTIVITY_TYPE.KF_ATHLETICS,TimeType.Type_Special_4)
	self.ActTime:CloseCountDownTime()
	self.ActTime:CreateActTime(ACTIVITY_TYPE.KF_ATHLETICS,TimeType.Type_Special_4)
	local reward_show_list = self.data:GetRewardShowList()
	self.ShowList:SetData(DataHelper.FormatItemList(reward_show_list))
	local right_list = self.data:GetSettlementCfg()
	self.RightList:SetData(right_list)
	self:FlushHistory()
end

function ActAthleticsView:FlushHistory()
	local histiry_list = self.data:GetHistoryRewardList()
	self.LeftList:SetData(histiry_list)
	local rank_num = self.data:GetRank() <= 0 and Language.Athletics.NotRank or (self.data:GetRank().."")
	UH.SetText(self.RankNum,rank_num)
end

function ActAthleticsView:OnGotoClick()
	ViewMgr:OpenView(ArenaView)
	ViewMgr:CloseView(ServerActivityView)
end

--==========AthleticsSettlementItem:新服竞技结算奖励item
AthleticsSettlementItem = AthleticsSettlementItem or DeclareMono("AthleticsSettlementItem",UIWidgetBaseItem)
function AthleticsSettlementItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local rank_num = data.ranking + 1
	self.RankOneObj:SetActive(rank_num == 1)
	self.RankOtherObj:SetActive(rank_num ~= 1)
    self.List:SetData(DataHelper.FormatItemList(data.reward_item))
    local last_seq = data.seq <= 0 and 0 or (data.seq - 1)
    local last_data = AthleticsData:GetSettlementBySeq(last_seq)
    if data.ranking - last_data.ranking > 1 then
    	UH.SetText(self.RankNum,string.format(Language.Athletics.Rank1,last_data.ranking+2,rank_num))
    else
    	UH.SetText(self.RankNum,string.format(Language.Athletics.Rank2,rank_num))
    end
end

--==========AthleticsSettlementItem:新服竞技历史奖励item
AthleticsHistoryItem = AthleticsHistoryItem or DeclareMono("AthleticsHistoryItem",UIWidgetBaseItem)
function AthleticsHistoryItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.List:SetData(DataHelper.FormatItemList(data.reward_item))
	UH.SetText(self.Desc,string.format(Language.Athletics.NeedRank,data.historical_ranking))
	self.GetBtnObj:SetActive(data.get_state ~= AthleticsData.REWARD_STATE.NOT)
	self.NotFlagObj:SetActive(data.get_state == AthleticsData.REWARD_STATE.NOT)
	self.Interactor.Interactable = (data.get_state == AthleticsData.REWARD_STATE.CAN)
	if data.get_state == AthleticsData.REWARD_STATE.GETED then
		UH.SetText(self.BtnText,Language.Athletics.GetState2)
	else
		UH.SetText(self.BtnText,Language.Athletics.GetState1)
	end
end

function AthleticsHistoryItem:OnGetClick()
	if LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:Center(Language.Athletics.GetTip)
        return
    end
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_ATHLETICS,1,self.data.seq)
end
