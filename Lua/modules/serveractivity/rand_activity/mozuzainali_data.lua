
----------------------------MoZuZaiNaLiData----------------------------
MoZuZaiNaLiData = MoZuZaiNaLiData or BaseClass()
function MoZuZaiNaLiData:__init()
	if MoZuZaiNaLiData.Instance ~= nil then
		Debuger.LogError("[MoZuZaiNaLiData] attempt to create singleton twice!")
		return
	end
    MoZuZaiNaLiData.Instance = self

    self:MoZuZaiNaLiConfig()
    self:MoZuZaiNaLiData()

    self.info_sm = SmartData.New{day_times = 0}

    self.login_flag = false
end

function MoZuZaiNaLiData:MoZuZaiNaLiConfig()
    self.config = Config.mozuzainali_auto
    self.config_reward = self.config.reward
    self.config_coordinate = self.config.coordinate
    self.config_basics = self.config.basics[1]

    self.npc_list = {}
    for k, v in ipairs(self.config_coordinate) do
        self.npc_list[v.npc_id] = true
    end
end

function MoZuZaiNaLiData:MoZuZaiNaLiData()
	for npc_id, _ in pairs(self.npc_list) do
        BehaviorData.Instance:CustomNpcTalkBtnsId(npc_id, BindTool.Bind(self.SetupBehaviourBtn, self))
    end
end

function MoZuZaiNaLiData:SetupBehaviourBtn(btn_list, obj_vo)
    if nil == obj_vo then
        return
    end
    if self.npc_list[obj_vo.id] and btn_list[1] then
        local not_finish = self.info_sm.day_times < self:BasicsFrequency()
        local btn = btn_list[1]
        local button =
            NpcDialogBehBtnData.FuncCreate(
                btn.dis_str .. string.format(Language.MoZuZaiNaLi.BtnDisStr, self.info_sm.day_times, self:BasicsFrequency()),
                function()
                    if not_finish then
                        BehaviorCtrl.Exe(btn.beh_id, btn.obj_vo)
                    else
                        PublicPopupCtrl.Instance:Center(PublicPopupCtrl.Instance:Center(Language.MoZuZaiNaLi.TodayFinish))
                    end
                end,
                not_finish, self:GetDetailInfo()
            )
        btn_list[1] = button
    end
end

function MoZuZaiNaLiData:GetDetailInfo()
	local config = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MOZUZAINALI, self.config_reward)
    if nil == config and nil == config[1] then return end
    local rewards = {}
    for i = 0, #config[1].reward_item do
        local reward = config[1].reward_item[i]
        table.insert(rewards, Item.Init(reward.item_id, reward.num, reward.is_bind))
    end
	local tips = string.split(self.config_basics.describe, "|")
	return NpcDialogData.CreateDetailInfo1{tips = tips, rewards = rewards}
end

function MoZuZaiNaLiData:SetRAMoZuZaiNaLiInfo(protocol)
	self.info_sm.day_times = protocol.day_times
end

function MoZuZaiNaLiData:BasicsSceneId()
	return self.config_basics.scene_id
end

function MoZuZaiNaLiData:BasicsFrequency()
	return self.config_basics.frequency
end