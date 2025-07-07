--情缘对对碰
FILE_REQ("modules/period_activity_act/lovers_match/lovers_match_data")
VIEW_REQ("modules/period_activity_act/lovers_match/lovers_match_rank_view")
VIEW_REQ("modules/period_activity_act/lovers_match/lovers_match_menu")
VIEW_REQ("modules/period_activity_act/lovers_match/lovers_match_finish_view")
VIEW_REQ("modules/period_activity_act/lovers_match/lovers_match_mask_view")
VIEW_REQ("modules/period_activity_act/lovers_match/lovers_match_alert_view")

local LoversMatchReqType = {
    TYPE_START = 0,         --准备开始
    TYPE_GOTO_COMPARE = 1,   --进行效验
    TYPE_RANK_INFO = 2,     --请求排行榜信息
    TYPE_DAY_COUNT = 3,     --请求每日次数
    TYPE_USE_FORM = 4,      --请求幻化
}

LoversMatchCtrl = LoversMatchCtrl or BaseClass(BaseCtrl)
function LoversMatchCtrl:__init()
    if LoversMatchCtrl.Instance ~= nil then
        Debuger.LogError("[LoversMatchCtrl] attempt to create singleton twice!")
        return
    end
    LoversMatchCtrl.Instance = self
    self.data = LoversMatchData.New()
    self:RegisterAllProtocols()
    self.match_state = 0
    self.match_role = 0
end

function LoversMatchCtrl:__delete()
    LoversMatchCtrl.Instance = nil
    Delete(self, "data")
    EventSys:UnBind(self.care_role_complete)
end

function LoversMatchCtrl:OnInit()
    BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data:GetNpcSeq(), BindTool.Bind(self.SetupBehaviourBtn, self))
    self.care_role_complete = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
    for i, v in pairs(self.data:GetNpcList()) do
        BehaviorData.Instance:CustomNpcTalkBtnsId(v.npc_id, BindTool.Bind(self.SetupBehaviourBtnFB, self))
    end
	EventSys:Bind(GlobalEventKeys.ServerOpenDayChange, BindTool.Bind1(self.OnDayChange, self))
    ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.LoversMatch, BindTool.Bind1(self.OnClickActEvent, self))
    PeriodActivityData.Instance:CustomCreateHandle(Mod.PeriodActivity.LoversMatch, BindTool.Bind1(self.OnActOpen, self))
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.LoversMatch, BindTool.Bind1(self.OnActOver, self))
    PeriodActivityData.Instance:CustomTimesHandle(Mod.PeriodActivity.LoversMatch, BindTool.Bind1(self.TimesStr, self))
    PeriodActivityData.Instance:CustomHelpHandle(Mod.PeriodActivity.LoversMatch, BindTool.Bind1(self.OnClickHelp, self))
    Remind.Instance:Register(Mod.PeriodActivity.LoversMatch, self.data.act_info, function()
        if PeriodActivityData.Instance:GetRemindData(Mod.PeriodActivity.LoversMatch) then
            return 0
        end
        return self.data:IsActOpen() and self.data:GetRemindNum() or 0
    end)
end

function LoversMatchCtrl:OnUnloadGameLogic()
    self:EndCheck()
    self.data:ClearData()
end

function LoversMatchCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCQingYuanDuiDuiPengStartNotice, "OnSCQingYuanDuiDuiPengStartNotice")
    self:RegisterProtocol(SCQingYuanDuiDuiRoleForm, "OnSCQingYuanDuiDuiRoleForm")
    self:RegisterProtocol(SCQingYuanDuiDuiFbInfo, "OnSCQingYuanDuiDuiFbInfo")
    self:RegisterProtocol(SCQingYuanDuiDuiFinish, "OnSCQingYuanDuiDuiFinish")
    self:RegisterProtocol(SCQingYuanDuiDuiRankInfo, "OnSCQingYuanDuiDuiRankInfo")
    self:RegisterProtocol(SCQingYuanDuiDuiDayTimesInfo, "OnSCQingYuanDuiDuiDayTimesInfo")
    self:RegisterProtocol(CSQingYuanDuiDuiPengReq)
end

function LoversMatchCtrl:RoleNoticeComplete()
    Invoke(function()
        self:SendDayCount()
    end, 1)
end
function LoversMatchCtrl:OnDayChange()
    Invoke(function()
        self:SendDayCount()
    end, 1)
end

function LoversMatchCtrl:OnActOpen()
    SceneCtrl.Instance:AddNpc(self.data:GetNpcSeq())
    return true
end
function LoversMatchCtrl:OnActOver()
    SceneCtrl.Instance:DeleteNpc(self.data:GetNpcSeq())
end
function LoversMatchCtrl:TimesStr()
    return self.data:GetRemainTimes(), self.data:OtherConfig().reward_times
end
function LoversMatchCtrl:OnClickHelp()
    PeriodActivityCtrl.Instance:HelpTip(self.data:OtherConfig().dec_id, Mod.PeriodActivity.LoversMatch)
end

