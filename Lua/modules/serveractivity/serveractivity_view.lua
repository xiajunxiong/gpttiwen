ServerActivityView = ServerActivityView or DeclareView("ServerActivityView", "serveractivity/openserver/server_activity",Mod.ServerActivity.Main)

VIEW_DECLARE_LEVEL(ServerActivityView,ViewLevel.L1)

function ServerActivityView:ServerActivityView()
    self.data = ServerActivityData.Instance
    self.Currency:DefaultSet()
    -- if ActivityData.IsOpen(ACTIVITY_TYPE.KF_COMPETITION_RANK) then
    --     CompetitionRanklCtrl.Instance:SendCompetitionRankType(ServeractivityConfig.CompetitionRank.Type.ArenaRank)
    -- end
end

function ServerActivityView:CloseCallback()
    self.data.open_server_data:Uncare(self.care_handle)
end

function ServerActivityView:LoadCallback(param_t)
    self.care_handle = self.data.open_server_data:Care(BindTool.Bind(self.FlushActivityView,self))
    self:FlushActivityView(param_t)
    self:FlushBtnItem()
end

function ServerActivityView:FlushActivityView(param_t)
    local function GetIndex(list,param_t)
        if param_t == nil then return end
        local key = param_t.mod_key and "mod_key" or "act_type"
        for k,v in pairs(list) do
            if v[key] == param_t[key] then
                return k
            end
        end
    end
    local open_list = self.data:GetOpenServerData()
    for k,v in pairs(open_list) do
        if self[v.view_name] then
            open_list[k].tab_objs = {self[v.view_name].gameObject}
        end
    end
    local data = self.data:GetOpenSelectData()
    self.List:SetDataList(open_list, GetIndex(open_list, param_t or {act_type = data.act_type}))
end

function ServerActivityView:OnClickClose()
    ViewMgr:CloseView(ServerActivityView)
end

function ServerActivityView:FlushBtnItem()
    -- local is_open = ActivityData.IsOpen(ACTIVITY_TYPE.KF_COMPETITION_RANK)
    -- if is_open then
    --     self.BtnItem:SetData(CompetitionRanklData.Instance:GetCompetitionBtnData())
    -- end
    self.BtnItem:SetObjActive(false)
end

--===========================ServerActivityBtn===========================
ServerActivityBtn = ServerActivityBtn or DeclareMono("ServerActivityBtn", UIWidgetBaseItem)

function ServerActivityBtn:SetData(data)
    UH.SetText(self.Name,data.name)
    self.TabActivator.Actives:Clear()
    if data.tab_objs ~= nil then
		for _, item in pairs(data.tab_objs) do
			self.TabActivator.Actives:Add(item)
		end
    end
    if data.mod then
        if self.handle == nil then
            self.handle = Remind.Instance:RegisterGroup(data.mod,function()
                self.RedPos:SetNum(Remind.Instance:GetGroupNum(data.mod))
            end, true)
        end
    else
        self.RedPos:SetNum(0)
	end
    UIWidgetBaseItem.SetData(self, data)
end

function ServerActivityBtn:OnDestroy()
    Remind.Instance:UnRegisterGroup(self.data.mod,self.handle)
end

function ServerActivityBtn:Click()
	if self.ToggleClick then
        self.ToggleClick:Trigger()
	end
end

function ServerActivityBtn:OnClickItem()
    if ActivityRandData.Instance:OnClickHandle(self.data.act_type) then
        return
    end
    ServerActivityData.Instance:SetOpenSelectData(self.data)
end