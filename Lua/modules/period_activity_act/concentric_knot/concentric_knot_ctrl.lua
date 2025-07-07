FILE_REQ("modules/period_activity_act/concentric_knot/concentric_knot_data")
VIEW_REQ("modules/period_activity_act/concentric_knot/concentric_knot_view")
VIEW_REQ("modules/period_activity_act/concentric_knot/concentric_knot_alert_view")
VIEW_REQ("modules/period_activity_act/concentric_knot/concentric_knot_team_view")
VIEW_REQ("modules/period_activity_act/concentric_knot/concentric_knot_finish_view")

local ConcentricKnotReqType = {
    TYPE_ENTER = 0,     --准备
    TYPE_AGREE = 1,     --准备回复
    TYPE_START = 2,     --开始
    TYPE_OPEN_CARD = 3, --翻牌
    TYPE_END = 4,       --游戏结束
    TYPE_INFO = 5,      --请求个人信息
}

ConcentricKnotCtrl = ConcentricKnotCtrl or BaseClass(BaseCtrl)
function ConcentricKnotCtrl:__init()
    if ConcentricKnotCtrl.Instance ~= nil then
        Debuger.LogError("[ConcentricKnotCtrl] attempt to create singleton twice!")
        return
    end
    ConcentricKnotCtrl.Instance = self
    self.data = ConcentricKnotData.New()
    self:RegisterAllProtocols()
    self.send_time_list = {}
end

function ConcentricKnotCtrl:__delete()
    ConcentricKnotCtrl.Instance = nil
    Delete(self, "data")
    EventSys:UnBind(self.care_role_complete)
end

function ConcentricKnotCtrl:OnInit()
    BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data:GetNpcSeq(), BindTool.Bind(self.SetupBehaviourBtn, self))
	EventSys:Bind(GlobalEventKeys.ServerOpenDayChange, BindTool.Bind1(self.OnDayChange, self))
    EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))

    ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.ConcentricKnot, BindTool.Bind1(self.OnClickActEvent, self))
    PeriodActivityData.Instance:CustomCreateHandle(Mod.PeriodActivity.ConcentricKnot, BindTool.Bind1(self.OnActOpen, self))
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.ConcentricKnot, BindTool.Bind1(self.OnActOver, self))
    PeriodActivityData.Instance:CustomTimesHandle(Mod.PeriodActivity.ConcentricKnot, BindTool.Bind1(self.TimesStr, self))
    Remind.Instance:Register(Mod.PeriodActivity.ConcentricKnot, self.data.act_info, function()
        if PeriodActivityData.Instance:GetRemindData(Mod.PeriodActivity.ConcentricKnot) then
            return 0
        end
        return self.data:IsActOpen() and self.data:GetRemindNum() or 0
    end)
end

function ConcentricKnotCtrl:OnUnloadGameLogic()
end

function ConcentricKnotCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCConcentricKnotAgreeNotice, "OnSCConcentricKnotAgreeNotice")
    self:RegisterProtocol(SCConcentricKnotStart, "OnSCConcentricKnotStart")
    self:RegisterProtocol(SCConcentricKnotOpenCard, "OnSCConcentricKnotOpenCard")
    self:RegisterProtocol(SCConcentricKnotEndGame, "OnSCConcentricKnotEndGame")
    self:RegisterProtocol(SCConcentricKnotInfo, "OnSCConcentricKnotInfo")
    self:RegisterProtocol(CSConcentricKnotReq)
end

function ConcentricKnotCtrl:RoleNoticeComplete()
    Invoke(function()
        self:SendInfo()
    end, 1)
end

function ConcentricKnotCtrl:OnDayChange()
    Invoke(function()
        self:SendInfo()
    end, 1)
end

--点击活动操作
function ConcentricKnotCtrl:OnClickActEvent()
    SceneLogic.Instance:AutoToNpc(self.data:GetNpcSeq(), nil, nil, true)
end

function ConcentricKnotCtrl:OnActOpen()
    SceneCtrl.Instance:AddNpc(self.data:GetNpcSeq())
    return true
end
function ConcentricKnotCtrl:OnActOver()
    SceneCtrl.Instance:DeleteNpc(self.data:GetNpcSeq())
end
function ConcentricKnotCtrl:TimesStr()
    return self.data:GetRemainTimes(), self.data:GetMaxTimes()
end

--场景Npc相关
function ConcentricKnotCtrl:SetupBehaviourBtn(btn_list,obj_vo)
    if nil == obj_vo or not self.data:IsActOpen() then
        return
    end
    self:SetupBehaviourBtn2(btn_list, obj_vo)
end
function ConcentricKnotCtrl:SetupBehaviourBtn2(btn_list,obj_vo)
    table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.ConcentricKnot.NpcButtonName[2], BindTool.Bind1(self.OnClickRule, self), false, nil))
    table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.ConcentricKnot.NpcButtonName[1], BindTool.Bind1(self.OnClickBegin, self), true, nil))
end
function ConcentricKnotCtrl:OnClickRule()
    NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.data:OtherConfig().talk_rule, not_block_close_func = true}, function()
        local param_t = NpcDialogData.Instance.cahce_chck_data
        if param_t then
            NpcDialogCtrl.Instance:NpcDialogView(param_t)
        end
    end, true)
