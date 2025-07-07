

TombRaiderBattleShow = TombRaiderBattleShow or BaseClass()
function TombRaiderBattleShow:__init()
    if TombRaiderBattleShow.Instance ~= nil then
		Debuger.LogError("[TombRaiderBattleShow] attempt to create singleton twice!")
		return
	end
	TombRaiderBattleShow.Instance = self

	self.tr_data = TombRaiderData.Instance
	self.tomb_raider_cfg = Config.treasure_contend_auto

	-- self:init_config()

	self.observe_effect_list = {}
	self.my_effect_list = {}
	self.my_effect_timer = {}
	self.tr_data.tomb_matching = false
end 

-- =====================注册特效======================
function TombRaiderBattleShow:init_config()
    for k, v in pairs(TombEffectConfig) do
        if v.resident == true then
            GameObjPool:RegisterPool(
                v.path,
                function(error)
                end,
                false
            )
        end
    end
end

-- =====================数据存取===========================
-- 读配置
-- 获取阵营技能配置
--0 是守卫者 1是盗宝者
function TombRaiderBattleShow:GetTeamSkillGroup()
	local team = {}
	local data = self.tr_data:CacheMyTeamInfo()
	local team_type = data.team_type or 0
	for k,v in pairs(TombRaiderConfig.SkillGroup[team_type]) do 
		local vo = {}
		vo.cfg = self.tomb_raider_cfg.skill[v]
		vo.index = k
		vo.team_type = team_type 
		table.insert(team,vo)
	end 

	return team
end

-- 获取物品技能配置
function TombRaiderBattleShow:GetItemSkillData()
	local tr_skill_info = self.tr_data.skill_info
	local skill_info = {}
	if tr_skill_info.item_skill == nil or tr_skill_info.item_skill.sn == 0 then 
		return {cfg = {sn = -1}} 
	end 
	skill_info.cfg = self.tomb_raider_cfg.skill[tr_skill_info.item_skill.sn]
	return skill_info
end

-- 特效标
function TombRaiderBattleShow:EffectInfoHandle(protocol)
	-- LogError("?<color=#" ..COLORSTR.Red8 .. ">EffectInfoHandle!</color>?",protocol)
	local obj = SceneCtrl.Instance:Objs():GetCharaByUid(protocol.uid)
	local vo = SceneCtrl.Instance:Objs():GetCharaVoByUid(protocol.uid)
	-- LogError("?mian?", RoleData.Instance:GetRoleId() == protocol.uid,protocol.special_effect_type ~= TombRaiderConfig.RoleSpecialEffectType.Type_Flash )
	local is_main = RoleData.Instance:GetRoleId() == protocol.uid and protocol.special_effect_type ~= TombRaiderConfig.RoleSpecialEffectType.Type_Flash 
	local is_catched = protocol.special_effect_type == TombRaiderConfig.RoleSpecialEffectType.Type_Catched

	-- 加入判断，如果是捕捉特效的话，没有obj也可以放行
	if obj == nil and not is_main and not is_catched then 
		-- LogError("?return !!?", obj == nil , not is_main )
		return 
	elseif obj == nil and vo == nil and not is_main and is_catched then 
		return 
	end 
	local observe_effect = TombRaiderConfig.SkillObserveEffectConfig[protocol.special_effect_type]
	if protocol.special_effect_type == 0 then self:EndOtherLastingSkillShow(protocol.uid) end 
	if observe_effect == nil then return end 

	local operate_obj = is_main and MainPlayer or obj
	if observe_effect.is_special == true then 
		if protocol.special_effect_type == TombRaiderConfig.RoleSpecialEffectType.Type_UnLock then 
			-- 备注：各个模型其实并非四肢健全，故只在持械手上生成特效
			local body = operate_obj.body
			-- LogError("fyubc?!",body.l_weapon_res_id~= nil ,body.l_weapon_res_id)
			if body.l_weapon_res_id and body.l_weapon_res_id > 0 then 
				local type,parent = DrawerHelper.GetWeaponTypeAndKey(body.l_weapon_res_id)
				-- LogError("show!left!",type,parent)
				if operate_obj.body.main_body.body_com ~= nil then 
				    local hanlde = SE:Play(observe_effect.eff, nil, nil, operate_obj.body.main_body.body_com[parent])
				    if self.observe_effect_list[protocol.uid] == nil then  self.observe_effect_list[protocol.uid] = {} end 
				    table.insert(self.observe_effect_list[protocol.uid], hanlde)
				end 
			end 
			-- LogError("fyubc?!",body.r_weapon_res_id~= nil ,body.r_weapon_res_id)
			if body.r_weapon_res_id and body.r_weapon_res_id > 0 then 
				local type,parent = DrawerHelper.GetWeaponTypeAndKey(body.r_weapon_res_id)
				-- LogError("show!right!",type,parent)
				if operate_obj.body.main_body.body_com ~= nil then 
				    local hanlde = SE:Play(observe_effect.eff, nil, nil, operate_obj.body.main_body.body_com[parent])
				    if self.observe_effect_list[protocol.uid] == nil then  self.observe_effect_list[protocol.uid] = {} end 
				    table.insert(self.observe_effect_list[protocol.uid], hanlde)
				end 
			end 

			-- SE:Play(observe_effect.eff, nil, nil, operate_obj.body.main_body.body_com.HandL1)
			-- SE:Play(observe_effect.eff, nil, nil, operate_obj.body.main_body.body_com.HandR1)
		elseif protocol.special_effect_type == TombRaiderConfig.RoleSpecialEffectType.Type_Catched then 
			local pos_change = vo ~= nil and SceneCtrl.Instance:CurScene():PosToClint(vo.x, vo.y) or nil
			local operate_node = operate_obj ~= nil and operate_obj.root_node or nil
			local hanlde = operate_obj == nil and (pos_change ~= nil and SE:Play(observe_effect.eff, pos_change, nil, nil) or nil) 
			    or SE:Play(observe_effect.eff, nil, nil, operate_node)
			if self.observe_effect_list[protocol.uid] == nil then  self.observe_effect_list[protocol.uid] = {} end 
			if hanlde ~= nil then table.insert(self.observe_effect_list[protocol.uid], hanlde) end 
		end 
	else 
		-- LogError("?shwo !",observe_effect.is_follow,observe_effect.eff)
		local hanlde = nil
		if observe_effect.is_follow then 
			hanlde = SE:Play(observe_effect.eff, nil, nil, operate_obj.root_node)
	    else 
			local pos = operate_obj:GetPos()
			hanlde = SE:Play(observe_effect.eff,  pos)
		end 
		if observe_effect.self_complete == nil then 
		    if self.observe_effect_list[protocol.uid] == nil then  self.observe_effect_list[protocol.uid] = {} end 
		    table.insert(self.observe_effect_list[protocol.uid], hanlde)
		end 
	end 
