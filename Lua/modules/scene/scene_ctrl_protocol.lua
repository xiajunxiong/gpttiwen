
function SceneCtrl:registerProcs()
	self:RegisterProtocol(CSObjMove)		--请求移动
	self:RegisterProtocol(CSTransportReq)	--请求进入传送门


	self:RegisterProtocol(SCEnterScene,"RecvEnterScene")		--进入场景

	self:RegisterProtocol(SCRoleSpeedChangeNotice,"OnRoleSpeedChangeNotice")		--obj移速改变通知
	self:RegisterProtocol(SCTitleChange,"OnTitleChange")		--obj移速改变通知

	self:RegisterProtocol(SCObjMove,"RecvObjMove")		--对象移动返回

	self:RegisterProtocol(SCResetPost,"RecvResetPost")		-- 强设主角位置
	self:RegisterProtocol(SCChangeSceneNotice,"OnChangeSceneNotice")		-- 强设主角位置
	self:RegisterProtocol(CSAutoEscapeFromCurrentPosition)		-- 强设主角位置

	self:RegisterProtocol(SCSceneObjList,"OnSceneObjList")				-- 场景中obj位置列表

	self:RegisterProtocol(SCVisibleObjEnterRole,"RecvVisibleObjEnterRole")		-- 显示其他角色
	self:RegisterProtocol(SCVisibleObjLeave,"RecvVisibleObjLeave")		-- 其他角色离开视线
	self:RegisterProtocol(SCVisibleObjEnterNPCMonster,"RecvVisibleObjEnterNPCMonster")	--	NPC怪物
	self:RegisterProtocol(SCVisibleObjEnterNPCGathers,"RecvVisibleObjEnterNPCGathers")	--	NPC怪物
	self:RegisterProtocol(SCNPCStartToGather, "RecvNPCStartToGather")		--	下发采集时间
	self:RegisterProtocol(SCNPCStopGathering, "RecvNPCStopGathering", true)		--	取消采集行为
	self:RegisterProtocol(SCNPCGathersNotify, "RecvNPCGathersNotify")		--	采集广播(通知周围玩家)


	self:RegisterProtocol(SCRoleStatusChangeNotice, "RecvRoleStatusChangeNotice")		--	人物状态

	
	self:RegisterProtocol(CSFlyByShoe)		--使用飞天神靴
	self:RegisterProtocol(CSNPCHandleRequest)		--npc交互操作请求
	self:RegisterProtocol(CSEnterFB)		--请求进副本
	self:RegisterProtocol(CSLeaveFB)		--请求离开副本

	self:RegisterProtocol(CSSpecialTransportReq)	--特别逻辑传送
	self:RegisterProtocol(SCVisibleObjEnterSpecialTransport, "OnVisibleObjEnterSpecialTransport")		--	特别逻辑传送点

	self:RegisterProtocol(CSTreasureChestReq)														-- 宝箱请求
	self:RegisterProtocol(SCVisibleObjEnterTreasureChest, "OnVisibleObjEnterTreasureChest")			-- 宝箱进入视野
	self:RegisterProtocol(SCNoticeNearbyOnOpenTreasureChest, "OnNoticeNearbyOnOpenTreasureChest")	-- 宝箱通知 通知附近的玩家 宝箱被打开
	self:RegisterProtocol(SCTreasureChestRewardList, "OnTreasureChestRewardList") 					-- --开启箱子获得的物品奖励信息，主要用于判断是否要在场景的箱子上显示获得物品的模型
	self:RegisterProtocol(SCVisibleObjEnterNPCFuncNpc, "OnVisibleObjEnterNPCFuncNpc")	-- 功能性NPC进入视野
	self:RegisterProtocol(SCNPCInfoNotify, "OnNPCInfoNotify")	-- 功能性NPCINFO通知

	self:RegisterProtocol(SCChangeAppearanceRole, "OnChangeAppearanceRole")	-- 形象变更
	self:RegisterProtocol(SCPetFollowStatus, "OnPetFollowStatus")	-- 宠物跟随进入视野

	self:RegisterProtocol(SCUpdateFollowList, "OnUpdateFollowList")		-- 更新跟随列表(更换队长时下发)
	self:RegisterProtocol(SCAddFollow, "OnAddFollow")					-- 增加跟随者(新入队或归队时下发)
	self:RegisterProtocol(SCRemoveFollow, "OnRemoveFollow")				-- 删除跟随者(离队或暂离时下发)

	self:RegisterProtocol(SCNPCCommitBack, "OnNPCCommitBack")				-- NPC交互返回
	self:RegisterProtocol(SCNPCShopInfo, "OnNPCShopInfo")					-- NPC商店返回
	self:RegisterProtocol(CSNPCShowBuyReq)									-- 向NPC购买请求

	self:RegisterProtocol(SCJoinTeamNotifyArea, "OnJoinTeamNotifyArea")				-- 进队
	self:RegisterProtocol(SCLeaveTeamNotifyArea, "OnLeaveTeamNotifyArea")			-- 离队
	self:RegisterProtocol(SCTeamRedNameNotifyArea, "OnTeamRedNameNotifyArea")

	--[[ self:RegisterProtocol(SCSpecialAppearanceChangeNotice, "OnSpecialAppearanceChangeNotice") ]]			-- 形象变更
	self:RegisterProtocol(SCMoveAssistReq, "OnSCMoveAssistReq")
	self:RegisterProtocol(CSMoveAssistRet)
	--巡游花车进入视野
	self:RegisterProtocol(SCVisibleObjEnterParadeFloat, "OnVisibleObjEnterParadeFloat")
	--天下至尊令 雕像 进入视野
	self:RegisterProtocol(SCVisibleObjEnterPeakTokenObj, "OnVisibleObjEnterPeakTokenObj")

	self:RegisterProtocol(SCROleDaWanKaInfoChange, "OnSCROleDaWanKaInfoChange")
