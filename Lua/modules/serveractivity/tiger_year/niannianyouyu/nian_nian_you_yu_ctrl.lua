--年年有鱼 YearYearHaveFish
FILE_REQ("modules/serveractivity/tiger_year/niannianyouyu/nian_nian_you_yu_data")
VIEW_REQ("modules/serveractivity/tiger_year/niannianyouyu/nian_nian_you_yu_view")

NianNianYouYuCtrl = NianNianYouYuCtrl or BaseClass(BaseCtrl)
function NianNianYouYuCtrl:__init()
    if NianNianYouYuCtrl.Instance ~= nil then
        Debuger.LogError("[NianNianYouYuCtrl] attempt to create singleton twice!")
        return
    end
    NianNianYouYuCtrl.Instance = self
    self.data = NianNianYouYuData.New()
    self:RegisterAllProtocols()

    self.custom_npc_list = {}
    self.last_fish_num = nil
    self.need_open_view = false
end

function NianNianYouYuCtrl:__delete()
    NianNianYouYuCtrl.Instance = nil
    Delete(self, "data")
    self:RemoveCustomNpcTalkBtns()
end

function NianNianYouYuCtrl:OnInit()
    ActivityData.Instance:CareEvent(BindTool.Bind1(self.OnActEvent, self))
    Remind.Instance:Register(Mod.TigerYear.NianNianYouYu, self.data.act_info, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU) then
            return 0
        end
    	return self.data:GetRemindNum()
    end)
end

function NianNianYouYuCtrl:OnUnloadGameLogic()
    self.last_fish_num = nil
    self.need_open_view = false
end

function NianNianYouYuCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRATigerFishEveryYearInfo, "OnSCRATigerFishEveryYearInfo")
end

function NianNianYouYuCtrl:OnActEvent()
    if ActivityData.Instance:GetActivityEventType() ~= ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU then
        return
    end
    local act_state = ActivityData.Instance:GetActivityStatus(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU)
    if act_state == ActStatusType.Close then
        self:RemoveCustomNpcTalkBtns()
        self.last_fish_num = nil
        self.need_open_view = false
    end
end

-----协议
function NianNianYouYuCtrl:OnSCRATigerFishEveryYearInfo(protocol)
    self.data:SetActInfo(protocol)
    self:RegisterCustomNpcTalkBtn()
    --飘字
    if self.last_fish_num ~= nil and self.last_fish_num ~= protocol.fish_num then
        local item = Item.Create({item_id = self.data:FishItemId(), num = protocol.fish_num - self.last_fish_num})
        PublicPopupCtrl.Instance:CenterI({item_id = item.item_id, num = item.num, color_str = item:ColorStr(), fly_icon = false})
    end
    self.last_fish_num = protocol.fish_num
    if self.need_open_view then
        self.need_open_view = false
        if self.data:IsAcceptTask() then
            PublicPopupCtrl.Instance:Center(Language.NianNianYouYu.TaskAccepted)
            ActivityRandData.SwitchView(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU, true)
        end
    end
end

function NianNianYouYuCtrl:SendActInfo()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU, 0)
end

--请求接取任务
function NianNianYouYuCtrl:SendAcceptTask()
    self.need_open_view = true
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU, 1)
end

--获取任务奖励 p1->任务seq
function NianNianYouYuCtrl:SendFetchTaskReward(seq)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU, 2, seq)
end

--获取奖励奖励 p1->奖励seq
function NianNianYouYuCtrl:SendFetchRewardReward(seq)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU, 3, seq)
end

-----注册NPC
function NianNianYouYuCtrl:RegisterCustomNpcTalkBtn()
    self:RemoveCustomNpcTalkBtns()
    if self.data:IsAcceptTask() then --接了任务就不需要npc了
        return
    end
    for i, v in pairs(self.data:GetNpcList()) do
        table.insert(self.custom_npc_list, BehaviorData.Instance:CustomNpcTalkBtnsSeq(v.task_npc, BindTool.Bind1(self.SetupBehaviourBtn, self)))
    end
end
function NianNianYouYuCtrl:RemoveCustomNpcTalkBtns()
    for i, v in pairs(self.custom_npc_list) do
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(v)
    end
    self.custom_npc_list = {}
end
function NianNianYouYuCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if obj_vo == nil or not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU) then
        return
    end
    --3）依据用户等级前往不同地点渔夫处开启任务
    local accept_npc = self.data:GetAcceptNpc()
    if accept_npc ~= nil and obj_vo.seq == accept_npc.task_npc and not self.data:IsAcceptTask() then
        local function func_accept()
            self:SendAcceptTask()
        end
        table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(ActivityRandData.GetName(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU), func_accept, true, nil))
    end
end