--点击活动操作
function LoversMatchCtrl:OnClickActEvent()
    SceneLogic.Instance:AutoToNpc(self.data:GetNpcSeq(), nil, nil, true)
end

--开始
function LoversMatchCtrl:OnSCQingYuanDuiDuiPengStartNotice(protocol)
    self.data:SetFBStateInfo(protocol)
    ViewMgr:OpenView(LoversMatchTopView)
end

--形象改变
function LoversMatchCtrl:OnSCQingYuanDuiDuiRoleForm(protocol)
    self.data:SetRoleForms(protocol)
    MainPlayer:Vo():SetDirty("appearance")
    local function func_check(obj)
        if obj:Vo() and protocol.role_form[obj:Vo().role_id] then
            obj:Vo():SetDirty("appearance")
        end
    end
    SceneCtrl.Instance:Objs():ForeachRole(func_check)
end

--副本信息
function LoversMatchCtrl:OnSCQingYuanDuiDuiFbInfo(protocol)
    self.data:SetFBInfo(protocol)
end

--结算
function LoversMatchCtrl:OnSCQingYuanDuiDuiFinish(protocol)
    self.data:SetResult(protocol)

    -- if protocol.is_finish == 1 then
    --     VIEW_DECLARE_MASK(LoversMatchFinishView, ViewMask.BgBlockClose)
    -- else
    --     VIEW_DECLARE_MASK(LoversMatchFinishView, ViewMask.BgBlock)
    -- end
    ViewMgr:OpenView(LoversMatchFinishView)
    self:SendDayCount()
end

--排行信息
function LoversMatchCtrl:OnSCQingYuanDuiDuiRankInfo(protocol)
    self.data:SetRankInfo(protocol)
end

function LoversMatchCtrl:OnSCQingYuanDuiDuiDayTimesInfo(protocol)
    self.data:SetDayTimes(protocol)
end

function LoversMatchCtrl:SendLoversMatchReq(req_type, param1)
    local protocol = GetProtocol(CSQingYuanDuiDuiPengReq)
    protocol.req_type = req_type
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end

--场景Npc相关
function LoversMatchCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if nil == obj_vo or not self.data:IsActOpen() then
        return
    end
    table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.LoversMatch.NpcBtn3, BindTool.Bind1(self.OnClickRank, self), true, nil))
    table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.LoversMatch.NpcBtn2, BindTool.Bind1(self.OnClickRule, self), false, nil))
    table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.LoversMatch.NpcBtn1, BindTool.Bind1(self.OnClickBegin, self), true, nil))
end

function LoversMatchCtrl:SetupBehaviourBtnFB(btn_list, obj_vo)
    return self:SetupBehaviourBtnFB2(btn_list, obj_vo)
end
function LoversMatchCtrl:SetupBehaviourBtnFB2(btn_list, obj_vo)
    if nil == obj_vo or not self.data:IsActOpen() then
        return
    end
    local npc_cfg = NpcHelper.NpcCo(obj_vo.id)
    if npc_cfg then
        local func_form = BindTool.Bind2(self.SendUseImage, self, obj_vo.obj_id)
        table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.LoversMatch.UseForm .. npc_cfg.name, func_form, true, nil))
    end
end

--点击npc
function LoversMatchCtrl:OnClickBegin()
    if not self.data:IsActOpenTime() then
        local act_cfg = PeriodActivityData.Instance:GetConfig(Mod.PeriodActivity.LoversMatch)
        if act_cfg then
            local s_time = ActivityData.Instance:GetTimeData(act_cfg.start_time)
            local e_time = ActivityData.Instance:GetTimeData(act_cfg.end_time)
            PublicPopupCtrl.Instance:Center(Format(Language.LoversMatch.FBError2, s_time.hour, s_time.min, e_time.hour, e_time.min))
        else
            PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        end
        return
    end
    if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() then
        PublicPopupCtrl.Instance:Center(ErrorText[57])
        return
    end
    local function func_succ()
        SceneCtrl.Instance:RequestEnterFb(FB_TYPE.LOVERS_MATCH)
    end
    local target_id = 243
    if TeamData.Instance:InTeam() then
        if TeamData.Instance:MemberNumRole() > 2 then
            PublicPopupCtrl.Instance:Center(Language.LoversMatch.FBError1)
            return
        elseif TeamData.Instance:MemberNumRole() == 2 then
            if TeamData.Instance:GetTeamInfo().info.team_type ~= target_id then
                local target = TeamData.Instance:GetTeamGoalWithId(target_id)
                TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
            end
            func_succ()
        else
            TeamCtrl.Instance:TeamDetectByTargetId(target_id, func_succ, 2)
        end
    else
        TeamCtrl.Instance:TeamDetectByTargetId(target_id, func_succ, 2)
    end
end
function LoversMatchCtrl:OnClickRule()
    NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.data:OtherConfig().communicate_rule, not_block_close_func = true}, function()
        local param_t = NpcDialogData.Instance.cahce_chck_data
        if param_t then
            NpcDialogCtrl.Instance:NpcDialogView(param_t)
        end
    end, true)
