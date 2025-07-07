VIEW_REQ("modules/serveractivity/may_day/vitality_help_others_view")

VitalityHelpOthersCtrl = VitalityHelpOthersCtrl or BaseClass(BaseCtrl)

function VitalityHelpOthersCtrl:__init()
	if VitalityHelpOthersCtrl.Instance ~= nil then
		Debuger.LogError("[VitalityHelpOthersCtrl] attempt to create singleton twice!")
		return
	end
	VitalityHelpOthersCtrl.Instance = self
	self.data = VitalityHelpOthersData.New()
    self:RegisterAllProtocols()
    self.npc_list = {}
end

function VitalityHelpOthersCtrl:__delete()
    self:UnRegisterAllProtocols()
    VitalityHelpOthersCtrl.Instance = nil
end

function VitalityHelpOthersCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function VitalityHelpOthersCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCVitalityHelpOthersInfo, "OnVitalityHelpOthersInfo")
	PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.VitalityHelpOthers,self.data.info_data,function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_VITALITY_HELP_OTHERS) then
            return 0
        end
        if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.VitalityHelpOthers) then
            return 0
        end
        return self.data:GetRemindNum()
    end)
end

function VitalityHelpOthersCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCVitalityHelpOthersInfo)
end

function VitalityHelpOthersCtrl:OnVitalityHelpOthersInfo(protocol)
    self.data:SetVitalityHelpOthersInfo(protocol)
    if protocol.task_id ~= 0 then
        self:CreateGatherNpc()
    else
        -- LogDG("元气助人 任务结束 或 任务ID == 0 不创建采集物========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================")
    end
end

function VitalityHelpOthersCtrl:CreateGatherNpc()
    if not FunOpen.Instance:IsFunOpen(Mod.MayDay.VitalityHelpOthers) then
        return
    end
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_VITALITY_HELP_OTHERS) then
        return
	end
    for k,v in pairs(Config.yuanqizhuren_auto.task) do
        SceneCtrl.Instance:DeleteNpc(v.gather_id)
    end
    local config = self.data:GetInfoConfig()
    if config ~= nil then
        SceneCtrl.Instance:AddNpc(config.gather_id)
        -- LogDG("添加NPC",config.gather_id)
    end
end

function VitalityHelpOthersCtrl:OnNpcDialogView(config)
    local config = config or self.data:GetInfoConfig()
    local name = Cfg.NpcById(config.gather_id,false).name
    if config ~= nil then
        local npc_config = Cfg.NpcBySeq(config.npc_id,false) or Cfg.NpcById(config.npc_id,false) or {}
        NpcDialogCtrl.Instance:NpcDialogView({
            talk_list = {[1]={
                talker = npc_config.id or 0,
                talk_text = config.desc,
            }},
            beh_btn_list = {
                [1] = NpcDialogBehBtnDataFuncExe.New(Format(Language.VitalityHelpOthers.ItemNameBtn,name),function()
                    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_VITALITY_HELP_OTHERS,1,1)
                    SceneCtrl.Instance:DeleteNpc(config.gather_id)
                    -- LogDG("删掉NPC",config.gather_id)
                    self.ShowTimer = Invoke(function()
                        ViewMgr:OpenViewByKey(Mod.MayDay.VitalityHelpOthers)
                    end)
                end,true)
            }
        }, nil, false)
    end
end

--===========================VitalityHelpOthersData===========================
VitalityHelpOthersData = VitalityHelpOthersData or BaseClass()

function VitalityHelpOthersData:__init()
    if VitalityHelpOthersData.Instance ~= nil then
        Debuger.LogError("[VitalityHelpOthersData] attempt to create singleton twice!")
        return
    end
    VitalityHelpOthersData.Instance = self
    self.info_data = SmartData.New({
        times = 0,
        task_id = 1,
        pre_task_id = 1,
        val = false
    })
end

function VitalityHelpOthersData:ClearData()
    self.info_data.times = 0
    self.info_data.task_id = 1
    self.info_data.pre_task_id = 0
end

function VitalityHelpOthersData:SetVitalityHelpOthersInfo(protocol)
    self.info_data.pre_task_id = protocol.pre_task_id
    self.info_data.task_id = protocol.task_id
    self.info_data.times = protocol.times
    self.info_data.val = not self.info_data.val
    MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_VITALITY_HELP_OTHERS, self:GetRemindNum() > 0 and MayDayConfig.MainBtnState.Go or MayDayConfig.MainBtnState.NextDay)
end

function VitalityHelpOthersData:GetTaskId()
    return self.info_data.task_id
end

function VitalityHelpOthersData:GetTimes()
    return self.info_data.times
end

function VitalityHelpOthersData:GetMaxTimes()
    return Config.yuanqizhuren_auto.other[1].frequency
end

function VitalityHelpOthersData:GetInfoConfig(is_finish)
    local config = is_finish and self:GetConfig(self.info_data.pre_task_id) or nil
	return self:GetConfig(self.info_data.task_id) or config
end

function VitalityHelpOthersData:GetConfigGatherId(gather_id)
    for k,v in pairs(Config.yuanqizhuren_auto.task) do
        if v.gather_id == gather_id then
            return v
        end
    end
end

function VitalityHelpOthersData:GetLastConfig()
    return self:GetConfig(self.info_data.pre_task_id)
end

function VitalityHelpOthersData:GetConfig(task_id)
    for k,v in pairs(Config.yuanqizhuren_auto.task) do
        if v.task_id == task_id then
            return v
        end
    end
end

--获取奖励组奖励列表
function VitalityHelpOthersData:GetRewardData(id)
    local config = Config.yuanqizhuren_auto.reward
    for k,v in pairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_VITALITY_HELP_OTHERS,config)) do
        if v.item_id == id then
            return v.reward_item
        end
    end
end

function VitalityHelpOthersData:GetRemindNum()
    return self:GetTimes() >= self:GetMaxTimes() and 0 or 1
end