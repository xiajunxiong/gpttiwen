 FILE_REQ("modules/god_areas_ruins/ruins_data")
VIEW_REQ("modules/god_areas_ruins/ruins_view")
VIEW_REQ("modules/god_areas_ruins/ruins_achi")
VIEW_REQ("modules/god_areas_ruins/ruins_map")
VIEW_REQ("modules/god_areas_ruins/ruins_result")
VIEW_REQ("modules/god_areas_ruins/ruins_rank")
VIEW_REQ("modules/god_areas_ruins/ruins_challenge_rank")
VIEW_REQ("modules/god_areas_ruins/ruins_scene")

RuinsCtrl = RuinsCtrl or BaseClass(BaseCtrl)

function RuinsCtrl:__init()
	if RuinsCtrl.Instance ~= nil then
		Debuger.LogError("[RuinsCtrl] attempt to create singleton twice!")
		return
	end
	RuinsCtrl.Instance = self
	self.data = RuinsData.New()
    --[[ local param = {}
	param.act_seq = 10052
	param.listen_data = self.data.base_info
	param.remin_data = self.data.my_info
	param.remind_func = BindTool.Bind(LBDragonPluseData.GetActRemind, LBDragonPluseData.Instance)
	param.click_func = BindTool.Bind(self.ClickActIcon, self)
	param.time_show_func = BindTool.Bind(LBDragonPluseData.GetActCurTimeStamp, LBDragonPluseData.Instance)
	param.act_show_func = BindTool.Bind(LBDragonPluseData.GetActIconShow, LBDragonPluseData.Instance)
	param.act_status_func = BindTool.Bind(LBDragonPluseData.GetActIconStatusShow, LBDragonPluseData.Instance)
	param.show_sp_sub = "dragon_vein_sub"
	LandsBetweenData.Instance:SetExtralActShowParam(param) ]]
    self:RegisterAllProtocols()
end

function RuinsCtrl:__delete()
    if self.handle_care then
        self.data.act_info_sm:Uncare(self.handle_care)
        self.handle_care = nil
    end
    self:UnRegisterAllProtocols()
    RuinsCtrl.Instance = nil
end
function RuinsCtrl:OnInit()
    FinishData.Instance:PassRegister(BATTLE_MODE.GUI_XU_ZHI_JING_BOSS,{
        desc_func = function()
            return string.format(Language.GodAreasActs.TotalHurt, self.data.hurt_info.hurt)
        end,
        show_type = function()
            return 5
        end
    })
    ActivityData.Instance:CustomClickHandle(ActType.Ruins,function()
        if LoginData.Instance:BaseData().is_on_crosssever == false then 
            LoginCtrl.Instance:SendCrossStartReq()
        end
        LandsBetweenCtrl.Instance:TryOpenMain()
        ViewMgr:OpenViewByKey(Mod.GodAreasActs.Ruins)
    end)
    ActivityData.Instance:CustomRemindClickHandle(ActType.Ruins,function()
        if LoginData.Instance:BaseData().is_on_crosssever == false then 
            LoginCtrl.Instance:SendCrossStartReq()
        end
        LandsBetweenCtrl.Instance:TryOpenMain()
        ViewMgr:OpenViewByKey(Mod.GodAreasActs.Ruins)
    end)
    EventSys:Bind(LandsBetweenEvent.BaseInfoChange,BindTool.Bind(self.LandsBetweenEventChange, self))
    self.handle_care = self.data.act_info_sm:Care(BindTool.Bind(self.LandsBetweenEventChange, self))
end
function RuinsCtrl:LandsBetweenEventChange()
    local base_info = LandsBetweenData.Instance:GetBaseInfo()
    if base_info.cur_status < GOD_SPACE_STATUS.OurterOpen then
       self.data:SetAchievementInfo({})
    end
    --外域晚上
    local is_open, need_wait = self.data:IsOpen()
    -- is_open = true
    if is_open then
        -- PublicPopupCtrl.Instance:ActTips(self.data.act_msg, os.time(self.data:EndTime()))
        self:ActTips()
    else
        self:ActTipsClose()
    end
end
function RuinsCtrl:ActTips()
    local act_msg = {act_status_info = {}}
	act_msg.act_status_info.act_type = ActType.Ruins
	act_msg.act_status_info.status = ActStatusType.Open
    act_msg.act_status_info.next_status_switch_time = os.time(self.data:EndTime())
    ActivityData.Instance:SetActivityStatus(act_msg)
end
function RuinsCtrl:ActTipsClose()
    local act_msg = {act_status_info = {}}
	act_msg.act_status_info.act_type = ActType.Ruins
	act_msg.act_status_info.status = ActStatusType.Close
    act_msg.act_status_info.next_status_switch_time = 0
    ActivityData.Instance:SetActivityStatus(act_msg)
