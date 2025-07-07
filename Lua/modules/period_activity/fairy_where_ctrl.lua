VIEW_REQ("modules/period_activity/fairy_where_view")

FairyWhereCtrl = FairyWhereCtrl or BaseClass(BaseCtrl)

FairyWhereCtrl.GameFuncList = {
    [1] = function()
        ViewMgr:OpenView(PuzzleFairyView)
    end,
    [2] = function()
        FairyWhereCtrl.Instance:OnCoupletView()
    end,
    [3] = function()
        ViewMgr:OpenView(VisionFairyView)
    end,
}

function FairyWhereCtrl:__init()
	if FairyWhereCtrl.Instance ~= nil then
		Debuger.LogError("[FairyWhereCtrl] attempt to create singleton twice!")
		return
	end
	FairyWhereCtrl.Instance = self
	self.data = FairyWhereData.New()
    self:RegisterAllProtocols()
end

function FairyWhereCtrl:__delete()
    self:UnRegisterAllProtocols()
    FairyWhereCtrl.Instance = nil
end

function FairyWhereCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSFairyWhereReq)
    self:RegisterProtocol(SCFairyWhereInfo, "OnFairyWhereInfo")
    self:RegisterProtocol(SCFairyWhereSceneInfo, "OnFairyWhereSceneInfo")
    BehaviorData.Instance:CustomNpcTalkBtnsId(self.data:GetNpcId(), BindTool.Bind(self.SetupBehaviourBtn, self))
    ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.FairyWhere, BindTool.Bind1(self.OnClickActEvent, self))
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.FairyWhere, BindTool.Bind1(self.OnDeleteHandle, self))
    PeriodActivityData.Instance:CustomTimesHandle(Mod.PeriodActivity.FairyWhere, BindTool.Bind1(self.OnTimesHandle, self))
end

function FairyWhereCtrl:OnRemindHandle()
    Remind.Instance:Register(Mod.PeriodActivity.FairyWhere,self.data.info_data,function()
        if not PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.FairyWhere) then
            return 0
        end
        if PeriodActivityData.Instance:GetRemindData(Mod.PeriodActivity.FairyWhere) then
            return 0
        end
        return self.data:GetRemindNum()
    end)
end

function FairyWhereCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSFairyWhereReq)
    self:UnRegisterProtocol(SCFairyWhereInfo)
    self:UnRegisterProtocol(SCFairyWhereSceneInfo)
end

function FairyWhereCtrl:OnSendAllProtocols()
    if PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.FairyWhere) then
        self:SendFairyWhereReq(0)
    end
end

--仙子去哪儿 0 玩家信息请求 1完成游戏请求
function FairyWhereCtrl:SendFairyWhereReq(req_type)
	local protocol = ProtocolPool.Instance:GetProtocol(CSFairyWhereReq)
	protocol.req_type = req_type or 0
	SendProtocol(protocol)
end

function FairyWhereCtrl:OnFairyWhereInfo(protocol)
    if protocol.has_fetch == 1 then
        self:OnDeleteHandle()
    else
        if PrefsInt(PrefKeys.FairyWhereTask()) == 1 and PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.FairyWhere) then
            self:SendFairyWhereReq(2)
        end
    end
    self.data:SetFairyWhereInfo(protocol)
end

function FairyWhereCtrl:OnFairyWhereSceneInfo(protocol)
    self.data:SetFairyWhereSceneInfo(protocol)
    if protocol.scene_id ~= 0 then
        if PrefsInt(PrefKeys.FairyWhereTask()) == 1 and PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.FairyWhere) then
            self:OnTaskHandle()
        end
    else
        self:OnDeleteHandle()
    end
end

function FairyWhereCtrl:OnDeleteHandle()
    TaskData.Instance:TaskInfoDeleteByTaskId(CUSTOM_OBJID.FAIRY_WHERE)
    PrefsInt(PrefKeys.FairyWhereTask(),0)
end