end

function SceneCtrl:OnPetFollowStatus(protocol)
    local objVo = self:VoMgr():RoleVoList()[protocol.obj_id]
    if objVo then
        objVo.follow_pet = TableCopy(protocol)
    end
    local obj = self:Objs():GetObjectByObjId(protocol.obj_id)
    if obj then
        obj:FlushFollow()
    end
end

function SceneCtrl:unregisterProcs()
	self:UnRegisterProtocol(SCEnterScene)
	self:UnRegisterProtocol(SCNPCStartToGather)
	self:UnRegisterProtocol(SCVisibleObjEnterSpecialTransport)
	self:UnRegisterProtocol(SCVisibleObjEnterTreasureChest)
	self:UnRegisterProtocol(SCNoticeNearbyOnOpenTreasureChest)
	self:UnRegisterProtocol(SCVisibleObjEnterNPCFuncNpc)
end

--------------Server to client---------------------

function SceneCtrl:RecvEnterScene(protocol)
	SceneCtrl.Instance.is_first_enter = protocol.is_first_enter
	self.data:SetRoleObjId(protocol.obj_id)
	RoleData.Instance:GetBaseData().obj_id = protocol.obj_id
	if MainPlayer then
		MainPlayer:FlushRootObjName()	--进场景后更新了主角的obj_id，需要更新Hierarchy里的显示
	end
	local old_sceneid = self.data:SceneId()
	if old_sceneid ~= protocol.sceneid then
		self.data:SetSceneId(protocol.sceneid)
	end
	if BattleData.Instance:BaseData().server_fighting == false then
		if old_sceneid > 0 then
			self:GotoScene(GameScene,false, old_sceneid)
		else
			self:GotoScene(GameScene,true)
		end
	end
	if MainPlayer then
		MainPlayer:Vo().scene_id = protocol.sceneid
		MainPlayer:SetScenePos(protocol.sceneid,protocol.pos_x,protocol.pos_y,true)
	end

	MainProber:Step(MainProber.STEP_ENTER_REAL_SCENE_END, MainProber.user_id, MainProber.server_id, MainProber.role_name, MainProber.role_id)