end
function ConcentricKnotCtrl:OnClickBegin()
    if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() then
        PublicPopupCtrl.Instance:Center(ErrorText[57])
        return
    end
    if TeamData.Instance:InTeam() then
        if TeamData.Instance:MemberNumRole() > 2 then
            PublicPopupCtrl.Instance:Center(Language.ConcentricKnot.FBError1)
            return
        end
        local team_type = TeamData.Instance:GetTeamInfo().info.team_type
        if self.data:TeamList()[team_type] == nil then
            ViewMgr:OpenView(ConcentricKnotAlertView)
        else
            local function func_succ()
                self:SendEnter()
            end
            TeamCtrl.Instance:TeamDetectByTargetId(team_type, func_succ, 2)
        end
    else
        ViewMgr:OpenView(ConcentricKnotAlertView)
    end
end

--请求
function ConcentricKnotCtrl:SendReq(req_type, param1)
    local protocol = GetProtocol(CSConcentricKnotReq)
    protocol.req_type = req_type
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end

--队伍信息
function ConcentricKnotCtrl:OnSCConcentricKnotAgreeNotice(protocol)
    if protocol.is_agree == 0 then
        self.data:ClearTeamInfo()
        ViewMgr:CloseView(ConcentricKnotTeamView)
        if protocol.uid ~= RoleData.Instance:GetRoleId() then
            PublicPopupCtrl.Instance:Center(Language.ConcentricKnot.TeamError1)
        end
    else
        if protocol.uid == self.data:GetLeaderUID() and not ViewMgr:IsOpen(ConcentricKnotTeamView) then
            self.data:ClearTeamInfo()
            -- self.data:SetAgreeNotice(protocol)
            ViewMgr:OpenView(ConcentricKnotTeamView)
            ViewMgr:CloseView(ConcentricKnotView)
        else
            self.data:SetAgreeNotice(protocol)
        end
    end
end

--游戏开始
function ConcentricKnotCtrl:OnSCConcentricKnotStart(protocol)
    self.data:SetStartInfo(protocol)
end

--翻牌改变
function ConcentricKnotCtrl:OnSCConcentricKnotOpenCard(protocol)
    self.data:OnCardChange(protocol)
end

function ConcentricKnotCtrl:OnSCConcentricKnotEndGame(protocol)
    self.data:OnGameOver(protocol)
    self:SendInfo()
end

function ConcentricKnotCtrl:OnSCConcentricKnotInfo(protocol)
    self.data:SetActInfo(protocol)
end

function ConcentricKnotCtrl:SendInfo()
    local cfg = PeriodActivityData.Instance:GetConfig(Mod.PeriodActivity.ConcentricKnot)
    if cfg and cfg.period_type == PeriodActivityData.Instance:GetPeriodType() then
        self:SendReq(ConcentricKnotReqType.TYPE_INFO)
    end
end

--队长请求进入   队员请求同意
function ConcentricKnotCtrl:SendEnter()
    self:SendReq(ConcentricKnotReqType.TYPE_ENTER)
end
function ConcentricKnotCtrl:SendAgree(is_agree)
    self:SendReq(ConcentricKnotReqType.TYPE_AGREE, is_agree)
end

function ConcentricKnotCtrl:SendStart(cards_grade)
    self:SendReq(ConcentricKnotReqType.TYPE_START, cards_grade)
end

function ConcentricKnotCtrl:SendOpenCard(card_id)
    self:SendReq(ConcentricKnotReqType.TYPE_OPEN_CARD, card_id)
end

function ConcentricKnotCtrl:SendGameEnd(is_win)
    self:SendReq(ConcentricKnotReqType.TYPE_END, is_win or 0)
end

function ConcentricKnotCtrl:SendInvite(item)
    local show_str = ""
    local item_list = {}
	for i, v in ipairs(ConcentricKnotData.Instance:GetStuffList()) do
        local bag_items = BagData.Instance:GetItemsByItemId(v.item.item_id)
        if #bag_items > 0 then
            table.insert(item_list, ChatItemMono.New(bag_items[1]))
            if show_str ~= "" then
				show_str = show_str .. "、"
            end
            show_str = show_str .. Format("{%s;%s;%s}", ChatItemMono.Mark(), #item_list, item_list[#item_list]:ParamShow())
        end
	end
	if show_str == "" then
		return
	end
    local link_str = Format("{conceont_knot_invite;%s;%s}", RoleData.Instance:GetRoleName(), RoleData.Instance:GetRoleId())
    local send_str = Format(Language.ConcentricKnot.ChatItem1, show_str, link_str)

    ChatData.Instance:SelChannelId(ChatConfig.ChannelId.world, true)
	ViewMgr:OpenView(ChatView)
	ChatCtrl.Instance:SendChannelChatText(send_str, item_list, ChatConfig.ChannelType.world)
	-- ChatData.Instance:SendGrey()
end

function ConcentricKnotCtrl:SendInviteBack(param_t)
    if param_t[3] == nil then
        return
    end
    local role_id = tonumber(param_t[3])
    if role_id == RoleData.Instance:GetRoleId() then
        PublicPopupCtrl.Instance:Center(Language.ConcentricKnot.ChatItem2)
        return
    end
    if self.send_time_list[role_id] == nil or self.send_time_list[role_id] + 15 < TimeHelper.GetServerTime() then
        self.send_time_list[role_id] = TimeHelper.GetServerTime()
        local send_str = Format(Language.ConcentricKnot.ChatItem3, param_t[2])
        ChatCtrl.Instance:SendChannelChatText(send_str, nil, ChatConfig.ChannelType.world)
        -- ChatData.Instance:SendGrey()
    else
        PublicPopupCtrl.Instance:Center(Language.ConcentricKnot.ChatItem4)
    end
end
