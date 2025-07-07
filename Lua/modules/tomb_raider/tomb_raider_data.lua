TombRaiderData = TombRaiderData or BaseClass()

function TombRaiderData:__init()
	if TombRaiderData.Instance ~= nil then
		Debuger.LogError("[TombRaiderData] attempt to create singleton twice!")
		return
	end
	TombRaiderData.Instance = self

	self.tomb_raider_cfg = Config.treasure_contend_auto

    self.tomb_team = SmartData.New({attack = {},defend = {}})
	self.battle_view_data = SmartData.New({flush_rb = false, hide_rb = false, hide_li = false, show_all = false})
	self.rb_toggle = SmartData.New{isOn = true}

	self.skill_info = SmartData.New({common_list = {},item_skill = nil,is_flush = 0})
	self.game_start_state = SmartData.New({reason_type = 5,time_stamp = 0})
	self.finish_info = SmartData.New({win_type = 0,defend_team = {},attack_team = {}})
	self.member_pos_info = SmartData.New({member_list = {},transmit_uid = -1})
	self.mark_pos_info = SmartData.New({create_timer = 0})
	self.mark_info = SmartData.New({list = {}})
	self.map_obj_info = SmartData.New({obj_list = {},cur_box_num = 0,done_box_num = 0})
	self.transmits_info = SmartData.New({uid = 0,end_tamp = 0,player_name = ""})
	self.rank_info = {}

	self.achievement_info = SmartData.New({param={},reward_status = {}})

	self.follow_mark = {}

	self.tomb_matching = false
end

-- =====================数据存取===========================
-- 读配置
function TombRaiderData:GetRewardCfg()
	return self.tomb_raider_cfg.rank_reward
end
function TombRaiderData:GetRewardByRank(rank)
	for k,v in pairs(self.tomb_raider_cfg.rank_reward) do 
		if rank <= v.rank_range_max and rank >= v.rank_range_min then 
			return v
		end 
	end 
end 
function TombRaiderData:GetSkillCfg(sn)
	return self.tomb_raider_cfg.skill[sn]
end
-- 获取阵营技能配置
--0 是守卫者 1是盗宝者
function TombRaiderData:GetBattleSkillGroup(team_type)
	local team = {}
	local type = team_type or 0 
	for k,v in pairs(TombRaiderConfig.SkillGroup[type]) do 
		local vo = {}
		vo.cfg = self.tomb_raider_cfg.skill[v]
		vo.index = k
		vo.team_type = type 
		table.insert(team,vo)
	end 

	return team
end
-- 排名信息
function TombRaiderData:SetRankInfo(protocol)
	-- LogError("?protocol!",protocol)
	self.rank_info.rank_list = protocol.rank_list
	self.rank_info.my_pos = protocol.my_pos
	self.rank_info.rank_num = protocol.rank_num
end
function TombRaiderData:GetRankInfo()
	return self.rank_info
end
function TombRaiderData:GetBoxRewardInfo()
	return self.tomb_raider_cfg.box_reward
end
-- =====================入场阶段===========================
function TombRaiderData:SetCurTeamInfo(protocol)
	-- LogError("<color=#"..COLORSTR.Yellow1..">?SetCurTeamInfo</color>",protocol)
	self.tomb_team.attack = protocol.attack_team_list
	self.tomb_team.defend = protocol.defend_team_list