end

function SceneCtrl:OnRoleSpeedChangeNotice(protocol)
	if SceneData.Instance:GetRoleObjId() == protocol.obj_id then
		local base_data = RoleData.Instance:GetBaseData()
		base_data.move_speed = MAIN_PLAY_BASE_MOVE_SPEED * (100 + protocol.move_speed) / 100
		if MainPlayer then
			MainPlayer:SetMoveSpeed(base_data.move_speed)
		end
	end
	local obj = self:Objs().objs[protocol.obj_id]
	if obj then
		obj.vo.move_speed = OBJ_BASE_MOVE_SPEED * (100 + protocol.move_speed) / 100
		obj:SetMoveSpeed(obj.vo.move_speed)
	end
end
function SceneCtrl:OnTitleChange(protocol)
	if SceneData.Instance:GetRoleObjId() == protocol.obj_id then
		local base_data = RoleData.Instance:GetBaseData()
		base_data.title_id = protocol.title_id
		base_data.facescroe_id = protocol.facescroe_id
	end
	local obj = self:Objs().objs[protocol.obj_id]
	if obj then
		obj.vo.title_id = protocol.title_id
		obj.vo.facescroe_id = protocol.facescroe_id
	end
end

--服务端发的是走向而不是最终位置
function SceneCtrl:RecvObjMove(protocol)
	if MainViewData.Instance:BaseData().is_battle == true then
		return
	end

	local obj = self:Objs():GetObjectByObjId(protocol.obj_id)
	local role_obj_id = SceneData.Instance:GetRoleObjId()
	if protocol.obj_id == role_obj_id then
		-- if TeamData.Instance:NeedFollow() then
		-- 	obj = MainPlayer
		-- end
		if protocol.distance == 0 and MainPlayer then
			MainPlayer:StopMove(true)
		end
		return
	end

	if obj then

		if obj.vo.is_rest == true and obj.vo.obj_type == SceneObjType.RabbitRace then
			if protocol.distance == 0 then
				obj:StopMove()
				--local pos = obj:GetPos()
				obj:SetVoPos(protocol.pos_x,  protocol.pos_y)
				return
			end
		end

		if protocol.mark == 4 then 
			obj:HyperMove(protocol.pos_x,protocol.pos_y,protocol.dir_y)
			return 
		end 
		-- if self.follow_data:IsFollowHead(protocol.obj_id) then
		-- 	self.follow_data:Follow(protocol, self:Objs().net_roles)
		-- 	return
		-- end
		local end_x = protocol.pos_x
		local end_y = protocol.pos_y
		local distance = protocol.distance
		if distance ~= 0 then
			end_x = end_x + (math.cos(protocol.dir_y) * distance)
			end_y = end_y + (math.sin(protocol.dir_y) * distance)
		end


		if self:CurScene():LoadingComplete() and self:CurScene():GetClassType() == GameScene and distance ~= 0 then
			if ChsFollow.Instance:IsInQueue(obj) == false or 
				ChsFollow.Instance:IsQueueHead(obj) == true then	--只有队伍头或者不在队列中才会应用移动
				local pos = self:CurScene():PosToClint(end_x, end_y)
				obj:MoveTo(pos, false)
			end
		end

		obj:SetVoPos(end_x, end_y)	--vo数据随时都可以进行设置，与前端场景状态无关
	end
end

function SceneCtrl:RecvResetPost(protocol)
	--SceneData.Instance:SceneId()
	if MainPlayer then
		MainPlayer:SetScenePos(protocol.scene_id, protocol.x, protocol.y, true)
	end
	-- MainPlayer:PlayAnim(ChAnimType.Idle)