function FairyWhereCtrl:OnClickActEvent()
    if self.data:GethasFetch() == 1 then
        PublicPopupCtrl.Instance:Center(Language.FairyWhere.NotHasTimeTip)
        return
    end
    local data = self.data:GetSceneData()
    if data ~= nil and data.id then
        PublicPopupCtrl.Instance:Center(FairyWhereData.GetOutdoorName(Language.FairyWhere.AutoToNpcTip))
    end
    if TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.FAIRY_WHERE) == nil then
        self:OnTaskHandle()
        TaskData.Instance:TraceTask(TaskConfig.TaskType.FairyWhere, CUSTOM_OBJID.FAIRY_WHERE)
        PrefsInt(PrefKeys.FairyWhereTask(),1)
    end
    TaskData.Instance:TaskIngEffect(CUSTOM_OBJID.FAIRY_WHERE)
    self.data:SetShowData(true)
end

function FairyWhereCtrl:OnTaskHandle()
    TaskCtrl.Instance:CustomTask({
        task_name = Language.FairyWhere.TaskName,
        task_desc = Language.FairyWhere.TaskDesc,
        task_id = CUSTOM_OBJID.FAIRY_WHERE,
        task_type = TaskConfig.TaskType.FairyWhere,
    })
end

function FairyWhereCtrl:OnTimesHandle()
    return self.data:GetMaxTimes() - self.data:GethasFetch(),self.data:GetMaxTimes()
end

function FairyWhereCtrl:SetupBehaviourBtn(btn_list,obj_vo)
    if nil == obj_vo or not PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.FairyWhere) then
        return
    end
    local NpcButtonName = Language.FairyWhere.NpcButtonName
    table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(NpcButtonName[1], BindTool.Bind1(self.OnFairyWhereOpen, self), true))
    table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(NpcButtonName[2], BindTool.Bind1(self.OnFairyWhereHelp, self), false))
    return btn_list
end

function FairyWhereCtrl:OnFairyWhereOpen()
    if not PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.FairyWhere) then
        return
    end
    if self.data:GethasFetch() == 1 then
        PublicPopupCtrl.Instance:Center(Language.FairyWhere.NotHasTimeTip)
        return
    end
    local team_list = TeamCtrl.Instance:Data():GetMemberRoleList()
    if TeamData.Instance:InTeam() and #team_list > 1 then
        PublicPopupCtrl.Instance:Center(Language.FairyWhere.GameRoleNumTip)
        return
    end
    local type = self.data:GetGameType()
    if FairyWhereCtrl.GameFuncList[type] then
        FairyWhereCtrl.GameFuncList[type]()
    end
end

--玩法规则
function FairyWhereCtrl:OnFairyWhereHelp()
    local type = self.data:GetGameType()
    NpcDialogCtrl.Instance:NpcDialogView({
        talk_id = self.data:GetHelpId(type),
        not_block_close_func = true
    },
    function()
        local param_t = NpcDialogData.Instance.cahce_chck_data
        if param_t then
            param_t.need_auto_typing = true
            NpcDialogCtrl.Instance:NpcDialogView(param_t)
        end
    end, true)
end

--结束对话
function FairyWhereCtrl:OnFairyWhereFinish(type)
    NpcDialogCtrl.Instance:NpcDialogView({
        talk_id = self.data:GetFinishId(type)
    },function()
        FairyWhereCtrl.Instance:SendFairyWhereReq(1)
    end, true)
end

--失败对话
function FairyWhereCtrl:OnFairyWhereFail(type)
    NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.data:GetFailId(type)})
end

function FairyWhereCtrl:OnCoupletView()
    ViewMgr:OpenView(CoupletView,{
        img_type = 2,
        config = Config.where_is_the_fairy_auto,
        close_func = function()
            PublicPopupCtrl.Instance:AlertTip(Language.FairyWhere.CoupletCloseTip,function ()
                ViewMgr:CloseView(CoupletView)
                FairyWhereCtrl.Instance:OnFairyWhereFail(2)
            end)
        end,
        send_func = function()
            ViewMgr:CloseView(CoupletView)
            FairyWhereCtrl.Instance:OnFairyWhereFinish(2)
        end,
        now_group = self.data:GetTurnType()
    })
