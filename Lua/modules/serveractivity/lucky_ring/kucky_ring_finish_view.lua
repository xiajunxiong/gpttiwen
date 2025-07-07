LuckyRingFinish = LuckyRingFinish or DeclareView("LuckyRingFinish", "serveractivity/lucky_ring/lucky_ring_finish")
VIEW_DECLARE_MASK(LuckyRingFinish,ViewMask.BgBlockClose)

function LuckyRingFinish:OnClickClose()
	ViewMgr:CloseView(LuckyRingFinish)
end

LuckyRingFinishPanel = LuckyRingFinishPanel or DeclareMono("LuckyRingFinishPanel", UIWFlushPanel)

function LuckyRingFinishPanel:LuckyRingFinishPanel()
	self.data = LuckyRingData.Instance

    self.param = self.data:FinishData()
end

function LuckyRingFinishPanel:Awake()
	UIWFlushPanel.Awake(self)
	self:FinishPassPanelView() -- 
	self:FlushCallListView() -- 

	self:FlushTimeView()
	self:FlushEffectActive()
end

-- 文本信息·显示
function LuckyRingFinishPanel:FinishPassPanelView()
    self.cell_list = self.param.reward_list or {}

    self.fail:SetActive(self.param.show_type == 5)
    self.success:SetActive(self.param.show_type == 1)

    self.operate_flag = self.param.show_type == 1

    UH.SetText(self.failTitle, Language.LuckyRing.FinishFailTitle)

    self.point_list:SetData(self.param.point_list)
    UH.SetText(self.total_point,Language.LuckyRing.FinishTotal..self.param.total_point)

    -- self.without_tips1:SetActive(self.param.show_type == 5 and self.data.finish_info.need_reward == 0)
    self.without_tips2:SetActive(#self.cell_list == 0)

    self.GridList.gameObject:SetActive( #self.cell_list > 0) 
    self.fail_reward.gameObject:SetActive(self.data.finish_info.need_reward > 0 ) 

	local flag = Config.lasso_auto.other[1].title_score <= self.param.total_point and self.data.finish_info.can_title and self.data.ring_info.flag_title == 1
	-- if flag then 
	-- 	PublicPopupCtrl.Instance:Center(string.format(Language.LuckyRing.TitleTips,Config.lasso_auto.other[1].title_score))
	-- end 
	self.successSpTitle.gameObject:SetActive(flag)
	self.successTitle.gameObject:SetActive(false)
	UH.SetText(self.successSpTitle,string.format(Language.LuckyRing.TitleTips,Config.lasso_auto.other[1].title_score))
	-- UH.SetText(self.successTitle,Language.LuckyRing.FinishWinTitle)
end

function LuckyRingFinishPanel:OnClickConfirm()
    ViewMgr:CloseView(LuckyRingFinish)
end

--设置特效
function LuckyRingFinishPanel:FlushEffectActive()
	local index = self.param.show_type
	if self.Effect[index] then
		self.Effect[index]:SetActive(true)
	end
end

function LuckyRingFinishPanel:FlushCallListView()
	if #self.cell_list == 0 then
		return
	end
	self.GridList:SetData(self.cell_list)
    self.fail_reward:SetData(self.cell_list)
	--0.7秒之后出现物品
	self.ShowTimer = TimeHelper:AddDelayTimer(function()
		self:FlushShowView()
	end, 0.7)
end

--加载奖励列表
function LuckyRingFinishPanel:FlushShowView()
	if self.count == nil then
		self.count = 0
	end
	if self.count < #self.cell_list then
        local operate_list = self.operate_flag and self.GridList or self.fail_reward
		self:SetItem(operate_list.item_list[self.count + 1])
		self.count = self.count + 1
		--物品出现的间隔时间是0.2秒
		self.ShowItemTime = TimeHelper:AddDelayTimer(function()
			self:FlushShowView()
		end, 0.2)
	end
end

function LuckyRingFinishPanel:SetItem(item)
	if item then
		item:SetShow()

		local oper_item = item.data
		if Item.GetShowType(item.data.item_id) == ShowType.Title then 
			return 
		end 
		PublicPopupCtrl.Instance:CenterI({item_id = oper_item.item_id, num = oper_item.num,
		color_str = oper_item:ColorStr(),item = oper_item,icon_id = oper_item:IconId()})
		ChatCtrl.Instance:ChannelChatCustom{item_get = {item = oper_item, num = oper_item.num}}
	end
end

--刷新倒计时
function LuckyRingFinishPanel:FlushTimeView()
    local operate = self.operate_flag and self.ConfirmTime or self.ConfirmTimeF
	operate:SetCallBack(function()
		ViewMgr:CloseView(LuckyRingFinish)
	end)
	operate:TotalTime(5,nil,Language.Finish.Confirm)
end

function LuckyRingFinishPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	self.ConfirmTime:CloseCountDownTime()
    self.ConfirmTimeF:CloseCountDownTime()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
end

LuckyRingFinishPoint = LuckyRingFinishPoint or DeclareMono("LuckyRingFinishPoint", UIWidgetBaseItem)
function LuckyRingFinishPoint:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.star:SetActive(data.light)
    UH.SetText(self.str, data.str)
end