end

function SceneCtrl:EscapeFromCurrentPosition()
	local protocol = GetProtocol(CSAutoEscapeFromCurrentPosition)
	SendProtocol(protocol)
end

function SceneCtrl:OnSceneObjList(protocol)
	if SceneObjNotifyReason.Add == protocol.notify_reason or SceneObjNotifyReason.Normal == protocol.notify_reason then
		for obj_id, info in pairs(protocol.obj_info_list) do
			VoMgr:AddSNpcPos(info)
		end
	elseif SceneObjNotifyReason.Delete == protocol.notify_reason then
		for obj_id, info in pairs(protocol.obj_info_list) do
			VoMgr:DelSNpcPos(obj_id)
		end
	end
end

function SceneCtrl:RecvVisibleObjEnterRole(protocol)
	local vo = CommonStruct.RoleVo(protocol)
	if FishData.Instance:InFishMap(vo.scene_id) then
		vo = CommonStruct.FishmanVo(vo)
	elseif SceneData.Instance:SceneId() == MarryData.Instance:ConfigOtherSceneId() then
		if MarryData.Instance:GetWeddingIsHostByUid(vo.role_id) then
			vo = CommonStruct.NoWeaponNoRideWeddingVo(vo)
		else
			vo = CommonStruct.NoWeaponNoRideVo(vo)
		end
	elseif TombRaiderData.Instance:InTombRaider() or ShieldRideScene[vo.scene_id] then 
		vo = CommonStruct.TombRaiderVo(vo)
	end
	VoMgr:AddRole(vo)
	if protocol.pet_id > 0 then
		local info = {}
		info.obj_id = protocol.obj_id
		info.pet_id = protocol.pet_id
		info.pet_skin = protocol.skin_id
		info.name = protocol.pet_name
		info.fly_flag = protocol.pet_fly_flag
		info.rank = protocol.rank or -1
		self:OnPetFollowStatus(info)
	end
end
function SceneCtrl:RecvVisibleObjLeave(protocol)
	TombRaiderBattleShow.Instance:EndOtherLastingSkillShowByObjId(protocol.obj_id)
	VoMgr:DelRole(protocol.obj_id)
	VoMgr:DelMonster(protocol.obj_id)
	VoMgr:DelDoor(protocol.obj_id)
	VoMgr:DelSNpc(protocol.obj_id)
	-- self.follow_data:CheckFollowHeadById(protocol.obj_id)
end

function SceneCtrl:RecvVisibleObjEnterNPCMonster(protocol)
	local vo = CommonStruct.MonsterVo(protocol)
	if Cfg.NpcById(protocol.npc_id) then
		vo.name = Cfg.NpcById(protocol.npc_id).name
	end
	VoMgr:AddMonster(vo)
end
function SceneCtrl:RecvVisibleObjEnterNPCGathers(protocol)
	local vo = CommonStruct.NpcVo(protocol)
	VoMgr:AddSNpc(vo)
end

function SceneCtrl:RecvNPCStartToGather(protocol)
	if protocol.need_time > 0 then
		GatherData.Instance:SetGatherTime(protocol.need_time)
		ViewMgr:OpenView(GatherView)
	end
end
function SceneCtrl:RecvNPCStopGathering(protocol)
	local role_obj_id = SceneData.Instance:GetRoleObjId()
	if role_obj_id == protocol.role_obj_id then
		GatherData.Instance:CacheReason(protocol.reasons)
		if protocol.reasons == 0 then
			MainOtherCtrl.Instance:CloseGather()
		end
	else
		local obj = self:Objs():GetObjectByObjId(protocol.role_obj_id)
		if obj then
			obj:PlayAnim(ChAnimType.Idle)
		end
	end
