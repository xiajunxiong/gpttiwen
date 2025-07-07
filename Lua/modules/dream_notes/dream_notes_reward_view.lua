DreamNotesRewardView = DreamNotesRewardView or DeclareView("DreamNotesRewardView", "dream_notes/dream_notes_reward")

function DreamNotesRewardView:DreamNotesRewardView()
    self.data = DreamNotesData.Instance
end

function DreamNotesRewardView:LoadCallback(param_t)
    self.cell_list = param_t.reward_list or {}
	local list = FinishData.Instance:GetEleRewards()
	FinishData.Instance:SetEleRewards(nil)
	if list then
		for i, v in ipairs(list) do
			table.insert(self.cell_list, v)
		end
	end
	self.EffectTimer = TimeHelper:AddDelayTimer(function()
		self.RewardAnim:Play()
		self:FlushCallListView()
	end,1)
	if param_t.is_not_first == true then
		UH.SpriteName(self.TitleImg,[[_LocTongGuanJiangLi]])
	end
	self.FirstEffect:SetActive(param_t.is_not_first == nil)
	self.Effect:SetActive(param_t.is_not_first == true)
	self.close_func = param_t.close_func
	self:SetAutoTime(param_t.is_auto_close)
end

function DreamNotesRewardView:SetAutoTime(is_auto_close)
	if is_auto_close == true then
		self.AutoTimer = Invoke(function()
			ViewMgr:CloseView(DreamNotesRewardView)
		end,5)
	end
end

--物品动画逻辑
function DreamNotesRewardView:FlushCallListView()
	if #self.cell_list == 0 then return end
	self.GridList:SetData(self.cell_list)
	self.ShowTimer = TimeHelper:AddDelayTimer(function()
		self:FlushShowView()
	end, 0.7)
end

function DreamNotesRewardView:FlushShowView()
	if self.count == nil then
		self.count = 0
	end
	if self.count < #self.cell_list then
		self:SetItem(self.GridList.item_list[self.count + 1])
		self.count = self.count + 1
		self.ShowItemTimer = TimeHelper:AddDelayTimer(function()
			self:FlushShowView()
        end, 0.1)
	else
		PublicPopupCtrl.Instance:ShowCustom(self.cell_list)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
		self.IsShowBlock = true
	end
end

function DreamNotesRewardView:SetItem(item)
	if item then
		item:SetShow()
	end
end

function DreamNotesRewardView:OnClickBlock()
	if self.IsShowBlock then
		ViewMgr:CloseView(DreamNotesRewardView)
	end
end

function DreamNotesRewardView:CloseCallback()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.EffectTimer)
	TimeHelper:CancelTimer(self.ShowItemTimer)
	TimeHelper:CancelTimer(self.AutoTimer)
	if self.close_func then
		self.close_func()
	end
end