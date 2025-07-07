FILE_REQ("modules/god_areas_palaces/palaces_data")
VIEW_REQ("modules/god_areas_palaces/palaces_view")
VIEW_REQ("modules/god_areas_palaces/palaces_achi")
VIEW_REQ("modules/god_areas_palaces/palaces_map")
VIEW_REQ("modules/god_areas_palaces/palaces_result")
VIEW_REQ("modules/god_areas_palaces/palaces_rank")
VIEW_REQ("modules/god_areas_palaces/palaces_challenge_rank")
VIEW_REQ("modules/god_areas_palaces/palaces_scene")
VIEW_REQ("modules/god_areas_palaces/palaces_hurt")

PalacesCtrl = PalacesCtrl or BaseClass(BaseCtrl)

function PalacesCtrl:__init()
	if PalacesCtrl.Instance ~= nil then
		Debuger.LogError("[PalacesCtrl] attempt to create singleton twice!")
		return
	end
	PalacesCtrl.Instance = self
	self.data = PalacesData.New()
    self:RegisterAllProtocols()
    --场景分页新增 功能类型 1复活点2挑战层3boss层
end

function PalacesCtrl:__delete()
    if self.handle_care then
        self.data.act_info_sm:Uncare()
        self.handle_care = nil
    end
    self:UnRegisterAllProtocols()
    PalacesCtrl.Instance = nil
end
function PalacesCtrl:OnInit()
    FinishData.Instance:PassRegister(BATTLE_MODE.YUN_DING_TIAN_GONG_ATTACK_BOSS,{
        desc_func = function()
            return string.format(Language.GodAreasActs.TotalHurt, self.data.hurt_info.hurt)
        end,
        show_type = function()
            return 5
        end
    })
    ActivityData.Instance:CustomClickHandle(ActType.Palaces,function()
        if LoginData.Instance:BaseData().is_on_crosssever == false then 
            LoginCtrl.Instance:SendCrossStartReq()
        end
        LandsBetweenCtrl.Instance:TryOpenMain()
        ViewMgr:OpenViewByKey(Mod.GodAreasActs.Palaces)
    end)
    ActivityData.Instance:CustomRemindClickHandle(ActType.Palaces,function()
        if LoginData.Instance:BaseData().is_on_crosssever == false then 
            LoginCtrl.Instance:SendCrossStartReq()
        end
        LandsBetweenCtrl.Instance:TryOpenMain()
        ViewMgr:OpenViewByKey(Mod.GodAreasActs.Palaces)
    end)
    EventSys:Bind(LandsBetweenEvent.BaseInfoChange, BindTool.Bind(self.LandsBetweenEventChange, self))
    self.handle_care = self.data.act_info_sm:Care(BindTool.Bind(self.LandsBetweenEventChange, self))
end
function PalacesCtrl:LandsBetweenEventChange()
    local base_info = LandsBetweenData.Instance:GetBaseInfo()
    if base_info.cur_status < GOD_SPACE_STATUS.InnerOpen then
       self.data:SetAchievementInfo({}) 
    end
    --内域晚上
    local is_open, need_wait = self.data:IsOpen()
    -- is_open = true
    if is_open then
        -- PublicPopupCtrl.Instance:ActTips(self.data.act_msg, os.time(self.data:EndTime()))
        self:ActTips()
    else
        self:ActTipsClose()
    end
end

function PalacesCtrl:ActTips()
    local act_msg = {act_status_info = {}}
	act_msg.act_status_info.act_type = ActType.Palaces
	act_msg.act_status_info.status = ActStatusType.Open
    act_msg.act_status_info.next_status_switch_time = os.time(self.data:EndTime())
    ActivityData.Instance:SetActivityStatus(act_msg)
end
function PalacesCtrl:ActTipsClose()
    local act_msg = {act_status_info = {}}
	act_msg.act_status_info.act_type = ActType.Palaces
	act_msg.act_status_info.status = ActStatusType.Close
    act_msg.act_status_info.next_status_switch_time = 0
    ActivityData.Instance:SetActivityStatus(act_msg)
end