end
function SceneCtrl:RecvNPCGathersNotify(protocol)
	local role_obj_id = SceneData.Instance:GetRoleObjId()
	if role_obj_id == protocol.role_obj_id then return end
	--找到采集物并look at
	local obj = self:Objs():GetObjectByObjId(protocol.role_obj_id)
	local vo = VoMgr.snpc_vo_list2[protocol.gather_seq]
	if obj and vo then
		local gather = self.objs:GetNetNpc(vo)
		if gather then
			obj:LookAt(gather)
		end
		local npc_types = NpcHelper.NpcTypesById(protocol.npc_id)
		obj:StopMove()
		if npc_types then
			if npc_types.mineral == true then
				obj:PlayAnim(ChAnimType.WaKuang)
			elseif npc_types.wood == true then
				obj:PlayAnim(ChAnimType.FaMu)
			else
				obj:PlayAnim(ChAnimType.Gather)
			end
		else
			obj:PlayAnim(ChAnimType.Gather)
		end
	end
end
function SceneCtrl:RecvRoleStatusChangeNotice(protocol)
	--[[ if MainViewData.Instance:BaseData().is_battle == true then
		return
	end ]]
	local role_obj_id = SceneData.Instance:GetRoleObjId()
	if protocol.obj_id == role_obj_id then
		RoleData.Instance:GetBaseData().behavior_state = protocol.behavior_state
		RoleData.Instance:GetBaseData().move_state = protocol.cur_move_status
		if MainPlayer then
			MainPlayer:FlushRoleStatus(protocol.cur_move_status )
		end
	else
		local obj = self:Objs():GetObjectByObjId(protocol.obj_id)
		if obj then
			VoMgr:ChangeRoleStatus(protocol.obj_id, protocol.behavior_state)
			VoMgr:ChangeRoleMoveStatus(protocol.obj_id, protocol.cur_move_status)
		end
	end
end
--------------Client to server---------------------

function SceneCtrl:MainChRequestMove(cur_pos,target_pos, not_touch_sync)
	if self:CurScene():GetClassType() ~= GameScene then return end
	local protocol = GetProtocol(CSObjMove)
	protocol.pos_x,protocol.pos_y = self:CurScene():PosToServer(cur_pos)
	protocol.target_pos_x,protocol.target_pos_y = self:CurScene():PosToServer(target_pos)
	if not_touch_sync and protocol.pos_x == protocol.target_pos_x and protocol.pos_y == protocol.target_pos_y then
		return
	end
	protocol.scene_id = SceneData.Instance:SceneId()--当前场景id
	SendProtocol(protocol)
end

function SceneCtrl:SendMainChRequestMove(pos_x,pos_y,target_pos_x,target_pos_y)
	if self:CurScene():GetClassType() ~= GameScene then return end
	local protocol = GetProtocol(CSObjMove)
	protocol.pos_x = pos_x or 0
	protocol.pos_y = pos_y or 0
	protocol.target_pos_x = target_pos_x or 0
	protocol.target_pos_y = target_pos_y or 0
	protocol.scene_id = SceneData.Instance:SceneId()
	SendProtocol(protocol)
end

function SceneCtrl:EnterDoorReq(door_id)
	local protocol = GetProtocol(CSTransportReq)
	protocol.door_id = door_id
	SendProtocol(protocol)
	AudioMgr:PlayEffect(AudioType.Scene, "enter_door")
end

function SceneCtrl:SendFlyByShoe(param_t)
	if param_t.pos_x == nil or param_t.pos_y == nil then
		BuglyReport("传送坐标有误，传送场景id = ", param_t and param_t.scene_id or 0)
		return
	end
	local protocol = GetProtocol(CSFlyByShoe)
	protocol.scene_id = param_t.scene_id
	protocol.pos_x = param_t.pos_x
	protocol.pos_y = param_t.pos_y
	SendProtocol(protocol)
end

function SceneCtrl:RequestGroupIdBattle(npc_seq,group_id,param2)
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
	local protocol = GetProtocol(CSNPCHandleRequest)
	protocol.op_type = 0
	protocol.npc_seq = npc_seq or 0
	protocol.param1 = group_id or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

