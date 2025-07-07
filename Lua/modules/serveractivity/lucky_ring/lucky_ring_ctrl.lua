FILE_REQ("modules/serveractivity/lucky_ring/lucky_ring_data")
FILE_REQ("modules/serveractivity/lucky_ring/lucky_ring_battle_show")
VIEW_REQ("modules/serveractivity/lucky_ring/lucky_ring_battle_view")
VIEW_REQ("modules/serveractivity/lucky_ring/kucky_ring_synopsis_view")
VIEW_REQ("modules/serveractivity/lucky_ring/kucky_ring_finish_view")

LuckyRingCtrl = LuckyRingCtrl or BaseClass(BaseCtrl)
function LuckyRingCtrl:__init()
	if LuckyRingCtrl.Instance ~= nil then
		Debuger.LogError("[LuckyRingCtrl] attempt to create singleton twice!")
		return
	end	
    LuckyRingCtrl.Instance = self


    self.data = LuckyRingData.New()
    self.show_data = LuckyRingBattleShow.New()
	self:RegisterAllProtocols()

    self.is_waiting = false
end

function LuckyRingCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSTaoQuanNaJiReq)
    self:RegisterProtocol(SCTaoQuanNaJiInfo,"RecvTaoQuanNaJiInfo")
	self:RegisterProtocol(SCTaoQuanNaJiPlayEndInfo,"RecvTanQuanNaJiPlayEndInfo")
end 

function LuckyRingCtrl:OnInit()
    ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.LuckyRing, BindTool.Bind1(self.OnClickActEvent, self))
    PeriodActivityData.Instance:CustomCreateHandle(Mod.PeriodActivity.LuckyRing, BindTool.Bind1(self.OnActOpen, self))
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.LuckyRing, BindTool.Bind1(self.OnActOver, self))
    PeriodActivityData.Instance:CustomTimesHandle(Mod.PeriodActivity.LuckyRing, BindTool.Bind1(self.TimesStr, self))
    Remind.Instance:Register(Mod.PeriodActivity.LuckyRing, self.data.ring_info, function()
        if PeriodActivityData.Instance:GetRemindData(Mod.PeriodActivity.LuckyRing) then
            return 0
        end
        return self.data:IsActOpen() and self.data:GetRemindNum() or 0
    end)

    BehaviorData.Instance:CustomNpcTalkBtnsId(Config.lasso_auto.other[1].npc_id , BindTool.Bind(self.SetupBehaviourBtn, self),true)
end 

function LuckyRingCtrl:OnClickActEvent()
    SceneLogic.Instance:AutoToNpc(Config.lasso_auto.other[1].npc_id, nil, nil, true)
end

function LuckyRingCtrl:SetupBehaviourBtn()
    local list = {}
    local enter_btn = NpcDialogBehBtnData.FuncCreate(Language.LuckyRing.PlayGame, function ()
        self:StartLuckyRing() 
    end, true)
    table.insert(list,enter_btn)

    local info_btn = NpcDialogBehBtnData.FuncCreate(Language.LuckyRing.Infomation, function ()
        ViewMgr:OpenView(LuckyRingSynopsisView)
    end, true)
    table.insert(list,info_btn)
    return list
end

function LuckyRingCtrl:OnActOpen()
    -- self:StartLuckyRing() 
    self:SendLuckyRingReq({req_type = 0,param1 = 0,param2 = 0})
    self:SendLuckyRingReq({req_type = 5,param1 = 0,param2 = 0})
    VoMgr:AddNpc(Config.lasso_auto.other[1].npc_id, nil, nil, true)
end

function LuckyRingCtrl:OnActOver()
    VoMgr:DeleteNpc(Config.lasso_auto.other[1].npc_id, nil, nil, true)
end

function LuckyRingCtrl:TimesStr()
    local pass_time = self.data.ring_info.today_play_times <= Config.lasso_auto.other[1].reward_time and Config.lasso_auto.other[1].reward_time - self.data.ring_info.today_play_times or 0

    return pass_time,Config.lasso_auto.other[1].reward_time
end

function LuckyRingCtrl:SendLuckyRingReq(param)
    local protocol = GetProtocol(CSTaoQuanNaJiReq)
    protocol.req_type = param.req_type or 0
	protocol.param1 = param.param1 or 0
    protocol.param2 = param.param2 or 0
	-- LogDG("健哥哥这个是套圈请求信息哦?1139",protocol)
    SendProtocol(protocol)
end

function LuckyRingCtrl:RecvTaoQuanNaJiInfo(protocol)
    self.data:SetRingInfo(protocol)

    if self.is_waiting then 
        self:EnterLuckyRing()
    end 
end

-- 分数统计
function LuckyRingCtrl:RecvTanQuanNaJiPlayEndInfo(protocol)
    self.data:SetScoreInfo(protocol)
end

function LuckyRingCtrl:StartLuckyRing()
    TeamData.Instance:CheckSingle(function ()
        self:SendLuckyRingReq({req_type = 0,param1 = 0,param2 = 0})
        self:SendLuckyRingReq({req_type = 5,param1 = 0,param2 = 0})
        self.is_waiting = true    
    end)
    -- if TeamData.Instance:InTeam() then 
    --     PublicPopupCtrl.Instance:Center(Language.LuckyRing.TeamErrorTips)
    --     return 
    -- end 

end

function LuckyRingCtrl:EnterLuckyRing()
    self.data:FixMapFlush() 
    self:SendLuckyRingReq({req_type = 3,param1 = 0,param2 = 0})
    self.is_waiting = false
end

function LuckyRingCtrl:OpenFinishView()
    if self.data.finish_info.need_finish > 0 then 
        ViewMgr:OpenView(LuckyRingFinish)
        self.data.finish_info.need_finish = 0
    end 
end