end
-- 记录并卸掉坐骑和跟随
function TombRaiderData:SetShowMark()
	self.follow_mark.bicycle = {rede_bicycle_index = BicycleData.Instance.ride_info.rede_bicycle_index}
	if self.follow_mark.bicycle.rede_bicycle_index > - 1 then
	    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.CANCEL_RIDE,self.follow_mark.bicycle.rede_bicycle_index)
	end 
	self.follow_mark.pet = {follow_index = PetData.Instance:GetFollowIndex()}
	if self.follow_mark.pet.follow_index > - 1 then 
	    PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, self.follow_mark.pet.follow_index, PET_STATE_OP.NOFOLLOW)
	end 
	self.follow_mark.role_setting = {
		is_show =RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER),
		show_num = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS),
    }

	RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS, 10)
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER, 1)
	RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER_NAME, 1)
	RoleSettingCtrl.Instance:SaveRoleSettingInfo()

	self.follow_mark.ride = { ride_id = RoleData.Instance.base_data.appearance.ride_id}
	if self.follow_mark.ride.ride_id > 0 then 
		for k,v in pairs(Config.surface_cfg_auto.surface_list) do 
			if v.res_id == self.follow_mark.ride.ride_id then 
			    self.follow_mark.ride.ride_id = v.id
			end 
		end 
		IllusionCtrl.Instance:SendSurfaceWear( 0 , IllusionConfig.FashionType.mount)
	end 
	-- 幻化可以直接整个铲掉
	if RoleData.Instance.base_data.appearance.model_card_seq > 0 then 
		LiveCtrl.Instance:SendSkillReq(LiveCtrl.LIVE_SKILL_OP_TYPE.UN_CHANGE_ROLE)
	end 
end
-- 清除记录并安装上坐骑和跟随
function TombRaiderData:EndShowMark()
	local flag_bicycle = self.follow_mark.bicycle.rede_bicycle_index > -1
	if flag_bicycle then 
		BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.RIDE,self.follow_mark.bicycle.rede_bicycle_index)
	end 
	self.follow_mark.bicycle = nil 
	local flag_pet = self.follow_mark.pet.follow_index > -1 
	if flag_pet then 
	    PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, self.follow_mark.pet.follow_index, PET_STATE_OP.FOLLOW)
	end 
	self.follow_mark.pet = nil 

	RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS,self.follow_mark.role_setting.show_num)
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER, self.follow_mark.role_setting.is_show)
	RoleSettingCtrl.Instance:SaveRoleSettingInfo()
	self.follow_mark.role_setting = nil

	local flag_ride = self.follow_mark.ride.ride_id > 0
	if flag_ride then 
		IllusionCtrl.Instance:SendSurfaceWear( self.follow_mark.ride.ride_id , IllusionConfig.FashionType.mount)
	end 
	self.follow_mark.ride = nil 
end

function TombRaiderData:InTombRaider()
	return SceneData.Instance:SceneId() == Config.treasure_contend_auto.other[1].scene_id
end

-- =====================后勤阶段===========================
-- 是否被禁止移动
function TombRaiderData:IsLimitedMove(operate_flag)
	-- LogError("?IsLimitedMove?", self:GetFreezeReason() )
	return self:GetFreezeReason(operate_flag) ~= nil