-- 不需要npc_seq直接怪物组战斗
function SceneCtrl:RequestGroupIdBattleNoNS(group_id)
	local protocol = GetProtocol(CSNPCHandleRequest)
	protocol.op_type = 5
	protocol.param1 = group_id or 0
	SendProtocol(protocol)
end

function SceneCtrl.RequestGather(npc_seq,param1,param2)
	local protocol = GetProtocol(CSNPCHandleRequest)
	protocol.op_type = 2
	protocol.npc_seq = npc_seq
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function SceneCtrl.RequestGatherFinsh(npc_seq,param1,param2)
	local protocol = GetProtocol(CSNPCHandleRequest)
	protocol.op_type = 3
	protocol.npc_seq = npc_seq or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function SceneCtrl.RequestTalk(obj_id, param1)
	local protocol = GetProtocol(CSNPCHandleRequest)
	protocol.op_type = 4
	protocol.obj_id = obj_id or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function SceneCtrl:OnNPCCommitBack(protocol)
	local param_t = {}
	param_t.talk_id = protocol.talk_id
	param_t.beh_btn_list = {}
	for key, info in pairs(protocol.option_list) do
		if 0 ~= info.button_id then
			local co = Cfg.NpcButtonListByButtonId(info.button_id)
			table.insert(param_t.beh_btn_list, NpcDialogBehBtnDataFuncExe.New(co and co.show_word or "", function ()
				SceneCtrl.RequestTalk(protocol.obj_id, key)
			end, true))
		end
	end
	NpcDialogCtrl.Instance:NpcDialogView(param_t)
end

function SceneCtrl:OnNPCShopInfo(protocol)
	FightData.Instance:SetNPCShopInfo(protocol)
	ViewMgr:OpenView(FightShopView)
end

function SceneCtrl:SendNPCShowBuyReq(obj_id, buy_index, buy_num)
	local protocol = GetProtocol(CSNPCShowBuyReq)
	protocol.obj_id = obj_id or 0
	protocol.buy_index = buy_index or 0
	protocol.buy_num = buy_num or 0
	SendProtocol(protocol)
end

function SceneCtrl:RequestEnterFb(fb_type, param_1,param_2)
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
	if FightCtrl.IsActFB() then
		return
	end
	SceneLogic.Instance:ClearMoveCache()
	local protocol = GetProtocol(CSEnterFB)
	protocol.fb_type = fb_type or 0
	protocol.param_1 = StringIsEmpty(param_1) and 0 or param_1
	protocol.param_2 = StringIsEmpty(param_2) and 0 or param_2
	SendProtocol(protocol)
end

function SceneCtrl:RequestLeaveFb()
	local protocol = GetProtocol(CSLeaveFB)
	SendProtocol(protocol)	
end


function SceneCtrl:SendSpecialTransportReq(obj_id)
	if not RushDungeonData.Instance:CanEnterDoor() then
		return
	end
	local protocol = GetProtocol(CSSpecialTransportReq)
	protocol.obj_id = obj_id or 0
	SendProtocol(protocol)
	AudioMgr:PlayEffect(AudioType.Scene, "enter_door")
end

function SceneCtrl:OnVisibleObjEnterSpecialTransport(protocol)
	local vo = CommonStruct.NpcVo(protocol)
	vo.name = self.data:DoorName(vo.name, protocol)
	VoMgr:AddDoor(vo)
end

function SceneCtrl:OnVisibleObjEnterNPCFuncNpc(protocol)
	protocol.obj_type = SceneObjType.Npc
	local vo = CommonStruct.SNpcVo(protocol)
	vo.name = Cfg.NpcById(vo.id).name
	VoMgr:AddSNpc(vo)
end

function SceneCtrl:OnVisibleObjEnterTreasureChest(protocol)
	protocol.obj_type = SceneObjType.Box
	protocol.is_server = true
	VoMgr:AddSNpc(CommonStruct.SNpcVo(protocol))
