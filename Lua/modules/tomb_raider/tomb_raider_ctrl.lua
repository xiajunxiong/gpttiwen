FILE_REQ("modules/tomb_raider/tomb_raider_data")
FILE_REQ("modules/tomb_raider/tomb_raider_battle_show")
FILE_REQ("modules/tomb_raider/tomb_raider_config")
VIEW_REQ("modules/tomb_raider/tomb_raider_view")
VIEW_REQ("modules/tomb_raider/tomb_raider_rank_view")
VIEW_REQ("modules/tomb_raider/tomb_raider_reward_view")
VIEW_REQ("modules/tomb_raider/tomb_raider_guide_view")
VIEW_REQ("modules/tomb_raider/tomb_raider_camp_view")
VIEW_REQ("modules/tomb_raider/tomb_raider_battle_view")
VIEW_REQ("modules/tomb_raider/tomb_raider_battle_bar")
VIEW_REQ("modules/tomb_raider/tomb_raider_finish_view")

TombRaiderCtrl = TombRaiderCtrl or BaseClass(BaseCtrl)
function TombRaiderCtrl:__init()
	if TombRaiderCtrl.Instance ~= nil then
		Debuger.LogError("[TombRaiderCtrl] attempt to create singleton twice!")
		return
	end	
    TombRaiderCtrl.Instance = self
    self.data = TombRaiderData.New()
	self.battle_show = TombRaiderBattleShow.New()
	self:RegisterAllProtocols()
end

function TombRaiderCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSRelicLootingMatchReq)
	self:RegisterProtocol(CSRelicLootingUseSkillReq)
	self:RegisterProtocol(CSTeamLeaderEjectWindowsReq)
	self:RegisterProtocol(SCRelicLootingTeamInfoReq,"RecvRelicLootingTeamInfoReq")
	self:RegisterProtocol(SCRelicLootingTampInfo,"RecvRelicLootingTampInfo")
	self:RegisterProtocol(SCRelicLootingFBSkillInfo,"RecvSCRelicLootingFBSkillInfo")
	self:RegisterProtocol(SCRelicootingFBFinishInfo,"RecvRelicootingFBFinishInfo")
	self:RegisterProtocol(SCRelicLootingObjInfo,"RecvRelicLootingObjInfo")
	self:RegisterProtocol(SCRelicootingMemberPosInfo,"RecvRelicootingMemberPosInfo")
	self:RegisterProtocol(SCRelicootingRankInfo,"RecvRelicootingRankInfo")
	self:RegisterProtocol(SCRelicootingMarkInfo,"RecvRelicootingMarkInfo")
	self:RegisterProtocol(SCRelicootingMemberTransmitInfo,"RecvRelicootingMemberTransmitInfo")
	self:RegisterProtocol(SCRelicLootingAchievementInfo,"RecvRelicLootingAchievementInfo")
	self:RegisterProtocol(SCRelicLootingEffectInfo,"RecvRelicLootingEffectInfo")
	self:RegisterProtocol(SCTeamLeaderEjectWindowsInfo,"RecvTeamLeaderEjectWindowsInfo")
end 

function TombRaiderCtrl:OnInit()
	ActivityData.Instance:CareEvent(BindTool.Bind(self.OnActEvent, self))

	BehaviorData.Instance:CustomNpcTalkBtnsSeq(Config.treasure_contend_auto.other[1].npc_seq , BindTool.Bind(self.SetupBehaviourBtn, self),true)
end

function TombRaiderCtrl:OnActEvent()
	if not ActivityData.IsOpen(ActType.TombRaider) then 
		local role_id = RoleData.Instance:GetRoleId()
		UnityPlayerPrefs.SetInt(role_id.."tomb_raider_opened",0)
		TombRaiderData.Instance.achievement_info.show_mark = nil
	end 
end

-- is_match 0 取消匹配 1 请求匹配 2 查询排行 3 成就奖励（param seq 4 成就下发 5 引导完毕

function TombRaiderCtrl:SendMatchOperate(is_match,param)
	-- LogError("?SendMatchOperate",is_match,param)
    local protocol = GetProtocol(CSRelicLootingMatchReq)
    protocol.is_match = is_match
	protocol.param = param or 0
    SendProtocol(protocol)
end