end
function RuinsCtrl:OnUnloadGameLogic()
    self.data.is_auto_battle_boss = false
    self.data.battle_report = {}
    self:ActTipsClose()
end

function RuinsCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSGuiXuZhiJingReq)
    self:RegisterProtocol(SCGuiXuZhiJingAchievementInfo, "SCGuiXuZhiJingAchievementInfo")
    self:RegisterProtocol(SCGuiXuZhiJingRoleInfo, "SCGuiXuZhiJingRoleInfo")
    self:RegisterProtocol(SCGuiXuZhiJingRank, "SCGuiXuZhiJingRank")
    self:RegisterProtocol(SCGuiXuZhiJingRoleRank, "SCGuiXuZhiJingRoleRank")
    self:RegisterProtocol(SCGuiXuZhiJingSceneRoleInfo, "SCGuiXuZhiJingSceneRoleInfo")
    self:RegisterProtocol(SCGuiXuZhiJingBossInfo, "SCGuiXuZhiJingBossInfo")
    self:RegisterProtocol(SCGuiXuZhiJingPVPResult, "SCGuiXuZhiJingPVPResult")
    self:RegisterProtocol(SCGuiXuZhiJingAttackBossResult, "SCGuiXuZhiJingAttackBossResult")
    self:RegisterProtocol(SCGuiXuZhiJingOpenInfo, "SCGuiXuZhiJingOpenInfo")
end


function RuinsCtrl:SendGuiXuZhiJingReq(req_type, p1, p2)
    local protocol = GetProtocol(CSGuiXuZhiJingReq)
    protocol.req_type = req_type or 0
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    SendProtocol(protocol)
end
function RuinsCtrl:PopupInfo()
    local act_info = self.data:GetActInfosm().info
    if act_info.state == ActStatusType.Standy then
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.ActReady)
        return false
    elseif act_info.state == ActStatusType.Open then
        return true
    else
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.ActOver)
        return false
    end
    return true
end
function RuinsCtrl:EnterScene()
    self:SendGuiXuZhiJingReq(0)
end
function RuinsCtrl:ChangePlatform(platform_id)
    if self:PopupInfo() then
        self:SendGuiXuZhiJingReq(1, platform_id)
    end
end
function RuinsCtrl:ChallengeRole(plat_type, role_id)
    if self:PopupInfo() then
        self:SendGuiXuZhiJingReq(2, role_id, plat_type)
    end
end
function RuinsCtrl:ChallengeBoss()
    if self:PopupInfo() then
        self:SendGuiXuZhiJingReq(3)
    end
end
function RuinsCtrl:ReqRank(rank_type)
    self:SendGuiXuZhiJingReq(4, rank_type)
end
function RuinsCtrl:ReqAchiReward(achieve_type, ach_level)
    self:SendGuiXuZhiJingReq(5, achieve_type, ach_level)
end

function RuinsCtrl:UnRegisterAllProtocols()
    -- self:UnRegisterProtocol(CSTemplateReq)
    -- self:UnRegisterProtocol(SCTemplateInfo)
end
function RuinsCtrl:SCGuiXuZhiJingAchievementInfo(protocol)
    self.data:SetAchievementInfo(protocol.list)
end
function RuinsCtrl:SCGuiXuZhiJingRoleInfo(protocol)
    self.data:SetRoleInfo(protocol)
end
--3个
function RuinsCtrl:SCGuiXuZhiJingRank(protocol)
    self.data:SetMyRank(protocol.rank_type, protocol.rank, protocol.rank_value)
    self.data:SetRank(protocol.rank_type, protocol.rank_list)
end
function RuinsCtrl:SCGuiXuZhiJingRoleRank(protocol)
    self.data:SetMyRank(protocol.rank_type, protocol.rank, protocol.rank_value)
    self.data:SetRank(protocol.rank_type, protocol.rank_list)
end

function RuinsCtrl:SCGuiXuZhiJingSceneRoleInfo(protocol)
    self.data:SetSceneRoleInfo(protocol)
end
function RuinsCtrl:SCGuiXuZhiJingBossInfo(protocol)
    self.data:SetSceneBossInfo(protocol)
end
function RuinsCtrl:SCGuiXuZhiJingPVPResult(protocol)
    table.insert(self.data.battle_report, TableCopy(protocol))
    ViewMgr:OpenView(RuinsResult,{protocol = protocol})
end
function RuinsCtrl:SCGuiXuZhiJingAttackBossResult(protocol)
    self.data:SetBossHurt(protocol.total_hurt)
end
function RuinsCtrl:SCGuiXuZhiJingOpenInfo(protocol)
    self.data:SetActInfo(protocol.info)
end