end

function SceneCtrl:OnNoticeNearbyOnOpenTreasureChest(protocol)
	VoMgr:OpenBox(protocol.obj_id)
	local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(protocol.obj_id)
	if nil ~= obj then
		obj:PlayAnim(ChAnimType.BattleRun)
		AudioMgr:PlayEffect(AudioType.Scene,"box_open")
	end
	--[[ Runner.Instance:RunUntilTrue(function ()
		if nil ~= obj and obj:IsVisible() == true then
			obj:PlayAnim(ChAnimType.BattleRun)
			return true
		end
	end) ]]
end

function SceneCtrl:OnTreasureChestRewardList(protocol)
	-- CommonStruct.AttachParam
	local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(protocol.obj_id)
	if obj then--容错
		for k,v in pairs(protocol.rewardInfo.rewardList) do
			local showItemModel, config = Item.GetIsShowInBoxDrop(v.item_id)
			if showItemModel then
				local vo = CommonStruct.AttachParam(config)
				obj:AddAttachItem(vo)
				break
			end
		end
	end
end

function SceneCtrl:SendTreasureChestReq(obj_id)
	local protocol = GetProtocol(CSTreasureChestReq)
	protocol.obj_id = obj_id or 0
	SendProtocol(protocol)
end

function SceneCtrl:OnChangeAppearanceRole(protocol)
	local appearance = protocol.appearance
	if protocol.obj_id == self.data:GetRoleObjId() then
		RoleCtrl.Instance:OnChangeProfession(CommonStruct.AppearanceParam(appearance))
		if TombRaiderData.Instance:InTombRaider() then 
			CommonStruct.TombRaiderVo(appearance)
		end 
		RoleData.Instance:SetApearance(appearance)
	else
		local vo = VoMgr:RoleVoList()[protocol.obj_id]
		if vo then
			if FishData.Instance:InFishMap(vo.scene_id) then
				return
			elseif SceneData.Instance:SceneId() == MarryData.Instance:ConfigOtherSceneId() then
				if MarryData.Instance:GetWeddingIsHostByUid(vo.role_id) then
					return
				else
					appearance = CommonStruct.NoWeaponNoRideAppearance(appearance)
				end
			elseif TombRaiderData.Instance:InTombRaider() then 
				return 
			end
			vo.appearance = appearance
		end
	end
end

function SceneCtrl:OnNPCInfoNotify(protocol)
	local vo = VoMgr:SNpcVoList()[protocol.obj_id]
	if vo == nil then return end
	if vo.info.info_type == NPC_EXTRA_INFO_TYPE.BRAVE_GROUND then
		-- PublicPopupCtrl.Instance:Center(ErrorText[79])
	end
end

function SceneCtrl:OnUpdateFollowList(protocol)
	self.follow_data:UpdateFollowList(protocol.follow_info)
end

function SceneCtrl:OnAddFollow(protocol)
	self.follow_data:AddFollowInfo(protocol.follow_info)
end

function SceneCtrl:OnRemoveFollow(protocol)
	self.follow_data:RemoveFollowInfo(protocol.follow_info)
end

function SceneCtrl:OnJoinTeamNotifyArea(protocol)
	local objVo = self:VoMgr():RoleVoList()[protocol.obj_id]
	local role_obj_id = SceneData.Instance:GetRoleObjId()
	if objVo and protocol.obj_id ~= role_obj_id then
		objVo.is_in_team = true
		objVo.team_is_red = protocol.is_red == 1
		objVo.team_id = protocol.team_id
	end
end
function SceneCtrl:OnLeaveTeamNotifyArea(protocol)
	local objVo = self:VoMgr():RoleVoList()[protocol.obj_id]
	local role_obj_id = SceneData.Instance:GetRoleObjId()
	if objVo and protocol.obj_id ~= role_obj_id then
		objVo.is_in_team = false
		objVo.team_is_red = false
		objVo.team_id = -1
	end