--[[
	type 1 丢弃物品技能 
	2 获取物品技能 param1 对应物品 obj_id
	3 使用技能 param1 技能id
	抓捕(sn1 param2 被抓捕者
	4 接受传送邀请 param1邀请uid
	5 标记地图 param1 标记类型 param2 pos_x param3 pos_y
]]
function TombRaiderCtrl:SendSkillOperate(param_t)
	local protocol = GetProtocol(CSRelicLootingUseSkillReq)
	protocol.type = param_t.type or 0
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	protocol.param3 = param_t.param3 or 0
	protocol.param4 = param_t.param4 or 0
	SendProtocol(protocol)
end

function TombRaiderCtrl:SendLeaderEjectWindows(param_t)
	local protocol = GetProtocol(CSTeamLeaderEjectWindowsReq)
	protocol.windows_type = param_t.windows_type
	SendProtocol(protocol)
end

--0 是守卫者 1是盗宝者
function TombRaiderCtrl:RecvRelicLootingTeamInfoReq(protocol)
	self.data:SetCurTeamInfo(protocol)
end

-- 
function TombRaiderCtrl:RecvRelicLootingTampInfo(protocol)
	self.data:SetGameStartState(protocol)
end

function TombRaiderCtrl:RecvSCRelicLootingFBSkillInfo(protocol)

	self.data:SetSkillInfo(protocol)
end

function TombRaiderCtrl:RecvRelicootingFBFinishInfo(protocol)
	-- LogError("RecvRelicootingFBFinishInfo",protocol)	
	self.data:SetFBFinishInfo(protocol)
	ViewMgr:OpenView(TombRaiderFinishView)
end

function TombRaiderCtrl:RecvRelicLootingObjInfo(protocol)
	self.data:SetMapObjInfo(protocol)
end

function TombRaiderCtrl:RecvRelicootingMemberPosInfo(protocol)
	-- LogError("?RecvRelicootingMemberPosInfo?",protocol)
	self.data:SetMemberPosInfo(protocol)
end

function TombRaiderCtrl:RecvRelicootingRankInfo(protocol)
	-- LogError("?RecvRelicootingRankInfo?",protocol)
	self.data:SetRankInfo(protocol)
end

function TombRaiderCtrl:RecvRelicootingMarkInfo(protocol)
    -- LogError("RecvRelicootingMarkInfo",protocol)
	self.data:SetMarkInfo(protocol)
end

function TombRaiderCtrl:RecvRelicootingMemberTransmitInfo(protocol)
	-- LogError("RecvRelicootingMemberTransmitInfo",protocol)
	self.data:SetTransmitInfo(protocol)
end 

function TombRaiderCtrl:RecvRelicLootingAchievementInfo(protocol)
	-- LogError("RecvRelicLootingAchievementInfo",protocol)
	self.data:SetAchievementInfo(protocol)
end

function TombRaiderCtrl:RecvRelicLootingEffectInfo(protocol)
	-- LogError("?RecvRelicLootingEffectInfo?",protocol)
	self.battle_show:EffectInfoHandle(protocol)
end 

function TombRaiderCtrl:RecvTeamLeaderEjectWindowsInfo(protocol)
	ViewMgr:OpenViewByKey(protocol.windows_type,true)
end

function TombRaiderCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	for k,v in pairs(btn_list) do
		if Config.treasure_contend_auto.other[1].behavior_id == v.beh_id and not ActivityData.IsOpen(ActType.TombRaider) then 
			table.remove( btn_list, k)
		end 
	end 
	return btn_list
end

--点击头像
function TombRaiderCtrl:OnRoleInfo(data)
	if data == nil or data.is_self or data.info == nil then
		return
	end
	RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
			return RoleInfoData.RoleInfoNew(data.info)
		end,
		funcs = function()
			local config = RoleInfoConfig.OnlyKickOut
            return config
		end,
		hide_base = true
	}
end

function TombRaiderCtrl:CheckNeedAdjust()
	if not TeamData.Instance:InTeam() then return false end 
	local flag_num = TeamData.Instance:MemberNumRole() > 3

	if flag_num then 
		PublicPopupCtrl.Instance:DialogTips({
			content = Language.TombRaider.PopupTeamTip,
			confirm = {name = Language.Battlefield.BtnTeamTip,
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				ViewMgr:OpenView(TeamView)
			end},
			cancel = {
				func = function()
					ViewMgr:CloseView(DialogTipsView)
				end
			},
		})
	end 

	return flag_num
end 