end
function LoversMatchCtrl:OnClickRank()
    ViewMgr:OpenView(LoversMatchRankView)
end

--场景匹配相关
function LoversMatchCtrl:BeginCheck()
    self.data.fb_info.is_init = true
    self.data.fb_info.reward_level = 0
    self.data.fb_info.begin_game_time = 0
    self.data.fb_info.integral = 0
    if self.runner_timer == nil then
        self.runner_timer = Runner.Instance:RepeatCall(BindTool.Bind1(self.CheckIsMatch, self), 0.8, nil)
    end
    ViewMgr:OpenView(LoversMatchMaskView)
    self:CheckShowTips()
end
function LoversMatchCtrl:EndCheck()
    Runner.Instance:CancelRepeat(self.runner_timer)
    self.runner_timer = nil
    --队员回归
    if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() and not TeamData.Instance:IsFollow() then
        TeamCtrl.Instance:SendTemporarilyPartReq()
    end
    local function func_check(obj)
        if self.data.role_form ~= nil and obj:Vo() and self.data.role_form[obj:Vo().role_id] then
            self.data.role_form[obj:Vo().role_id] = nil
            obj:Vo():SetDirty("appearance")
        end
    end
    SceneCtrl.Instance:Objs():ForeachRole(func_check)
    self.data.role_form = nil
end
function LoversMatchCtrl:CheckIsMatch()
    local role_list = {}
    local center_pos = self.data:GetCenterPos()
    local function func_check(obj)
        local obj_pos = obj:GetPos()
        if GameMath.GetDistance(obj_pos.x, obj_pos.z, center_pos.x, center_pos.z) < self.data:GetCenterDis() then
            if self.data:GetRoleFormResId(obj:Vo()) then
                table.insert(role_list, obj)
            end
        end
    end
    func_check(MainPlayer)
    if #role_list >= 1 then
        if self.match_role == 0 then
            PublicPopupCtrl.Instance:Center(Language.LoversMatch.FBError3)
            self.match_role = 1
        end
    else
        self.match_role = 0
    end

    SceneCtrl.Instance:Objs():ForeachRole(func_check)
    if #role_list >= 2 then
        if self.match_state == 0 and TeamData.Instance:IsLeader() then
            self.match_state = 1
            self:SendLoversMatchReq(LoversMatchReqType.TYPE_GOTO_COMPARE)
        end
    else
        self.match_state = 0
    end
end

--请求准备
function LoversMatchCtrl:SendBegin()
    self:SendLoversMatchReq(LoversMatchReqType.TYPE_START)
end

function LoversMatchCtrl:SendRankInfo()
    self:SendLoversMatchReq(LoversMatchReqType.TYPE_RANK_INFO)
end

function LoversMatchCtrl:SendDayCount()
    local cfg = PeriodActivityData.Instance:GetConfig(Mod.PeriodActivity.LoversMatch)
    if cfg and cfg.period_type == PeriodActivityData.Instance:GetPeriodType() then
        self:SendLoversMatchReq(LoversMatchReqType.TYPE_DAY_COUNT)
    end
end

function LoversMatchCtrl:SendUseImage(obj_id)
    self:SendLoversMatchReq(LoversMatchReqType.TYPE_USE_FORM, obj_id)
end

function LoversMatchCtrl:CheckFriend()
    local score = self.data.fb_info.integral or 0
    local member_list = TeamCtrl.Instance:Data():GetTeamInfo():MemberListSort()
    local point_cfg = LoversMatchData.Instance:GetPointTitleCfg(score)
    if #member_list < 2 or point_cfg == nil then
        return
    end
    for i, v in pairs(member_list) do
        if v.info ~= nil and v.info.uid ~= RoleData.Instance:GetRoleId() then
            if not SocietyData.Instance:IsFriend(v.info.uid) then
                ViewMgr:OpenView(LoversMatchAlertView, {info = v.info, point_cfg = point_cfg})
            end
            return
        end
    end
end

function LoversMatchCtrl:CheckShowTips()
    if self.data:IsShowTips() then

        local function func_custom()
            local info = {
                content = Language.LoversMatch.CloseSure,
                confirm = {
                    func = function()
                        if DialogTipsData.Instance:GetTodayValue(DTTodayNotRemind.LoversMatchTips) then
                            self.data:SetNoShowTips()
                            DialogTipsData.Instance:SetTodayValue(DTTodayNotRemind.LoversMatchTips, false)
                        end
                        PeriodActivityCtrl.Instance:CloseTipsPopup()
                        PublicPopupCtrl.Instance:CloseDialogTips()
                        self:SendBegin()
                    end,
                },
                tnr = DTTodayNotRemind.LoversMatchTips,
                trn_str = Language.LoversMatch.NoRemind
            }
            PublicPopupCtrl.Instance:DialogTips(info)
        end
        PeriodActivityCtrl.Instance:HelpTip(self.data:OtherConfig().dec_id, Mod.PeriodActivity.LoversMatch, nil, {custom_close_func = func_custom})
    else
        self:SendBegin()
    end
end