end

function FairyWhereCtrl:OpenView(param1,param2)
    local type = tonumber(param1)
    PrefsInt(PrefKeys.FairyWhereType(),tonumber(param2))
    if FairyWhereCtrl.GameFuncList[type] then
        FairyWhereCtrl.GameFuncList[type]()
    end
end

--=======================FairyWhereData===============================
FairyWhereData = FairyWhereData or BaseClass()

function FairyWhereData:__init()
    if FairyWhereData.Instance ~= nil then
        Debuger.LogError("[FairyWhereData] attempt to create singleton twice!")
        return
    end
    FairyWhereData.Instance = self
    self.info_data = SmartData.New({has_fetch = 0,is_show = false,val = false})
    self.scene_data = {}
end

function FairyWhereData:SetFairyWhereInfo(protocol)
    self.info_data.has_fetch = protocol.has_fetch
    self.info_data.val = not self.info_data.val
end

function FairyWhereData:SetFairyWhereSceneInfo(protocol)
    self.scene_data = protocol.info
end

function FairyWhereData:GetSceneData()
    return self.scene_data
end

--获取当前哪个玩法
function FairyWhereData:GetGameType()
    local curr_time = ActivityData.Instance:GetCurrTime()
    for k,v in pairs(Config.where_is_the_fairy_auto.time_list) do
        if IsRange(curr_time,v.strat_time,v.end_time) then
            return v.type
        end
    end
end

--获取当前轮到第几套类型
function FairyWhereData:GetTurnType()
    local turn_type = PrefsInt(PrefKeys.FairyWhereType())
    local server_day = TimeCtrl.Instance:GetCurOpenServerDay()
    if PrefsInt(PrefKeys.FairyWhere()) ~= server_day then
        if turn_type == 7 then 
            turn_type = 0 
        end
        PrefsInt(PrefKeys.FairyWhereType(),turn_type + 1)
        PrefsInt(PrefKeys.FairyWhere(),server_day)
    end
    return PrefsInt(PrefKeys.FairyWhereType())
end

function FairyWhereData:GetHelpId(type)
    for k,v in pairs(Config.where_is_the_fairy_auto.game_list) do
        if v.game_type == type then
            return v.talk_rule
        end
    end
end

function FairyWhereData:GetFinishId(type)
    for k,v in pairs(Config.where_is_the_fairy_auto.game_list) do
        if v.game_type == type then
            return v.talk_finish
        end
    end
end

function FairyWhereData:GetFailId(type)
    for k,v in pairs(Config.where_is_the_fairy_auto.game_list) do
        if v.game_type == type then
            return v.talk_fail
        end
    end
end

function FairyWhereData:GetNpcId()
    return Config.where_is_the_fairy_auto.play_config[1].npc_id
end

function FairyWhereData:IshasFetch()
    return self.info_data.has_fetch == 1
end

function FairyWhereData:GethasFetch()
    return self.info_data.has_fetch or 0
end

function FairyWhereData:GetMaxTimes()
    return 1
end

function FairyWhereData:SetShowData(is_show)
    self.info_data.is_show = is_show
end

function FairyWhereData:GetShowData()
    return self.info_data.is_show
end

function FairyWhereData:GetRemindNum()
    return self:GethasFetch() == 0 and 1 or 0
end

function FairyWhereData.GetOutdoorName(desc)
    local data = FairyWhereData.Instance:GetSceneData()
    local name = data.name or SceneData.Instance:SceneName()
    local door_name = MapData.GetOutdoorName(data.id or SceneLogic.Instance:SceneID())
    if door_name ~= nil then
        return Format(desc,door_name .. Language.FairyWhere.MapName[1])
    end
    if string.find(name,Language.FairyWhere.MapName[3]) then
        return Format(desc,Language.FairyWhere.MapName[4])
    end
    return Format(desc,name .. Language.FairyWhere.MapName[2])
end

function FairyWhereData:GetStampTime()
    return self.scene_data.stamp_time or 0
end