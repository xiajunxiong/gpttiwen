FILE_REQ("modules/period_activity_act/goto_school/goto_school_data")
FILE_REQ("modules/period_activity_act/goto_school/goto_school_logic")

VIEW_REQ("modules/period_activity_act/goto_school/goto_school_game_view")
GotoSchoolCtrl = GotoSchoolCtrl or BaseClass(BaseCtrl)

GotoSchoolCtrl.ReqType = {
    Fight = 0,  -- 打怪， p1怪物组id
    Succ = 1,   -- 护送成功，发奖励
    FightReward = 2, -- 打怪成功奖励
}

GotoSchoolCtrl.Tip = {
    None = 0,
    GetTask = 1,    -- 获得任务提示
    TaskFinish = 2, -- 任务完成提示
    Time = 3,       -- 倒计时提示
}

function GotoSchoolCtrl:__init()
	if GotoSchoolCtrl.Instance ~= nil then
		Debuger.LogError("[GotoSchoolCtrl] attempt to create singleton twice!")
		return
	end
	GotoSchoolCtrl.Instance = self
    self.data = GotoSchoolData.New()
    self.data = GotoSchoolData.Instance
    self.logic = GotoSchoolLogic.New()
    self.logic = GotoSchoolLogic.Instance
    self:RegisterAllProtocols()
end

function GotoSchoolCtrl:__delete()
    GotoSchoolCtrl.Instance = nil
end

function GotoSchoolCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function GotoSchoolCtrl:OnInit()
    SceneData.Instance.base:Care(BindTool.Bind(self.CheckCreateNPC,self), "scene_id")
    -- 活动开启监听
    PeriodActivityData.Instance:CustomCreateHandle(Mod.PeriodActivity.School, function()
        self:CheckCreateNPC()
        return true
    end)
    -- 活动结束监听
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.School, function()
        self:CheckCreateNPC()
        return true
    end)

    Remind.Instance:Register(Mod.PeriodActivity.School, self.data.info_data, function()
        local num = self.data:RemindNum()
        return num
    end)

    -- 战斗结束
    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_GOTO_SCHOOL, function(is_win)
		GotoSchoolLogic.Instance:BattleEnd(is_win)
    end)

    -- npc按钮
    BehaviorData.Instance:CustomNpcTalkBtnsId(self.data:NpcId(), BindTool.Bind(self.SetupBehaviourBtn, self))
    ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.School, BindTool.Bind1(self.OnClickActEvent, self))
    PeriodActivityData.Instance:CustomTimesHandle(Mod.PeriodActivity.School, function()
        return self.data:GetRemainRewardCount(), self.data:RewardCountCfg()
    end)
end

function GotoSchoolCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSGotoSchoolReq)
    self:RegisterProtocol(SCGotoSchoolInfo, "OnSCGotoSchoolInfo")
end

function GotoSchoolCtrl:SendReq(req_type,p1)
    local protocol = GetProtocol(CSGotoSchoolReq)
    protocol.req_type = req_type
    protocol.p1 = p1 or 0
    SendProtocol(protocol)
end

function GotoSchoolCtrl:OnSCGotoSchoolInfo(protocol)
    self.data.info_data.reward_times = protocol.get_reward_times
end

function GotoSchoolCtrl:OnClickActEvent()
    -- 判断是否开启
    if not self.data:IsOpen() then
       PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
       return
   end
   SceneLogic.Instance:AutoToNpc(self.data:NpcId(), nil, false, true)
end

function GotoSchoolCtrl:CheckCreateNPC()
    -- local npc_id = self.data:NpcId()
    -- if self.data:IsOpen() then
    --     SceneCtrl.Instance:AddNpc(npc_id)
    -- else
    --     SceneCtrl.Instance:DeleteNpc(npc_id)
    -- end
end

function GotoSchoolCtrl:SetupBehaviourBtn(btn_list,obj_vo)
    if not obj_vo then
        return
    end
    local npc_id = obj_vo.id
	if npc_id == self.data:NpcId() then
        -- 首先判断活动是否开启 ** 
        if not self.data:IsOpen() then
            return
        end
        -- 玩法入口
        local button = NpcDialogBehBtnData.FuncCreate(
            Language.GotoSchool.NpcBtn1,function()
                
                if TeamData.Instance:InTeam() then
                    local my_info = TeamData.Instance:GetTeamInfo()
                    local member_count = my_info:MemberNum()
                    if member_count > 1 then
                        LogError("member_count",member_count,my_info.member_list)
                        PublicPopupCtrl.Instance:Center("超出活动人数")
                        return
                    end
                end
                local child_cfg = self.data:GetRandChildCfg()    -- 小孩配置
                GotoSchoolLogic.Instance:SetChildCfg(child_cfg)
                local talk_id = child_cfg.talk_id

                Invoke(function ()
                    NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id},function ()
                        -- 对话结束进入场景玩法
                        SceneCtrl.Instance:RequestEnterFb(FB_TYPE.GOTO_SCHOOL)
                        --SceneData.Instance.change_scene_effect.id = 30000000
                    end)
                end,0.1)
            end)
		button:SetIconType(NpcIconType.Act)
        table.insert(btn_list, button)
        
        -- 玩法说明
        local button = NpcDialogBehBtnData.FuncCreate(
            Language.GotoSchool.NpcBtn2,function()
				ViewMgr:OpenView(GotoSchoolDescView)
            end)
        table.insert(btn_list, button)
        local button = NpcDialogBehBtnData.FuncCreate(
            Language.GotoSchool.NpcBtn3,function()
                
            end)
        table.insert(btn_list, button)
    end

end

function GotoSchoolCtrl.TestStart(param)
    GotoSchoolLogic.Instance:Ready()
end

function GotoSchoolCtrl.TestEnd(param)
    SceneCtrl.Instance:RequestLeaveFb()
    GotoSchoolLogic.Instance:Destroy()
end