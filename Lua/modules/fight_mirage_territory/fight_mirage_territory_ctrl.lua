FILE_REQ("modules/fight_mirage_territory/fight_mirage_territory_data")
VIEW_REQ("modules/fight_mirage_territory/fight_mirage_territory_view")
VIEW_REQ("modules/fight_mirage_territory/fight_mirage_territory_level_view")
VIEW_REQ("modules/fight_mirage_territory/formation_partner_view")
VIEW_REQ("modules/fight_mirage_territory/partner_dispatch_view")
FILE_REQ("modules/fight_mirage_territory/mirage_territory_config")

--琉璃蜃境
FightMirageTerritoryCtrl = FightMirageTerritoryCtrl or BaseClass(BaseCtrl)
function FightMirageTerritoryCtrl:__init()
    if FightMirageTerritoryCtrl.Instance ~= nil then
        Debuger.LogError("[FightMirageTerritoryCtrl] attempt to create singleton twice!")
        return
    end
    FightMirageTerritoryCtrl.Instance = self
    self.data = FightMirageTerritoryData.New()
    self:RegisterProtocol(CSMirageTerritorySetPartnerReq)
    self:RegisterProtocol(CSMirageTerritoryReq)
    self:RegisterProtocol(SCMirageTerritorySetPartnerInfo, "OnSCMirageTerritorySetPartnerInfo")
    self:RegisterProtocol(SCMirageTerritoryInfo, "OnSCMirageTerritoryInfo")

    self.handle_login = LoginData.Instance:BaseData():KeysCare({"login_state"}, BindTool.Bind(self.OnLoginEvent, self))

    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY,function(is_win)
        if is_win == 1 then
            ViewMgr:OpenView(DreamNotesRewardView,{reward_list = DataHelper.FormatItemList(self.data:GetFinishData()),
            is_not_first = false,close_func = function()
                ViewMgr:OpenView(FightMirageTerritoryLevelView)
            end})
		else
            if self.data.cur_pass_level then
                self.data:SetCurAreaData(self.data.cur_area_data)
                ViewMgr:OpenView(FightMirageTerritoryLevelView)
            end			
        end
	end)
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Experience.MirageTerritory,function(d)
        return self.data:GetDispatchTimes(),self.data.dispatch_data.all_num
    end)
    --主界面红点
	RemindCtrl.Instance:Remind():Register(Mod.Experience.MirageTerritory, FightMirageTerritoryData.Instance.dispatch_data, function ()
		return FightMirageTerritoryData.Instance:GetDispatchRemind()
	end)
end

function FightMirageTerritoryCtrl:OnLoginEvent()
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
        if FunOpen.Instance:GetFunIsOpened(Mod.Experience.MirageTerritory) then
            for i,v in pairs(self.data:GetAreaConfig()) do
                FightMirageTerritoryCtrl.Instance:SendMirageTerritoryInfoReq(MirageTerritoryConfig.ReqType.Info,v.seq)
            end
        end
    end
end

function FightMirageTerritoryCtrl:OnUnloadGameLogic()
    self.data:DataReset()
end

function FightMirageTerritoryCtrl:OnInit()
    ChallengeNaviCtrl.Instance:RegisterOpenReqFunc(Mod.Experience.MirageTerritory,BindTool.Bind(self.OnOpenView, self))
end

function FightMirageTerritoryCtrl:OnOpenView()   
    if TeamCtrl.Instance:Data():InTeam() then
        PublicPopupCtrl.Instance:AlertTip(Language.MirageTerritory.ExitTeamTip,function()
            TeamCtrl.Instance:SendExitTeam()
            ViewMgr:OpenView(FightMirageTerritoryView)
        end)
        return
    end
    ViewMgr:OpenView(FightMirageTerritoryView)
end

function FightMirageTerritoryCtrl:__delete()
    
end

--设置伙伴布阵
function FightMirageTerritoryCtrl:SendMirageTerritorySetPartnerReq(info_list,param1)
    local protocol = GetProtocol(CSMirageTerritorySetPartnerReq)
    protocol.programme_id = param1 or 0
    protocol.id_list = {}
    protocol.pos_list = {}
    for i = 1,#info_list do
        protocol.id_list[i] = info_list[i].id
        protocol.pos_list[i] = info_list[i].pos
    end
	SendProtocol(protocol)
end

--请求信息
function FightMirageTerritoryCtrl:SendMirageTerritoryInfoReq(req_type,param1,param2,param3,param4)
    local protocol = GetProtocol(CSMirageTerritoryReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    protocol.param3 = param3 or 0
    protocol.param4 = param4 or 0
	SendProtocol(protocol)
end

function FightMirageTerritoryCtrl:OnSCMirageTerritorySetPartnerInfo(protocol)
    self.data:SetFormationInfo(protocol)
end

function FightMirageTerritoryCtrl:OnSCMirageTerritoryInfo(protocol)
    self.data:SetAreaInfo(protocol)
end