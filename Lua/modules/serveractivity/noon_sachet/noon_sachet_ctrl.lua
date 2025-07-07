VIEW_REQ("modules/serveractivity/noon_sachet/noon_sachet_view")
VIEW_REQ("modules/serveractivity/noon_sachet/noon_reward_view")
NoonSachetCtrl = NoonSachetCtrl or BaseClass(BaseCtrl)
function NoonSachetCtrl:__init()
    if NoonSachetCtrl.Instance ~= nil then
        Debuger.LogError("[NoonSachetCtrl] attempt to create singleton twice!")
        return
    end
    NoonSachetCtrl.Instance = self

    self.data = NoonSachetData.New()
end

function NoonSachetCtrl:__delete()
    if NoonSachetCtrl.Instance == self then
        NoonSachetCtrl.Instance = nil
    end
end

function NoonSachetCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocols()
end 

function NoonSachetCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.DragonBoat.NoonSachet,
        self.data.act_info,
        remind_func
    )
    FunOpen.Instance:SelfCondition(Mod.DragonBoat.NoonSachet,function ()
        return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DUAN_YANG_XIANG_NANG), Language.Activity.NotOpenTip
    end)

    -- BagData.Instance:ListenItems(self.data:ConsumeItem(), function()
    --     self.data.info_data:Notify()
    -- end)
end

function NoonSachetCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end


function NoonSachetCtrl:OnUnloadGameLogic()
    --切换角色操作
end

function NoonSachetCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRADuanYangXiangNang,"OnSCDuanYangXiangNang")
end

function NoonSachetCtrl:OnSCDuanYangXiangNang(protocol)
    -- LogDG("聪哥哥@这是端阳香囊的下发哦？3183",protocol)
    self.data:SetActInfo(protocol)
end

function NoonSachetCtrl:SendInfo()
    -- LogDG("聪哥哥@这是端阳香囊请求信息哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_DUAN_YANG_XIANG_NANG, 0)
end

function NoonSachetCtrl:SendReward(seq)
    -- LogDG("聪哥哥@这是端阳香囊请求领取任务奖励哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_DUAN_YANG_XIANG_NANG, 1,seq)
    self.data:SetMissionMark(true)
end

function NoonSachetCtrl:SendSachet()
    -- LogDG("聪哥哥@这是端阳香囊请求制作香囊哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_DUAN_YANG_XIANG_NANG, 2)
end

NoonSachetData = NoonSachetData or BaseClass()
function NoonSachetData:__init()
    if NoonSachetData.Instance ~= nil then
        Debuger.LogError("[NoonSachetData] attempt to create singleton twice!")
        return
    end
    NoonSachetData.Instance = self
    self.act_info = SmartData.New({flush = false})
    self.config = Config.duanyangxiangnang_auto
end

function NoonSachetData:__delete()
    if NoonSachetData.Instance == self then
        NoonSachetData.Instance = nil
    end
end

function NoonSachetData:SetActInfo(protocol)
    self.act_info.today_task_finish = protocol.today_task_finish
    self.act_info.is_receive_reward = protocol.is_receive_reward
    self.act_info.today_task_count = protocol.today_task_count

    self.act_info.flush = not self.act_info.flush
end

function NoonSachetData:GetMissionList()
    local list = self:RewardShowCfg()

    local fit_list = {}
    for k,v in pairs(list) do
        local vo = {}
        vo.seq = v.seq
        vo.task_type=v.task_type
        vo.parameter = v.parameter
        vo.task_desc = v.task_desc
        vo.jump_interface = v.jump_interface
        vo.reward_item = v.reward_item
        vo.is_receive = self.act_info.is_receive_reward[v.seq] or 0
        vo.is_finish = self.act_info.today_task_finish[v.seq] or 0
        vo.progress = self.act_info.today_task_count[v.seq] or 0
        table.insert(fit_list,vo)
    end 


    table.sort(fit_list, function (a, b)
        local a_weight = a.is_receive*-1000 + a.is_finish*100
        local b_weight = b.is_receive*-1000 + b.is_finish*100

        return a_weight > b_weight
        end)

    return fit_list
end

function NoonSachetData:CheckTaskJump(data)
    return data.jump_interface == 0
end

function NoonSachetData:CheckTaskFinish(data)
    return data.is_finish > 0
end

function NoonSachetData:CheckTaskReceive(data)
    return data.is_receive > 0
end

function NoonSachetData:GetStuffInfo()
    return self.config.make_configuration
end

function NoonSachetData:CheckIsEnough()
    for k,v in pairs(self.config.make_configuration) do 
        if Item.GetNum(v.submit_item_id) < v.submit_num then 
            return false
        end 
    end 
    return true
end

function NoonSachetData:RewardShowCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUAN_YANG_XIANG_NANG, self.config.basis_configuration)
    return cfg
end

function NoonSachetData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DUAN_YANG_XIANG_NANG) then
        return 0
    end

    for k,v in pairs(self.act_info.today_task_finish) do 
        if v == 1 and self.act_info.is_receive_reward[k] == 0 then 
            return 1
        end 
    end 
    -- -- 任务红点
    -- local cfg = self:TaskConfig()
    -- for k, v in pairs(cfg) do
    --     if self:IsCanReceiveTask(v) then
    --         return 1
    --     end
    -- end
    -- return self:LeftRemind()
end

function NoonSachetData:GetNoonPreList()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUAN_YANG_XIANG_NANG, self.config.reward_configuration)
    local result_list = {}
    for k,v in pairs(cfg) do
        local list = BagData.Instance:ItemListBuild(v.reward_item or {})
        for i,j in pairs(list) do 
            table.insert(result_list,j)
        end 
    end 
    return result_list
end 

function NoonSachetData:SetMissionMark(flag)
    self.mission_mark = true
end

function NoonSachetData:GetMissionMark()
    return self.mission_mark
end
