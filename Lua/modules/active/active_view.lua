ActiveView = ActiveView or DeclareView("ActiveView", "active/active_view",Mod.Activity.Daily)

VIEW_DECLARE_LEVEL(ActiveView,ViewLevel.L1)

function ActiveView:ActiveView()
    RushDungeonCtrl.Instance:SendBraveGroundReq({req_type = BRAVE_GROUND_REQ_TYPE.RANK_INFO})
    -- 播完才能点击，不然动画会有问题
    self.delay_handler = Invoke(function()
        self.ObjBlock:SetActive(false)
        if PrefsInt([[ActiveActivityBackGuide]].. RoleId()) == 0 then
            if RewardData.Instance:GetActivityBackRedPointNum() == 1 then
                PrefsInt([[ActiveActivityBackGuide]].. RoleId(),1)
                GuideManager.Instance:Start(110)
            end
        end
        if GuideData.Instance.open_active_guide.step_id then
            GuideManager.Instance:Start(GuideData.Instance.open_active_guide.step_id)
            UnityPlayerPrefs.SetInt(PrefKeys.ActiveGuide(), 1)
            GuideData.Instance.open_active_guide.step_id = nil
        end
    end, 2)
    GuideManager.Instance:RegisterGetGuideUi("ActiveActivityBackSelect", function()
		if self.Board.Tabbar and self.Board.Tabbar:GetItem(4) then
			return self.Board.Tabbar:GetItem(4)
		end
	end)
end

function ActiveView:LoadCallback(param_t)
    self.Board:SetData(self:GetType())
    self.Board:SetTabbarVisible(true)
    self.Board:SetDataList(self.TabList)
	self.Board:ClickItem(Mod.Activity,param_t)
	self.Currency:DefaultSet()
    local main_task_info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
    if main_task_info and main_task_info:TaskMenuConditionPause() and not UnityPlayerPrefs.HasKey(PrefKeys.ActiveDailyGuide()) then
        -- GuideManager.Instance:Start(128)
        UnityPlayerPrefs.SetInt(PrefKeys.ActiveDailyGuide(), 1)
    end
end

function ActiveView:CloseCallback()
    ActiveData.IsListAnim = nil
    TimeHelper:CancelTimer(self.delay_handler)
end
--===========================ActiveBoard===========================
ActiveBoard = ActiveBoard or DeclareMono("ActiveBoard")

function ActiveBoard:ActiveBoard()
	self.view_type = nil
end

function ActiveBoard:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function ActiveBoard:SetData(view_type)
	self.view_type = view_type
end

function ActiveBoard:SetTabbarVisible(visible)
	self.Tabbar:SetObjActive(visible)
end

function ActiveBoard:OnRelease()
	self.Tabbar:SetObjActive(false)
end

function ActiveBoard:SetDataList(tab,tab_list)
    self.Tabbar:SetData(self:GetTabList(tab,tab_list))
end

function ActiveBoard:ClickItem(modTab,open_param)
	self.Tabbar:ClickItem(self:GetIndex(modTab,open_param or {}))
end

function ActiveBoard:GetIndex(modTab,param_t)
    return FunOpen.Instance:IWantSelect(modTab,self.Tabbar,param_t.open_param)
end

function ActiveBoard:GetTabList(tab)
    local list = {}
    for i,v in ipairs(ActiveConfig.TabList) do
        table.insert(list,{name = Language.Active["Tab" .. i],
        tab_objs = {tab[i]},mod = TabMod(v),open = v})
    end
    return list
end