end

-- 主动技能演出
function TombRaiderBattleShow:MainSkillShow(data)
	-- LogError("?MainSkillShow!")
	local param_t = self.tr_data:GetSkillParamt(data.cfg)
	-- LogError("?fuvck1",param_t)
	if self:MainOtherSkillShow(data,param_t) and self:CheckWithTarget(param_t) then 
	    TombRaiderCtrl.Instance:SendSkillOperate(param_t)
	end 
end

-- 其他特效演出
function TombRaiderBattleShow:MainOtherSkillShow(data,param)
	if data.cfg.sn == TombRaiderEnum.HyperMove then 
		local hype_data = self.tr_data:MeasureHyperDistance(data.cfg.param1)
		MainPlayer:HyperMove(hype_data.distance,function ()
			TombRaiderCtrl.Instance:SendSkillOperate(param)
		end)
	    return false
	end 

	-- 传送系技能止步
	if (data.cfg.sn == TombRaiderEnum.TransMits or data.cfg.sn == TombRaiderEnum.TransMitsItem) and MainPlayer ~= nil then 
		MainPlayer:StopMove()
	end 
	return true
end

function TombRaiderBattleShow:CheckWithTarget( param_t )
	if param_t.param1 == TombRaiderEnum.Rescue and param_t.param2 == 0 then 
		PublicPopupCtrl.Instance:Center(Language.TombRaider.NeedSaveTarget)
		return false
	elseif param_t.param1 == TombRaiderEnum.UnLock and param_t.param2 == 0 then 
		PublicPopupCtrl.Instance:Center(Language.TombRaider.NeedTarget)
		return false
	end 
	return true
end

function TombRaiderBattleShow:CheckWithTargetForPop(param_t)
	if param_t.param1 == TombRaiderEnum.Catch and param_t.param2 == 0 then 
		return false
	end 
	return true
end

-- 结束 我 的持续性特效演出
function TombRaiderBattleShow:EndLastingSkillShow()
	local skill_info = self.tr_data:CheckSkillInfo()
	if skill_info.is_nil == true then return end 
	if self.my_effect_list[skill_info.sn] == nil then return end 

	-- LogError("!!!!!success enter?!!!!!")
	self.my_effect_timer[skill_info.sn] = TimeHelper:AddDelayTimer(function()
    	for k,v in pairs(self.my_effect_list[skill_info.sn])do 
	    	SE:Stop(v)
	    end 
    end,skill_info.tamp - TimeHelper.GetServerTime())
end

-- 结束 他 的持续性特效演出
function TombRaiderBattleShow:EndOtherLastingSkillShow(uid)
	-- LogError("!?@!!!",uid)
	if self.observe_effect_list[uid] == nil then return end 
	
	for k,v in pairs(self.observe_effect_list[uid]) do 
		SE:Stop(v)
	end 
end

function TombRaiderBattleShow:EndOtherLastingSkillShowByObjId(obj_id)
	if not self.tr_data:InTombRaider() then return end 
	local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(obj_id)
	if obj == nil then return end 
	self:EndOtherLastingSkillShow(obj.vo.role_id)
end

-- 离开场景时专用
function TombRaiderBattleShow:DeleteAllEffect()
	for k,v in pairs(self.observe_effect_list) do 
		for i,j in pairs(v) do 
			SE:Stop(j)
		end 
	end 

	for k,v in pairs(self.my_effect_list) do 
		SE:Stop(v)
	end 
end