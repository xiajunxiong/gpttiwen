
BranchMarbasTaskView = BranchMarbasTaskView or DeclareView("BranchMarbasTaskView", TaskConfig.ResPath .. "branch_marbas_task", Mod.BranchMarbasTask.Main)

-- VIEW_DECLARE_LEVEL(BranchSerialTaskView,ViewLevel.L0)

function BranchMarbasTaskView:BranchMarbasTaskView()
end

function BranchMarbasTaskView:LoadCallback()
end

function BranchMarbasTaskView:OnClickClose()
    ViewMgr:CloseView(BranchMarbasTaskView)
end

BranchMarbasTaskViewF = BranchMarbasTaskViewF or DeclareMono("BranchMarbasTaskViewF", UIWFlushPanel)

function BranchMarbasTaskViewF:BranchMarbasTaskViewF()
	self.data = TaskCtrl.Instance:Data()
	-- self.language = Language.HarmonyHeaven.Shop
	self.data:BranchMarbasTaskAutoSel()
end

function BranchMarbasTaskViewF:Awake()
	UIWFlushPanel.Awake(self)

	self:FlushShow()
end

function BranchMarbasTaskViewF:FlushShow()
	local list =self.data:GetBranchTaskChainShow(2)
	for i = 1, self.ShowItem:Length() do
		self.ShowItem[i]:SetData(list[i])
	end
end


BranchMarbasTaskViewItem = DeclareMono("BranchMarbasTaskViewItem", UIWidgetBaseItem)

function BranchMarbasTaskViewItem:BranchMarbasTaskViewItem()
	self.task_data = TaskCtrl.Instance:Data()
	self.language = Language.Task.BranchSerialTask
end

function BranchMarbasTaskViewItem:OnDestroy()
    self:uncareData()
end

function BranchMarbasTaskViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}
	self:careData(self.task_data.branch_marbas_task_sm.info, BindTool.Bind(self.FlushSelect, self))

	if nil == data.chapter_award_show then
		local rewards = {}
		for k, v in pairs(data.chapter_award) do
			table.insert(rewards, Item.Init(BagData.GetNewItemId(v.item_id), v.num, v.is_bind))
		end
		data.chapter_award_show = rewards
	end
	self.TitleNameShow.text = string.format(self.language.TitleNameShow, data.chapter_show, data.chapter_name)
	self.NameShow.text = data.chapter_name
	self.DescShow.text = RichTextHelper.ColorStr("ttttt", COLORSTR.Transparent) .. data.chapter_describe
	self.RewardList:SetData(data.chapter_award_show)

	local task = self.task_data:GetTaskInfoByTaskType(TaskConfig.TaskType.branch_marbas)
    if task then
        local task_id = task.task_id
        local co = Config.tasklist_auto.chain_go_on[task_id]
        if co then
			local chain_chapter = co.chain_chapter
			local is_lock = data.chain_chapter > chain_chapter
			local is_complete = data.chain_chapter < chain_chapter
			local can_accpet = data.chain_chapter == chain_chapter and 1 == co.turn
			local is_ing = data.chain_chapter == chain_chapter and 1 ~= co.turn
			self.StateSp:SetObjActive(not is_lock)
			self.SuoObj:SetActive(is_lock)
			self.CompleteObj:SetActive(is_complete)
			self.LockObj:SetActive(is_lock)
			self.BtnAcceptObj:SetActive(can_accpet)
			self.BtnGoObj:SetActive(is_ing)
			self.BgSp1.SpriteName = is_lock and "ZhangJieDiWeiKaiQi" or "ZhangJieDi"
			self.BgSp2.SpriteName = is_lock and "ZhangJieMingWeiKaiQi" or "ZhangJieMingZiDi"

			if not is_lock then
				self.StateSp.SpriteName = is_complete and "YiWanChengDi" or "KeJieQuDi"
				local state = is_complete and 1 or (can_accpet and 2 or 3)
				self.StateShow.text = RichTextHelper.ColorStr(self.language.StateShow[state], is_complete and COLORSTR.Red5 or COLORSTR.Green6)
			end
			self.red_point = self.red_point or UiRedPoint.New(self.BtnAcceptObj, Vector2.New(77.7,24))
			self.red_point:SetNum(1)
        end
    end
end

function BranchMarbasTaskViewItem:FlushSelect()
	if self.data == nil then return end
	local value = self.data.chain_chapter == self.task_data.branch_marbas_task_sm.info.chain_chapter
	if value then
		UIWidgetBaseItem.SetSelected(self, true)
	else
		UIWidgetBaseItem.SetSelected(self, false)
	end
end

function BranchMarbasTaskViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function BranchMarbasTaskViewItem:OnClickItem(selected)
	local value = false
	if selected then
		self.task_data.branch_marbas_task_sm.info:Set(self.data)
	elseif self.data.chain_chapter == self.task_data.branch_marbas_task_sm.info.chain_chapter then
		self.task_data.branch_marbas_task_sm.info:Set{chain_chapter = 0}
	end

	if value then
		UIWidgetBaseItem.SetSelected(self, true)
	end
end

function BranchMarbasTaskViewItem:OnClickComplete()
	PublicPopupCtrl.Instance:Center(self.language.CompleteTips)
end

function BranchMarbasTaskViewItem:OnClickAccept()
	local task = self.task_data:GetTaskInfoByTaskType(TaskConfig.TaskType.branch_marbas)
    if task then
		task:RequestCommit()
	end
	ViewMgr:CloseView(BranchMarbasTaskView)
end

function BranchMarbasTaskViewItem:OnClickGo()
	local task = self.task_data:GetTaskInfoByTaskType(TaskConfig.TaskType.branch_marbas)
    if task then
		task:GoTask()
	end
	ViewMgr:CloseView(BranchMarbasTaskView)
end

function BranchMarbasTaskViewItem:OnClickLock()
	PublicPopupCtrl.Instance:Center(self.language.LockTips)
end