function PalacesCtrl:OnUnloadGameLogic()
    self.data.is_auto_battle_boss = false
    self.data.battle_report = {}
    self:ActTipsClose()
end

function PalacesCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSYunDingTianGongReq)
    self:RegisterProtocol(SCYunDingTianGongAchievementInfo, "SCYunDingTianGongAchievementInfo")
    self:RegisterProtocol(SCYunDingTianGongRoleInfo, "SCYunDingTianGongRoleInfo")
    self:RegisterProtocol(SCYunDingTianGongRank, "SCYunDingTianGongRank")
    self:RegisterProtocol(SCYunDingTianGongRoleRank, "SCYunDingTianGongRoleRank")
    self:RegisterProtocol(SCYunDingTianGongSceneRoleInfo, "SCYunDingTianGongSceneRoleInfo")
    self:RegisterProtocol(SCYunDingTianGongBossInfo, "SCYunDingTianGongBossInfo")
    self:RegisterProtocol(SCYunDingTianGongPVPResult, "SCYunDingTianGongPVPResult")
    self:RegisterProtocol(SCYunDingTianGongAttackBossResult, "SCYunDingTianGongAttackBossResult")
    self:RegisterProtocol(SCYunDingTianGongOpenInfo, "SCYunDingTianGongOpenInfo")
end


function PalacesCtrl:CSYunDingTianGongReq(req_type, p1, p2)
    local protocol = GetProtocol(CSYunDingTianGongReq)
    protocol.req_type = req_type or 0
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    SendProtocol(protocol)
end
function PalacesCtrl:PopupInfo()
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
function PalacesCtrl:EnterScene()
    self:CSYunDingTianGongReq(0)
end
function PalacesCtrl:ChangePlatform(platform_id)
    if self:PopupInfo() then
        self:CSYunDingTianGongReq(1, platform_id)
    end
end
function PalacesCtrl:ChallengeRole(plat_type, role_id)
    if self:PopupInfo() then
        self:CSYunDingTianGongReq(2, role_id, plat_type)
    end
end
function PalacesCtrl:ChallengeBoss()
    if self:PopupInfo() then
        self:CSYunDingTianGongReq(3)
    end
end
function PalacesCtrl:ReqRank(rank_type)
    self:CSYunDingTianGongReq(4, rank_type)
end
function PalacesCtrl:ReqAchiReward(achieve_type, ach_level)
    self:CSYunDingTianGongReq(5, achieve_type, ach_level)
end

function PalacesCtrl:UnRegisterAllProtocols()
    -- self:UnRegisterProtocol(CSTemplateReq)
    -- self:UnRegisterProtocol(SCTemplateInfo)
end
function PalacesCtrl:SCYunDingTianGongAchievementInfo(protocol)
    self.data:SetAchievementInfo(protocol.list)
end
function PalacesCtrl:SCYunDingTianGongRoleInfo(protocol)
    self.data:SetRoleInfo(protocol)
end
--3个
function PalacesCtrl:SCYunDingTianGongRank(protocol)
    self.data:SetMyRank(protocol.rank_type, protocol.rank, protocol.rank_value)
    self.data:SetRank(protocol.rank_type, protocol.rank_list)
end
function PalacesCtrl:SCYunDingTianGongRoleRank(protocol)
    self.data:SetMyRank(protocol.rank_type, protocol.rank, protocol.rank_value)
    self.data:SetRank(protocol.rank_type, protocol.rank_list)
end

function PalacesCtrl:SCYunDingTianGongSceneRoleInfo(protocol)
    self.data:SetSceneRoleInfo(protocol)
end
function PalacesCtrl:SCYunDingTianGongBossInfo(protocol)
    self.data:SetSceneBossInfo(protocol)
end
function PalacesCtrl:SCYunDingTianGongPVPResult(protocol)
    table.insert(self.data.battle_report, TableCopy(protocol))
    ViewMgr:OpenView(RuinsResult,{protocol = protocol})
end
function PalacesCtrl:SCYunDingTianGongAttackBossResult(protocol)
    self.data:SetBossHurt(protocol.total_hurt)
end
function PalacesCtrl:SCYunDingTianGongOpenInfo(protocol)
    self.data:SetActInfo(protocol.info)
end