end
-- 获取禁止移动的原因(文本
-- operate_flag 是否进行中断判断
function TombRaiderData:GetFreezeReason(operate_flag)
	if self.game_start_state.reason_type == 0 then 
		return Language.TombRaider.GameStateWaitLoading
	elseif self.game_start_state.reason_type == 1 then 
		return Language.TombRaider.GameStateReadyStart
	elseif self.game_start_state.reason_type == 2 and ViewMgr:IsOpen(TombRaiderFinishView) then 
		return Language.TombRaider.GameStateEnd
	elseif self.skill_info.stop_reason and self.skill_info.stop_reason > 0 and self:InTombRaider() then
		if self.skill_info.stop_reason == 1 then 
		    return Language.TombRaider.BeCatched
		elseif self.skill_info.stop_reason == 2 then
			return Language.TombRaider.BeTraped
		end 
	elseif (operate_flag == nil or operate_flag == true) and self:IsCastingSkill() then 
		return Language.TombRaider.BeCasting
	end 
end
function TombRaiderData:IsCastingSkill()
	local check_info = self:CheckSkillInfo()
	if check_info.is_nil ~= true and self:CanStopSkill(check_info.sn) then 
		TombRaiderCtrl.Instance:SendSkillOperate({type = TombRaiderConfig.OperType.StopSkill})
		return false
	end 
	return check_info.is_nil ~= true and self.tomb_raider_cfg.skill[check_info.sn].skill_type == 1
end
-- 可不可以中断技能
function TombRaiderData:CanStopSkill(sn)
	return sn == TombRaiderEnum.UnLock or sn == TombRaiderEnum.Rescue or sn == TombRaiderEnum.TransMits or sn == TombRaiderEnum.TransMitsItem
end
--设置游戏阶段（开始
function TombRaiderData:SetGameStartState(protocol)
	-- LogError("?SetGameStartState?",protocol)
	self.game_start_state.reason_type = protocol.reason_type
	self.game_start_state.time_stamp = protocol.tamp
end 
function TombRaiderData:GetGameStartState()
	return self.game_start_state
end
--设置结算信息
function TombRaiderData:SetFBFinishInfo(protocol)
	-- LogError("?SetFBFinishInfo",protocol)
	self.finish_info.win_type = protocol.win_type --  1 盗宝胜 2 守军胜
	self.finish_info.defend_team = protocol.defend_team
	self.finish_info.attack_team = protocol.attack_team
end
-- 整合结算信息
function TombRaiderData:GetFBFinishDetailInfo()
	local mvp_member = nil
	local my_id = RoleData.Instance:GetBaseData().role_id
	local result = {attack_team = {},defend_team = {},my_point = 0}
	local function pick_info(v,type)
		local vo = {}
		vo.team_type = v.team_type
		vo.uid = v.uid
		vo.play_name = v.play_name
		vo.gongxian = v.gongxian
		vo.add_score = v.add_score

		local operate_list = type == 0 and result.defend_team or result.attack_team
		table.insert(operate_list,vo)
	end 

	-- 寻找两队中的MVP
	for k,v in pairs(self.finish_info.defend_team) do
		pick_info(v,0)
		if mvp_member == nil then 
			mvp_member = v
		elseif v.add_score > mvp_member.add_score then 
			mvp_member = v
		end 

		if my_id == v.uid then 
			result.my_point = v.add_score
		end 
    end 

	for k,v in pairs(self.finish_info.attack_team) do
		pick_info(v,1)
		if mvp_member == nil then 
			mvp_member = v
		elseif v.add_score > mvp_member.add_score then 
			mvp_member = v
		end 

		if my_id == v.uid then 
			result.my_point = v.add_score
		end
    end 

	-- 反向为赋值完成的结算信息增加mvp标记
	local operate_list = mvp_member.team_type == 0 and result.defend_team or result.attack_team
	for k,v in pairs(operate_list) do 
		if v.uid == mvp_member.uid then v.is_mvp = true end 
	end 

	table.sort(result.defend_team,function(a,b)return a.gongxian > b.gongxian end)
	table.sort(result.attack_team,function(a,b)return a.gongxian > b.gongxian end)

	return result
end
--获取自己是否胜利了
function TombRaiderData:GetFBFinishInfo()
	local cur_win_type = self.finish_info.win_type
	local my_team_type = self:CacheMyTeamInfo().team_type == 0 and 2 or 1
	return my_team_type == cur_win_type
end
-- =====================战斗阶段===========================
function TombRaiderData:CheckIsMyTeam(other_id)

	local my_id = RoleData.Instance:GetBaseData().role_id
	local team = nil 
	for k,v in pairs(self.tomb_team.attack) do 
		if my_id == v.uid then 
			team = self.tomb_team.attack
		end 
	end 

	if team == nil then 
		for k,v in pairs(self.tomb_team.defend) do 
			if my_id == v.uid then 
				team = self.tomb_team.defend
			end 
		end 
	end 

	for k,v in pairs(team) do 
		if v.uid == other_id then 
			return true
		end 
	end 
	return false
end
-- 获取我的队伍信息
function TombRaiderData:GetMemberNum()
	local num_our = 0
	local num_other = 0
	local my_info = self:CacheMyTeamInfo()
	for k,v in pairs(my_info.team_type == 0 and self.tomb_team.attack or self.tomb_team.defend) do 
		if v.stop_reason == 0 and v.uid > 0 and v.on_line > 0 then
			num_our = num_our + 1
		end 
	end 

	for k,v in pairs(my_info.team_type == 0 and self.tomb_team.defend or self.tomb_team.attack) do 
		if v.stop_reason == 0 and v.uid > 0 and v.on_line > 0 then
			num_other = num_other + 1
		end 
	end 

	-- LogError("!?@",num_our,num_other)
	return num_our,num_other
end

function TombRaiderData:CacheMyTeamInfo()
	local my_id = RoleData.Instance:GetBaseData().role_id
	for k,v in pairs(self.tomb_team.attack) do 
		if my_id == v.uid then 
			return v
		end 
	end 

	for k,v in pairs(self.tomb_team.defend) do 
		if my_id == v.uid then 
			return v
		end 
	end 
	return {team_type = 0 }
end
-- 下发技能信息
function TombRaiderData:SetSkillInfo(protocol)
	-- LogError("?SetSkillInfo?",protocol)
	self.skill_info.common_list = protocol.m_common_skill
	self.skill_info.stop_reason = protocol.stop_reason
	self.skill_info.item_skill = protocol.m_item_skill
	self.skill_info.is_flush = 1 + self.skill_info.is_flush

	if protocol.stop_reason == 1 then 
		PublicPopupCtrl.Instance:Center(Language.TombRaider.BeCatched)
	elseif protocol.stop_reason == 2 then 
		PublicPopupCtrl.Instance:Center(Language.TombRaider.BeTraped)
	end 

	-- TombRaiderBattleShow.Instance:EndLastingSkillShow()
end
-- 检查技能信息
function TombRaiderData:CheckSkillInfo()
	local skill_info = nil
	for k,v in pairs(self.skill_info.common_list) do 
		if v.guide_over_tamp > 0 or v.casting_tamp > 0 then 
			skill_info = v
		end 
	end
	if skill_info == nil then 
		if self.skill_info.item_skill ~= nil and (self.skill_info.item_skill.guide_over_tamp > 0 
		    or self.skill_info.item_skill.casting_tamp > 0) then 
				skill_info =  self.skill_info.item_skill
		end 
	end 

	local result = {}
	if skill_info == nil then 
		result.is_nil = true
		return result
	end 
	result.sn = skill_info.sn
	result.tamp = skill_info.guide_over_tamp > 0 and skill_info.guide_over_tamp or skill_info.casting_tamp
	result.moving_text = skill_info.guide_over_tamp > 0 and Language.TombRaider.Guiding or Language.TombRaider.Casting

	return result
end

-- 获取距离最近的一位玩家
function TombRaiderData:CatchNearestRole(is_enemy,limit)
	local my_info = self:CacheMyTeamInfo()
	local player_list = is_enemy and (my_info.team_type == 1 and self.tomb_team.defend or self.tomb_team.attack)
	    or (my_info.team_type == 1 and self.tomb_team.attack or self.tomb_team.defend)

	local nearest = {
		obj_id = 0,
		distance = 20000,
		target = nil,
	}
	local target = nil
	for k,v in pairs(player_list) do 

		local obj = SceneCtrl.Instance:Objs():GetCharaByUid(v.uid)

		if obj ~= nil and RoleData.Instance:GetBaseData().role_id ~= v.uid 
		    and ((is_enemy and v.stop_reason ~= 1) or(not is_enemy and v.stop_reason == 1)) then

			local obj_x, obj_y = SceneCtrl.Instance:CurScene():PosToServer(obj:GetPos())
			local main_x, main_y = SceneCtrl.Instance:CurScene():PosToServer(MainPlayer:GetPos())

		    local distance = GameMath.GetDistance(main_x, main_y, obj_x, obj_y, false)  
		    if distance < (limit*limit) and distance < nearest.distance then 
			    nearest.distance = distance
			    nearest.obj_id = obj.vo.obj_id
				nearest.target = obj
		    end 
	    end 
	end 

	-- 测试代码
	-- if DEBUG_MODE == true then 
	--     if target == nil then 
	-- 	    PublicPopupCtrl.Instance:Center("技能没有捕获到玩家")
	--     else 
	-- 	    PublicPopupCtrl.Instance:Center("对目标·"..(target ~= nil and target.vo.name or "nil").."·进行了"..
	-- 		    (is_enemy and "捕获" or "救援").." 距离为："..math.floor(math.sqrt(nearest.distance)))
	--     end 
    -- end 

	return nearest
end

function TombRaiderData:CatchNearestObj(obj_type,limit)
	local nearest = {
		obj_id = 0,
		distance = 20000,
		target = nil,
	}
	
	for k,v in pairs(self.map_obj_info.obj_list) do 
		if obj_type == v.obj_type then 
			SceneCtrl.Instance:Objs():ForeachSNPC(
				function(npc_obj)
					-- 生成物的服务端位置与地图标点的位置是完全一致的
					if npc_obj~=nil and npc_obj.vo~=nil and npc_obj.vo.x == v.pos_x and npc_obj.vo.y == v.pos_y then 
						local main_x, main_y = SceneCtrl.Instance:CurScene():PosToServer(MainPlayer:GetPos())
						local distance = GameMath.GetDistance(main_x, main_y, v.pos_x, v.pos_y, false)  
	
						if distance < (limit*limit) and distance < nearest.distance then 
							nearest.distance = distance
							nearest.obj_id = npc_obj.vo.obj_id
							nearest.target = npc_obj
						end 

						return true
					end 
					return false
				end 
			)
		end 
	end 

	-- if DEBUG_MODE == true then 
	--     if nearest.target == nil then 
	-- 	    PublicPopupCtrl.Instance:Center("你的周围没有"..(obj_type == 0 and "宝箱" or "道具"))
	--     else 
	-- 		local cfg =  NpcHelper.NpcCo(target.vo.id,false)
	-- 	    PublicPopupCtrl.Instance:Center("对目标·"..(cfg ~= nil and cfg.name or "").."·进行了"..
	-- 		(obj_type == 0 and "开箱" or "拾取").." 距离为："..math.floor(math.sqrt(nearest.distance)))
	--     end 
    -- end 

	return nearest
end 

-- 获取技能协议的参数组
function TombRaiderData:GetSkillParamt(skill_cfg)
	local param_t = {}
	param_t.type = TombRaiderConfig.OperType.UseSkill
	param_t.param1 = skill_cfg.sn
	-- 抓捕
	if skill_cfg.sn == TombRaiderEnum.Catch then 
		param_t.param2 = self:CatchNearestRole(true,skill_cfg.param2).obj_id
	end 
	-- 救援
	if skill_cfg.sn == TombRaiderEnum.Rescue then 
		param_t.param2 = self:CatchNearestRole(false,skill_cfg.param3).obj_id
	end 
	-- 开箱
	if skill_cfg.sn == TombRaiderEnum.UnLock then 
		param_t.param2 = self:CatchNearestObj(0,skill_cfg.param3).obj_id
	end 
	-- 挪移尺
	if skill_cfg.sn == TombRaiderEnum.HyperMove then 
		local paramt = self:MeasureHyperDistance(skill_cfg.param1)
		param_t.param2 = paramt.end_x
		param_t.param3 = paramt.end_z
	end 

	return param_t
end

function TombRaiderData:MeasureHyperDistance(Distance)
	-- LogError("?Distance",Distance)
	local move_dir = MainPlayer.root_node:GetEulerAngles().y * (math.pi / 180)
	local main_pos_x,main_pos_z = SceneCtrl.Instance:CurScene():PosToServer(MainPlayer:GetPos())
	local max_distance = 0
	local result = {max_distance = 0,end_x = main_pos_x,end_z = main_pos_z,distance = 0}
	for i = 1, Distance do 
		result.end_x = main_pos_x + (math.sin(move_dir) * i)
		result.end_z = main_pos_z + (math.cos(move_dir) * i)
		local check_pos = SceneCtrl.Instance:CurScene():PosToClint(result.end_x, result.end_z)
		local flag_pos = MainPlayer.nav_agent:CalculatePath(check_pos, MainPlayer.nav_mesh_path)
		if not flag_pos then 
			result.max_distance = i - 1
			break
		end 
		result.max_distance = Distance
	end 

	-- if result.max_distance == Distance then 
	--	  LogError("full brust!")
	--end 

	local check_pos = SceneCtrl.Instance:CurScene():PosToClint(result.end_x, result.end_z)
	result.distance = GameMath.GetDistance(MainPlayer:GetPos().x, MainPlayer:GetPos().z, check_pos.x, check_pos.z, true)

	return result
end

-- 技能信息扩展( 暂时无扩展，纯格式
function TombRaiderData:GetSkillInfo()
	local skill_group = {
		common = {},
		item_skill = nil,
	}
	for i = 1,3 do 
		local vo = {}
		vo.sn = self.skill_info.common_list[i].sn
		vo.cool_down_tamp = self.skill_info.common_list[i].cool_down_tamp
		vo.guide_over_tamp = self.skill_info.common_list[i].guide_over_tamp
		vo.casting_tamp = self.skill_info.common_list[i].casting_tamp
		table.insert(skill_group.common,vo)
	end 
	local item = {}
	item.sn = self.skill_info.item_skill
	item.cool_down_tamp = self.skill_info.cool_down_tamp
	item.guide_over_tamp = self.skill_info.guide_over_tamp
	item.casting_tamp = self.skill_info.casting_tamp
	skill_group.item_skill = item

	return skill_group
end 

function TombRaiderData:SetMemberPosInfo(protocol)
	-- LogError("?!?",protocol)
	self.member_pos_info.member_list = protocol.member_list
end

function TombRaiderData:SetTransmit(protocol)
	self.member_pos_info.transmit_uid = protocol.uid
end

function TombRaiderData:SetMarkInfo(protocol)
	local vo = {}
	vo.marker_uid = protocol.marker_uid
	vo.mark_type = protocol.mark_type
	vo.pos_x = protocol.mark_pos_x
	vo.pos_y = protocol.mark_pos_y
	vo.timer = 50
	table.insert( self.mark_info.list, vo)
end 

function TombRaiderData:FlushMarkInfo()
	for k,v in pairs(self.mark_info.list) do 
		v.timer = v.timer - 1
		if v.timer == 0 then 
			table.remove( self.mark_info.list, k)
		end 
	end 
end

function TombRaiderData:GetBattleCampInfo()
	local list_defend = {}
	local list_attack = {}
	for k,v in pairs(self.tomb_team.defend) do 
		if v.uid > 0 then table.insert( list_defend, v ) end 
	end 

	table.sort(list_defend,function(a,b)return a.gongxian > b.gongxian end)

	for k,v in pairs(self.tomb_team.attack) do 
		if v.uid > 0 then table.insert( list_attack, v ) end 
	end 

	table.sort(list_attack,function(a,b)return a.gongxian > b.gongxian end)

	--LogError("?",self.tomb_team)
	return list_defend,list_attack
end

function TombRaiderData:SetTransmitInfo(protocol)
	self.transmits_info.uid = protocol.uid
	self.transmits_info.end_tamp = protocol.end_tamp
	self.transmits_info.player_name = protocol.player_name
end 

function TombRaiderData:SetAchievementInfo(protocol)
	self.achievement_info.param = protocol.param
	self.achievement_info.reward_status = protocol.reward_status
	self.achievement_info.show_flag = protocol.show_flag
end

-- op_t = 0,技能；op_t = 1,拾取
-- 获取操作的有效与否
function TombRaiderData:GetOperateIneracter(oper_type,skill_cfg)
	if self.skill_info.stop_reason and self.skill_info.stop_reason > 0 then 
		if self.skill_info.stop_reason == 1 then 
		    return false,Language.TombRaider.BeCatched
		elseif self.skill_info.stop_reason == 2 then
			return false,Language.TombRaider.BeTraped
		end 
	end 
	if oper_type == 0 and skill_cfg == nil then return false end 
	-- 救人
	if oper_type == 0 and skill_cfg.sn == TombRaiderEnum.Rescue then 
		local limit = skill_cfg.param3
		local target = self:CatchNearestRole(false,limit).target
		return target ~= nil
	-- 开箱 or 拾取道具
	elseif (oper_type == 0 and skill_cfg.sn == TombRaiderEnum.UnLock) or oper_type == 1 then 
		local type = oper_type == 0 and 0 or 1
		local limit = oper_type == 0 and skill_cfg.param3 or Config.treasure_contend_auto.other[1].pick_distance
		-- 正向搜索
		local target = self:CatchNearestObj(type,limit).target
		return target ~= nil 
	end 

	return true
end

-- 逆向搜索，当发现不应存在的技能道具时直接删除
function TombRaiderData:ReverseFlushSkillItem()
	local target = nil
	SceneCtrl.Instance:Objs():ForeachSNPC(function(npc_obj)
		    if npc_obj.vo == nil then return end 
		-- 生成物的服务端位置与地图标点的位置是完全一致的
		    local clear = false
		    for k,v in pairs(self.map_obj_info.obj_list) do 
				if npc_obj.vo.x == v.pos_x and npc_obj.vo.y == v.pos_y then 					
					clear = true
				end 	
			end 
			if not clear then 
				target = npc_obj
				return true
			end 
			return false
	end)
	if target ~= nil then 
	    VoMgr:DelSNpc(target.vo.obj_id)
	end 
end

function TombRaiderData:IsTombRaider()
	-- LogError("?IsTombRaider!",SceneData.Instance:GetMode(),SCENE_TYPE.TOMB_RAIDER)
	return SceneData.Instance:GetMode() == SCENE_TYPE.TOMB_RAIDER
end

function TombRaiderData:GetTombFbName(role_id)
	for k,v in pairs(self.tomb_team.attack) do 
		if role_id == v.uid then 
			return "Loc_DaoBaoZhe_small"
		end 
	end 

	for k,v in pairs(self.tomb_team.defend) do 
		if role_id == v.uid then 
			return "Loc_ShouWeiZhe_small"
		end 
	end 
end 

-- Buff信息生成
function TombRaiderData:CacheSkillBuffInfo(skill_info)
	local result = {}
	local cfg = self.tomb_raider_cfg.skill[skill_info.sn]
	result.sn = skill_info.sn 

	if skill_info.sn == TombRaiderEnum.Boost then  --疾走
		result.temp_time = TimeHelper.GetServerTime() + cfg.param3
		result.buff_type = TombRaiderConfig.Buff.Speedup
	elseif skill_info.sn == TombRaiderEnum.Alpha then -- 神隐
		result.temp_time = TimeHelper.GetServerTime() + cfg.param2
		result.buff_type = TombRaiderConfig.Buff.Alpha
	elseif skill_info.sn == TombRaiderEnum.BurstBoost then -- 轻身符
		if skill_info.is_debuff then 
			result.temp_time = TimeHelper.GetServerTime() + cfg.param4
			result.buff_type = TombRaiderConfig.Buff.SpeedDown
		else 
			result.temp_time = TimeHelper.GetServerTime() + cfg.param3
			result.buff_type = TombRaiderConfig.Buff.Speedup
		end 
	elseif skill_info.sn == TombRaiderEnum.AlphaItem then -- 隐身衣
		result.temp_time = TimeHelper.GetServerTime() + cfg.param2
		result.buff_type = TombRaiderConfig.Buff.Alpha
	end 

	result.desc = string.format( Language.TombRaider.BuffDesc,Language.TombRaider.BuffName[result.buff_type]) 

	return result
end

-- ======================= 地图信息 =========================
function TombRaiderData:SetMapObjInfo(protocol)
	-- LogError("?SetMapObjInfo?",protocol)
	self.map_obj_info.obj_list = protocol.obj_list
	local num = 0
	local done_num = 0
	for k,v in pairs(self.map_obj_info.obj_list) do 
		if v.obj_type == 0 and v.status == 0 then 
			num = num + 1
			-- LogError("map_obj",v)
			-- LogError("map _num ",num)
		end 
		if v.obj_type == 0 and v.status == 1 then 
			done_num = done_num + 1
		end 
	end 

	-- LogDG("?!?@#?#",num,done_num)
	self.map_obj_info.cur_box_num = num
	self.map_obj_info.done_box_num = done_num
end

function TombRaiderData:GetWinBoxNum()
	-- LogDG("?self.map_obj_info.!",self.map_obj_info.done_box_num)
	return Config.treasure_contend_auto.other[1].win_box_num - self.map_obj_info.done_box_num
end

function TombRaiderData:GetMapMiniIconList()
	local map_type = TombRaiderConfig.BattleMapIconType
	local list = {}
	for k,v in pairs(self.map_obj_info.obj_list) do 
		if v.obj_type == 0 then 
			local vo = {}
			vo.type = v.status == 0 and map_type.Box or map_type.Ruins
			vo.c_pos = SceneCtrl.Instance:CurScene():PosToClint(v.pos_x, v.pos_y)
			table.insert( list, vo )
		-- elseif v.obj_type == 1 and DEBUG_MODE == true then -- 要求被去除
		-- 	-- 地图技能图标（测试用
		-- 	local vo = {}
        --     -- 改这里可以把测试标记改成该标记
		-- 	vo.type = (v.param1 == 14 or v.param1 == 13) and map_type.Flag or map_type.Death
		-- 	vo.c_pos = SceneCtrl.Instance:CurScene():PosToClint(v.pos_x, v.pos_y)
		-- 	table.insert( list, vo )
		end 
	end

	for k,v in pairs(self.mark_info.list) do 
		local vo = {}
		vo.type = v.mark_type
		vo.is_mark = true
		vo.c_pos = Vector2.New(v.pos_x,v.pos_y)
		table.insert(list ,vo)
	end 


	for k,v in pairs(self.member_pos_info.member_list) do
		local vo = {}
		local my_id = RoleData.Instance:GetBaseData().role_id
		if self:CheckIsMyTeam(v.uid) and v.uid ~= my_id then 
		    vo.type = TombRaiderConfig.BattleMapIconType.Member
		    vo.c_pos = SceneCtrl.Instance:CurScene():PosToClint(v.pos_x, v.pos_y)
		    table.insert( list, vo )
		end 
	end 

	return list

end

-- 测试代码·检查这里是不是有个东西
function TombRaiderData:CheckBoxHere()
	for k,v in pairs(self.map_obj_info.obj_list) do 
		if SceneCtrl.Instance:Objs():ForeachSNPC(
			function(npc_obj)
                if npc_obj.vo.x == v.pos_x and npc_obj.vo.y == v.pos_y then
					-- if npc_obj.vo.obj_id ~= nil and  DEBUG_MODE == true then 
					--     PublicPopupCtrl.Instance:Center("发现了坐标相同场景物品 "..npc_obj.vo.obj_id)
					-- end 
                    return true
                end
                return false
            end
		) then 
			break
		end 
	end 

end


-- ======================= 引导信息 =========================
-- 引导功能组
-- BoxGuide 箱子总数
-- GuardGuide 守卫者人数
-- RaiderGuide 盗宝者人数
-- PointGuide 分数
-- JoyGuide 摇杆
-- ItemSkillGuide 技能物品
-- Skill1Guide 技能1
-- Skill2Guide 技能2
-- MainSkillGuide 主技能
-- PickGuide 拾取按钮
-- SmallMapGuide 小地图
-- MapShowGuide 地图宝箱

function TombRaiderData:GetGuideList()
	local special_list = {}
	local simple_list = {}
	for k,v in pairs(Config.treasure_contend_auto.guide) do 
		if v.is_special == 1 then 
			table.insert(special_list,v)
		else 
			-- 注意 通常指引最大只支持四个
			if simple_list[v.index] == nil then simple_list[v.index] = {} end 
			table.insert(simple_list[v.index],v)
		end 
	end 

	return simple_list,special_list
end 

-- 
function TombRaiderData:GetIsNeedGuide()
	if self.achievement_info.show_flag == 0 and ActivityData.IsOpen(ActType.TombRaider) then 
		TombRaiderCtrl.Instance:SendMatchOperate(5)
	end 
	return self.achievement_info.show_flag == 0
end