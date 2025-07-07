FILE_REQ("modules/god_areas_acts/god_areas_acts_data")
VIEW_REQ("modules/god_areas_acts/god_areas_acts")
VIEW_REQ("modules/god_areas_acts/areas_god_animals")
VIEW_REQ("modules/god_areas_acts/areas_god_ruins")
VIEW_REQ("modules/god_areas_acts/areas_god_palaces")
VIEW_REQ("modules/god_areas_acts/palaces_worship")
VIEW_REQ("modules/god_areas_acts/palaces_history_rank")
VIEW_REQ("modules/god_areas_acts/god_areas_battle_report")

GodAreasActsCtrl = GodAreasActsCtrl or BaseClass(BaseCtrl)
function GodAreasActsCtrl:__init()
    if GodAreasActsCtrl.Instance ~= nil then
		Debuger.LogError("[GodAreasActsCtrl] attempt to create singleton twice!")
		return
	end
	GodAreasActsCtrl.Instance = self
	self.data = GodAreasActsData.New()
    self:RegisterAllProtocols()

    ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE,
    {MainType = GodAreasActsCtrl.Instance,
    challenge_func = "FightChallenge",
    pet_on_limit_func = "PetOnLimitNum",
    assist_text_func = "AssistTextFunc",
    assist_limit_text_func = "AssistLimitTip"
    })

    FinishData.Instance:SetCloseFunc(BATTLE_MODE.ZHEN_YING_SHEN_SHOU,function(is_win)
        LandsBetweenCtrl.Instance:TryOpenMain()
        ViewMgr:OpenViewByKey(Mod.GodAreasActs.Animals)
	end)
end
function GodAreasActsCtrl:AssistTextFunc()
    return ""
end
function GodAreasActsCtrl:AssistLimitTip()
    return ""
end
function GodAreasActsCtrl:FightChallenge()
    -- 神坛战斗
    if ViewMgr:IsOpen(LandsBetweenSingleAltarView) then
        local map_pic = LandsBetweenData.Instance:GetMapPicClick()
        local fix_link = LandsBetweenData.FixLink(map_pic.static.link_mark)
        LandsBetweenCtrl.Instance:OperateOccupy({param_1 = fix_link[1],param_2 = fix_link[2],param_3 = 1})
        ViewMgr:CloseView(LandsBetweenSingleAltarView)
        ViewMgr:CloseView(LandsBetweenMainView)
        return 
    end 

    -- 龙脉战斗
    if ViewMgr:IsOpen(LBDragonPluseView) then
        if LBDragonPluseData.Instance.operate_info.send_data.is_boss then 
            LBDragonPluseCtrl.Instance:ReqDragonAttackBoss({
                index = LBDragonPluseData.Instance.operate_info.cache.index-1
            })
        else
            LBDragonPluseCtrl.Instance:ReqDragonAttackMonster({
                index = LBDragonPluseData.Instance.operate_info.cache.index-1,
                id_1 = LBDragonPluseData.Instance.operate_info.send_data.help_info[1].id,
                id_2 = LBDragonPluseData.Instance.operate_info.send_data.help_info[2].id
            })
        end 
    
        ViewMgr:CloseView(LBDragonPluseDetailView)
        ViewMgr:CloseView(LBDragonPluseView)
        ViewMgr:CloseView(LandsBetweenMainView)
        ViewMgr:CloseView(DialogTipsView)
        return 
    end 

    local boss_info  = self.data:GetBossInfo()
    local total_times = self.data:ChallengeTimes()
    if boss_info.fight_times >= total_times then
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.CantChallenge)
        return
    end
    GodAreasActsCtrl.Instance:CSZhenYingShenShouInfo(GodAreasActsData.Animals_Req_Type.Fight)
end
function GodAreasActsCtrl:PetOnLimitNum()
    return 4
end
function GodAreasActsCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSZhenYingShenShouInfo)
    self:RegisterProtocol(SCZhenYingShenShouInfo, "SCZhenYingShenShouInfo")
    self:RegisterProtocol(SCZhenYingShenShouRoleInfo, "SCZhenYingShenShouRoleInfo")
    self:RegisterProtocol(SCZhenYingShenShouRank, "SCZhenYingShenShouRank")
    
    
    self:RegisterProtocol(CSGodSpacePalaceReq)
    self:RegisterProtocol(SCGodSpacePalaceRankInfo, "SCGodSpacePalaceRankInfo")
    self:RegisterProtocol(SCGodSpacePalaceStatueInfo, "SCGodSpacePalaceStatueInfo")
    self:RegisterProtocol(SCGodSpacePalaceWorshipInfo, "SCGodSpacePalaceWorshipInfo")


end

function GodAreasActsCtrl:__delete()
    
end
function GodAreasActsCtrl:CSZhenYingShenShouInfo(req_type, p1)
	local protocol = GetProtocol(CSZhenYingShenShouInfo)
    protocol.req_type = req_type
    protocol.p1 = p1 or 0
    SendProtocol(protocol)
end

function GodAreasActsCtrl:SCZhenYingShenShouInfo(protocol)
	self.data:SetHp(protocol.hp_percent)
end
function GodAreasActsCtrl:SCZhenYingShenShouRoleInfo(protocol)
	self.data:SetRoleInfo(protocol)
end

function GodAreasActsCtrl:SCZhenYingShenShouRank(protocol)
    self.data:SetAnimalsRank(protocol)
end


function GodAreasActsCtrl:SCGodSpacePalaceRankInfo(protocol)
    self.data:SetPalaceRankInfo(protocol.season_id, protocol.rank_info)
end
function GodAreasActsCtrl:SCGodSpacePalaceStatueInfo(protocol)
    self.data:SetPalaceStatueInfo(protocol.rank_info)
end
function GodAreasActsCtrl:SCGodSpacePalaceWorshipInfo(protocol)
    self.data:SetPalacesWorship(protocol.palace_worship_flag)
end

function GodAreasActsCtrl:GodSpaceOperateWorship()
    local param_t = {}
    param_t.oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_PALACE_WORSHIP
    LandsBetweenCtrl.Instance:GodSpaceOperate(param_t)
end
function GodAreasActsCtrl:GodSpaceOperatePalaceRank(season_id, start_num)
    local param_t = {}
    param_t.oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_PALACE_RANK_QUERY
    param_t.param_1 = season_id or 1
    param_t.param_2 = start_num or 0
    LandsBetweenCtrl.Instance:GodSpaceOperate(param_t)
end
function GodAreasActsCtrl:GodSpaceOperatePalaceFirstRank()
    local param_t = {}
    param_t.oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_PALACE_FIRST_RANK_QUERY
    LandsBetweenCtrl.Instance:GodSpaceOperate(param_t)
end

function GodAreasActsCtrl:SendEnterPalace()
    local protocol = GetProtocol(CSGodSpacePalaceReq)
    SendProtocol(protocol)
end