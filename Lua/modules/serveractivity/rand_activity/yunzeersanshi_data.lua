
----------------------------MoZuZaiNaLiData----------------------------
YunZeErSanShiData = YunZeErSanShiData or BaseClass()
function YunZeErSanShiData:__init()
	if YunZeErSanShiData.Instance ~= nil then
		Debuger.LogError("[YunZeErSanShiData] attempt to create singleton twice!")
		return
	end
    YunZeErSanShiData.Instance = self

    self:YunZeErSanShiConfig()
    self:YunZeErSanShiData()

    self.info_sm = SmartData.New({
		info = SmartData.New{
            times = 0,
            is_turntable_reward = 0,
            task_id = 0,
        },
		draw = SmartData.New{
            seq = 0,
        },
    })
    
    self.login_flag = false
end

function YunZeErSanShiData:YunZeErSanShiConfig()
    self.config = Config.yunzeersanshi_auto
    self.config_basics = self.config.basics
    self.config_reward = self.config.reward
    self.config_reward_turntable = self.config.reward_turntable
    self.config_other = self.config.other[1]

    -- self.npc_list = {}
    -- for k, v in ipairs(self.config_coordinate) do
    --     self.npc_list[v.npc_id] = true
    -- end
end

function YunZeErSanShiData:YunZeErSanShiData()
	-- for npc_id, _ in pairs(self.npc_list) do
    --     BehaviorData.Instance:CustomNpcTalkBtnsId(npc_id, BindTool.Bind(self.SetupBehaviourBtn, self))
    -- end
end

function YunZeErSanShiData:OnUnloadGameLogic()
	if nil ~= self.task_btn_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.task_btn_handle)
        self.task_btn_handle = nil
    end
    self.pre_info = nil
end

function YunZeErSanShiData:SetRAYunZeErSanShiInfo(protocol)
    local info = protocol.info
    self.info_sm.info:Set(info)
    self.need_open_view = false
    if nil ~= self.pre_info then
        if 0 == self.pre_info.is_turntable_reward and 1 == info.is_turntable_reward then
            if ViewMgr:IsOpen(NpcDialogView) then
                self.need_open_view = true
            else
                ViewMgr:OpenView(YunZeErSanShiView)
            end
        end
        -- if 1 == info.times - self.pre_info.times then
        --     self:AutoToNpc()
        -- end
    end
    self.pre_info = info
    if nil ~= self.task_btn_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.task_btn_handle)
        self.task_btn_handle = nil
    end
    if info.task_id > 0 then
        local task = self.config_basics[info.task_id]
        if nil == task then return end
        local npc_seq = task.npc_id
        self.task_btn_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(npc_seq, function(btn_list, obj_vo)
			if obj_vo and npc_seq == (obj_vo.seq or -1) then
                local activity_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_YUN_ZE_ER_SAN_SHI)
                local status = activity_status and activity_status.status or ActStatusType.Close
                if ActStatusType.Open == status then
                    table.insert(
                        btn_list,
                        NpcDialogBehBtnData.FuncCreate(
                            string.format(Language.YunZeErSanShi.TaskBtnShow.Name, info.times, self:ConfigFrequency()),
                            BindTool.Bind(self.doTaskAnswer, self, task),
                            false,
                            self:GetDetailInfo(task.item_id)
                        )
                    )
                end
			end
		end)
    end
end

function YunZeErSanShiData:doTaskAnswer(task)
    local npc_co = Cfg.NpcBySeq(task.npc_id)
    local talker = npc_co and npc_co.id or 0
    NpcDialogCtrl.Instance:NpcDialogView(
        {
            talk_list = {
                {
                    talker = talker,
                    talk_text = task.problem
                },
            },
            beh_btn_list = self:doTaskAnswerBtn(task, talker)
        }
    )
end

function YunZeErSanShiData:doTaskAnswerBtn(task, talker)
    local beh_btn_list = {}
    local btn1 = NpcDialogBehBtnData.FuncCreate(
        task.answer1,
        BindTool.Bind(self.doTaskAnswerBtnEnd, self, task, talker, 1 == task.correct),
        false
    )
    local btn2 = NpcDialogBehBtnData.FuncCreate(
        task.answer2,
        BindTool.Bind(self.doTaskAnswerBtnEnd, self, task, talker, 2 == task.correct),
        false
    )
    table.insert(beh_btn_list, btn1)
    table.insert(beh_btn_list, btn2)
    return beh_btn_list
end

function YunZeErSanShiData:doTaskAnswerBtnEnd(task, talker, is_true)
    if is_true then
        ServerActivityCtrl.Instance:SendActivityYunZeErSanShiReqAnswer()
    end
    NpcDialogCtrl.Instance:NpcDialogView(
        {
            talk_list = {
                {
                    talker = talker,
                    talk_text = is_true and task.tips1 or task.tips2
                },
            },
        },
        is_true and function ()
            if self.need_open_view then
                self.need_open_view = false
                ViewMgr:OpenView(YunZeErSanShiView)
            else
                self:AutoToNpc()
            end
        end or nil
    )
end

function YunZeErSanShiData:GetDetailInfo(item_id)
	local config = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YUN_ZE_ER_SAN_SHI, self.config_reward)
    if nil == config then return end
    local reward_item
    for _, v in ipairs(config) do
        if v.item_id == item_id then
            reward_item = v.reward_item
            break
        end
    end
    if nil == reward_item then return end
    local rewards = {}
    for i = 0, #reward_item do
        local reward = reward_item[i]
        table.insert(rewards, Item.Init(reward.item_id, reward.num, reward.is_bind))
    end
	-- local tips = string.split(self.config_basics.describe, "|")
	local tips = string.split(self.config_other.describe, "|")
	return NpcDialogData.CreateDetailInfo1{tips = tips, rewards = rewards}
end

function YunZeErSanShiData:SetRAYunZeErSanShiTurntableInfo(protocol)
	self.info_sm.draw:Set(protocol.info)
end

function YunZeErSanShiData:GetAnswerTaskId()
   return self.info_sm.info.task_id
end

function YunZeErSanShiData:ConfigFrequency()
	return self.config_other.frequency
end

function YunZeErSanShiData:ConfigFrequencyTotal()
	return self.config_other.frequency_total
end

function YunZeErSanShiData:GetYunZeErSanShiRewardList()
	local list = ServerActivityData.Instance:GetActivityOpenCfg(nil, nil, self.config_reward_turntable)
	return list
end

function YunZeErSanShiData:AutoToNpc()
    local info = self.info_sm.info
    local task = self.config_basics[info.task_id]
    if nil == task then return end
    local npc_seq = task.npc_id
    SceneLogic.Instance:AutoToNpc(npc_seq, nil, nil, true)
end