end
function SceneCtrl:OnTeamRedNameNotifyArea(protocol)
	for i, objVo in pairs(self:VoMgr():RoleVoList()) do
		if objVo.team_id == protocol.team_id then
			objVo.team_is_red = protocol.is_red == 1
		end
	end
end

--[[ 
function SceneCtrl:OnSpecialAppearanceChangeNotice(protocol)
end ]]

--客户端辅助寻路
function SceneCtrl:OnSCMoveAssistReq(proto_t)
	if self.data:SceneId() ~= proto_t.scene_id then
		return
	end
	local protocol = TableCopy(proto_t)

	local start_pos = self:CurScene():PosToClint(protocol.cur_pos_x, protocol.cur_pos_y)
	local end_pos = self:CurScene():PosToClint(protocol.target_pos_x, protocol.target_pos_y)
	if protocol.assist_type == 2 then
		self.assist_type_times = self.assist_type_times or 1
		local pos_list = self.data:GetXunLuPos()
		local times = 0
		while(times <= 10) do
			times = times + 1
			local random_pos = pos_list[self.assist_type_times]
			if random_pos == nil then
				self.assist_type_times = 1
				random_pos = pos_list[1]
			end
			end_pos = {x = random_pos.x,y = SceneCtrl.Instance:CurScene():GetPosHeight(random_pos.x, random_pos.y), z = random_pos.y}
			protocol.way_point = SceneData.CalculatePath(start_pos, end_pos)
			self.assist_type_times = self.assist_type_times + 1
			if protocol.way_point ~= nil then
				self.SendMoveAssistRet(protocol)
				break
			end
			
		end
	else
		protocol.way_point = SceneData.CalculatePath(start_pos, end_pos)
		if protocol.way_point ~= nil then
			self.SendMoveAssistRet(protocol)
		end
	end
	
end
-- function SceneCtrl:CalculatePathTest()
-- 	local start_pos = self:CurScene():PosToClint(self:CurScene():PosToServer({x = 72, y = 66.69, z = 109}))
-- 	local end_pos = self:CurScene():PosToClint(self:CurScene():PosToServer({x = 96.7, y = 67.198, z = 171}))

-- end

function SceneCtrl.SendMoveAssistRet(param)
	local protocol = GetProtocol(CSMoveAssistRet)
	protocol.move_assist_id = param.move_assist_id
    protocol.scene_id = param.scene_id
    protocol.scene_key = param.scene_key

    protocol.obj_id = param.obj_id
    protocol.assist_type = param.assist_type

    protocol.cur_pos_x = param.cur_pos_x
    protocol.cur_pos_y = param.cur_pos_y
    protocol.target_pos_x = param.target_pos_x
    protocol.target_pos_y = param.target_pos_y
    protocol.way_point = param.way_point
    SendProtocol(protocol)
end

function SceneCtrl:OnChangeSceneNotice(protocol)
	LogDG("强设主角位置")
	self.data:SetChangeSceneNotice(protocol.info)
end

--巡游花车进入视野
function SceneCtrl:OnVisibleObjEnterParadeFloat(protocol)
	MarryData.Instance:SetParadeFloatInfo(protocol.info.host_info)
	VoMgr:AddSNpc(CommonStruct.NpcVo(protocol))
	VoMgr:AddSNpc(CommonStruct.OtherRoleVo(protocol))
end

function SceneCtrl:OnVisibleObjEnterPeakTokenObj(protocol)
	VoMgr:AddSNpc(CommonStruct.OtherRoleVo(protocol))
end

function SceneCtrl:OnSCROleDaWanKaInfoChange(protocol)
	LogError("5745",protocol)
	VoMgr:ChangeRoleDaWanKaLevel(protocol.obj_id, protocol.da_wan_ka_level)
end