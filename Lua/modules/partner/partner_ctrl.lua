
FILE_REQ("modules/partner/partner_config")
FILE_REQ("modules/partner/partner_info")
FILE_REQ("modules/partner/partner_skill_info")
FILE_REQ("modules/partner/partner_data")
FILE_REQ("modules/partner/partner_data_life_handbook")
FILE_REQ("modules/partner/partner_data_fuyu")
VIEW_REQ("modules/partner/partner_items")
VIEW_REQ("modules/partner/partner_view")
VIEW_REQ("modules/partner/partner_view_attr_wash")
VIEW_REQ("modules/partner/partner_view_up_preview")
VIEW_REQ("modules/partner/partner_view_up_success")
VIEW_REQ("modules/partner/partner_view_lock")
VIEW_REQ("modules/partner/partner_view_skill_replace")
VIEW_REQ("modules/partner/partner_hole_view")
VIEW_REQ("modules/partner/partner_life_view")
VIEW_REQ("modules/partner/partner_life_suit_view")
VIEW_REQ("modules/partner/partner_up_attr_view")
VIEW_REQ("modules/partner/partner_contract_view")
VIEW_REQ("modules/partner/partner_contract_attr_view")
VIEW_REQ("modules/partner/partner_crystal_view")
VIEW_REQ("modules/partner/partner_reset_level_view")
VIEW_REQ("modules/partner/partner_skillf_up_view")
VIEW_REQ("modules/partner/partner_biography_view")
VIEW_REQ("modules/partner/partner_life_handbook_view")
VIEW_REQ("modules/partner/partner_gift")
VIEW_REQ("modules/partner/partner_up_gift_view")
VIEW_REQ("modules/partner/partner_gift_sp_show_view")
VIEW_REQ("modules/partner/partner_gift_reset_view")
VIEW_REQ("modules/partner/partner_soar_view")
VIEW_REQ("modules/partner/partner_fuyu_bag")
VIEW_REQ("modules/partner/partner_soar_view")
VIEW_REQ("modules/partner/partner_fuyu_qianghua")
VIEW_REQ("modules/partner/partner_fuyu_result")
VIEW_REQ("modules/partner/partner_fuyu_decom")
VIEW_REQ("modules/partner/partner_fuyu_effect_show")
VIEW_REQ("modules/partner/partner_fuyu_change")
VIEW_REQ("modules/partner/partner_fuyu_sublimation")

VIEW_REQ("modules/item_info/partner_fetter_info_view")
VIEW_REQ("modules/item_info/partner_skill_info_view")

PartnerCtrl = PartnerCtrl or BaseClass(BaseCtrl)

function PartnerCtrl:__init()
	if PartnerCtrl.Instance ~= nil then
		Debuger.LogError("[PartnerCtrl] attempt to create singleton twice!")
		return
	end	
	PartnerCtrl.Instance = self
	self.data = PartnerData.New()

	self:RegisterAllProtocols()
end

function PartnerCtrl:__delete()
	if PartnerCtrl.Instance == self then
		PartnerCtrl.Instance = nil
	end

	self:UnRegisterAllProtocols()
	TaskData.Instance:UnCareAcceptedTaskChange(self.handle_completed)
	FlyUpData.Instance:UnCareInfoDataChange(self.handle_fly_up)
end

function PartnerCtrl:OnInit()
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, self.data.partner_acti_red_point, function ()
		return self.data.partner_acti_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, AttributeDanData.Instance.attribute_red_piont_partner, function ()
		return  AttributeDanData.Instance.attribute_red_piont_partner.count
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerUp, self.data.partner_up_red_point, function ()
		return self.data.partner_up_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, self.data.partner_crystal_red_point, function ()
		return self.data.partner_crystal_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, self.data.partner_skill_red_point, function ()
		return self.data.partner_skill_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, self.data.partner_attr_level_red_point, function ()
		return self.data.partner_attr_level_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, self.data.partner_biography_red_point, function ()
		return self.data.partner_biography_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, self.data.partner_soar_red_point, function ()
		return self.data.partner_soar_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, self.data.partner_soar_weapon_red_point, function ()
		return self.data.partner_soar_weapon_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerStrengthen, self.data.partner_strengthen_red_point, function ()
		return self.data.partner_strengthen_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerAttr, self.data.partner_life_red_point, function ()
		return (FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerLife) and self.data.partner_life_red_point.val) and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerContract, self.data.partner_pact_equip_red_point, function ()
		return (FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerContract) and self.data.partner_pact_equip_red_point.val) and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerContract, self.data.partner_pact_up_red_point, function ()
		return (FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerContract) and self.data.partner_pact_up_red_point.val) and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerQuality, self.data.partner_qua_red_point, function ()
		return (FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerQuality) and self.data.partner_qua_red_point.val) and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Partner.PartnerGift, self.data.partner_gift_red_point, function ()
		return (FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerGift) and self.data.partner_gift_red_point.val) and 1 or 0
	end)
	local acti_rp_func = BindTool.Bind(PartnerData.PartnerActiRedPoint,self.data)
	local lifecalc_rp_func = BindTool.Bind(PartnerData.PartnerLifeCalc,self.data)
	local life_rp_func = BindTool.Bind(PartnerData.PartnerLifeRedPoint,self.data)
	local up_rp_func = BindTool.Bind(PartnerData.PartnerUpRedPoint,self.data)
	local crystal_rp_func = BindTool.Bind(PartnerData.PartnerCrystalRedPoint,self.data)
	local attrlv_rp_func = BindTool.Bind(PartnerData.PartnerAttrLevelRedPoint,self.data)
	local qua_rp_func = BindTool.Bind(PartnerData.PartnerQuaRedPoint,self.data)
	local gift_rp_func = BindTool.Bind(PartnerData.PartnerGiftRedPoint,self.data)
	local soarweapon_rp_func = BindTool.Bind(PartnerData.PartnerSoarWeaponRedPoint,self.data)


	BagData.Instance:Care(acti_rp_func)
	BagData.Instance:Care(lifecalc_rp_func)
	BagData.Instance:Care(life_rp_func)
	BagData.Instance:Care(up_rp_func)
	BagData.Instance:Care(crystal_rp_func)
	BagData.Instance:Care(attrlv_rp_func)
	BagData.Instance:Care(qua_rp_func)
	BagData.Instance:Care(gift_rp_func)
	BagData.Instance:Care(soarweapon_rp_func)

	RoleData.Instance:GetBaseData():Care(up_rp_func, "level")
	RoleData.Instance:GetBaseData():Care(attrlv_rp_func, "level")

	-- BagData.Instance:Care(function ()
	-- 	self.data:PartnerActiRedPoint()
	-- 	self.data:PartnerLifeCalc()
	-- 	self.data:PartnerLifeRedPoint()
	-- 	self.data:PartnerUpRedPoint()
	-- 	self.data:PartnerCrystalRedPoint()
	-- 	self.data:PartnerAttrLevelRedPoint()
	-- 	self.data:PartnerQuaRedPoint()
	-- 	self.data:PartnerGiftRedPoint()
	-- 	self.data:PartnerSoarWeaponRedPoint()
	-- end)
	-- RoleData.Instance:GetBaseData():Care(function ()
	-- 	self.data:PartnerUpRedPoint()
	-- 	self.data:PartnerAttrLevelRedPoint()
	-- end, "level")

	-- self.handle_completed = TaskData.Instance:CareCompletedTaskChange(BindTool.Bind(self.data.PartnerSoarRedPoint, self.data))
	self.handle_fly_up = FlyUpData.Instance:CareInfoDataFlyFlagChange(BindTool.Bind(self.data.PartnerSoarRedPoint, self.data))
	TaskData.Instance:ListenTaskCompleteByIds(BindTool.Bind(self.OnTaskComplete, self), self.data.partner_soar_task_ids)
end

function PartnerCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSPartnerOperate)
	self:RegisterProtocol(CSPartnerToSaveSkillSelection)
	self:RegisterProtocol(CSPartnerSkillActivate)
	self:RegisterProtocol(CSChangeSyncFightOrder)
	self:RegisterProtocol(CSPartnerSwitchFightPos)
	self:RegisterProtocol(CSPartnerEqOperatorReq)
	self:RegisterProtocol(CSPartnerQualityUpReq)
	self:RegisterProtocol(CSPartnerElementCrystalOper)
	self:RegisterProtocol(CSPartnerSkinReq)
	self:RegisterProtocol(CSPartnerIntensifyReq)
	self:RegisterProtocol(CSPatnerPetRankReq)
	self:RegisterProtocol(CSFuYuReq)

	self:RegisterProtocol(SCPartnerAllInfo, "OnPartnerAllInfo")
	self:RegisterProtocol(SCChangeFightStateReq, "OnChangeFightStateReq", true)
	self:RegisterProtocol(SCPartnerAttrListInfo, "OnPartnerAttrListInfo", true)
	self:RegisterProtocol(SCPartnerFetter, "OnPartnerFetter", true)
	self:RegisterProtocol(SCPartnerBaseGradeInfo, "OnPartnerBaseGradeInfo", true)
	self:RegisterProtocol(SCPartnerSkillReq, "OnPartnerSkillReq", true)
	self:RegisterProtocol(SCSynFightPartnerOrder, "OnSynFightPartnerOrder")
	self:RegisterProtocol(SCPartnerEqSingleSlots, "OnPartnerEqSingleSlots", true)
	self:RegisterProtocol(SCPartnerQualityUp, "OnPartnerQualityUp", true)
	self:RegisterProtocol(SCPartnerUseElementCrystal, "OnPartnerUseElementCrystal", true)
	self:RegisterProtocol(SCPartnerSkinActive, "OnPartnerSkinActive")
	self:RegisterProtocol(SCPartnerSkinSingleOper, "OnPartnerSkinSingleOper", true)
	self:RegisterProtocol(SCPartnerSingleInfo, "OnPartnerSingleInfo", true)
	self:RegisterProtocol(SCPartnerGiftTreeListInfo, "OnPartnerGiftTreeListInfo", true)
	self:RegisterProtocol(SCPartnerSingleGiftTreeInfo, "OnPartnerSingleGiftTreeInfo", true)
	self:RegisterProtocol(SCPartnerSpiritPactList, "OnPartnerSpiritPactList", true)
	self:RegisterProtocol(SCPartnerPactBaseAttr, "OnPartnerPactBaseAttr", true)
	self:RegisterProtocol(SCPartnerPactInfo, "OnPartnerPactInfo", true)
	self:RegisterProtocol(SCPartnerSoarNotice, "OnPartnerSoarNotice", true)
	self:RegisterProtocol(SCNewTaskPartnerSoarInfo, "OnNewTaskPartnerSoarInfo", true)
	self:RegisterProtocol(SCPartnerIntensifyInfo, "OnPartnerIntensifyInfo", true)
	self:RegisterProtocol(SCPartnerTopRankInfo, "OnPartnerTopRankInfo", true)

	--符玉
	self:RegisterProtocol(SCAllFuYuInfo, "SCAllFuYuInfo")
	self:RegisterProtocol(SCFuYuInfo, "SCFuYuInfo")
end

function PartnerCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCPartnerAllInfo)
	self:UnRegisterProtocol(SCChangeFightStateReq)
	self:UnRegisterProtocol(SCPartnerAttrListInfo)
	self:UnRegisterProtocol(SCPartnerFetter)
	self:UnRegisterProtocol(SCPartnerBaseGradeInfo)
	self:UnRegisterProtocol(SCPartnerSkillReq)
	self:UnRegisterProtocol(SCSynFightPartnerOrder)
	self:UnRegisterProtocol(SCPartnerEqSingleSlots)
	self:UnRegisterProtocol(SCPartnerQualityUp)
	self:UnRegisterProtocol(SCPartnerUseElementCrystal)
	self:UnRegisterProtocol(SCPartnerSkinActive)
	self:UnRegisterProtocol(SCPartnerSkinSingleOper)
	self:UnRegisterProtocol(SCPartnerSingleInfo)
	self:UnRegisterProtocol(SCPartnerSpiritPactList)
	self:UnRegisterProtocol(SCPartnerPactBaseAttr)
	self:UnRegisterProtocol(SCPartnerPactInfo)
	self:UnRegisterProtocol(SCPartnerSoarNotice)
	self:UnRegisterProtocol(SCNewTaskPartnerSoarInfo)
	self:UnRegisterProtocol(SCPartnerIntensifyInfo)
	self:UnRegisterProtocol(SCPartnerTopRankInfo)
end

function PartnerCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function PartnerCtrl:UnLockTips()
	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	local vo = data.vo
	if vo.is_unlocked == PartnerConfig.LockState.lock then
		PublicPopupCtrl.Instance:Center(Language.Partner.UnLockTips)
		return true
	else
		return false
	end
end

function PartnerCtrl:StrengthenLockTips()
	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	local vo = data.vo
	if self:UnLockTips() then return true end
	local co = Cfg.ParnterIntensifyByIdLevel(self.data.cur_sel_partner, vo.intensify_level)
	if vo.level < co.levels then
		PublicPopupCtrl.Instance:Center(string.format(Language.Partner.StrengthenTips.level_tips, co.levels))
		return true
	elseif vo.soar_count < co.feisheng_num then
		PublicPopupCtrl.Instance:Center(Language.Partner.StrengthenTips.soar_tips[co.feisheng_num])
		return true
	end
	return false
end

function PartnerCtrl:OnTaskComplete(task_id)
	self.data.cur_sel_partner = self.data.partner_soar_task_ids2[task_id] or 1
	ViewMgr:OpenView(PartnerSoarView)
	self.data:PartnerSoarRedPoint()
end

function PartnerCtrl:OnPartnerAllInfo(protocol)
    self.data:SetPartnerAllInfo(protocol)
end

function PartnerCtrl:OnPartnerSingleInfo(protocol)
    self.data:SetPartnerSingleInfo(protocol)
end

function PartnerCtrl:OnChangeFightStateReq(protocol)
	self.data:SetChangeFightState(protocol)
	-- if protocol.is_show == 1 then
		-- self.data.new_partner = protocol.partner
		-- ViewMgr:OpenView(PartnerViewLock)
	-- end
end

function PartnerCtrl:OnPartnerAttrListInfo(protocol)
    self.data:SetPartnerAttrListInfo(protocol)
end

function PartnerCtrl:OnPartnerFetter(protocol)
    self.data:SetPartnerFetter(protocol)
end

function PartnerCtrl:OnPartnerBaseGradeInfo(protocol)
	self.data:SetPartnerBaseGradeInfo(protocol)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.UpGeneral2)
end

function PartnerCtrl:OnPartnerSkillReq(protocol)
    self.data:SetPartnerSkill(protocol)
end

function PartnerCtrl:OnSynFightPartnerOrder(protocol)
	self.data:SetFightOrderList(protocol)
	
	if not TeamCtrl.Instance:Data():InTeam() then
		if ViewMgr:IsOpen(TeamView) then ViewMgr:FlushView(TeamView) end
	end
end

function PartnerCtrl:OnPartnerEqSingleSlots(protocol)
	self.data:SetPartnerEqSingleSlots(protocol)
end

function PartnerCtrl:OnPartnerQualityUp(protocol)
	self.data:SetPartnerQualityInfo(protocol)
end

function PartnerCtrl:OnPartnerUseElementCrystal(protocol)
	self.data:SetPartnerUseElementCrystal(protocol)
end

function PartnerCtrl:OnPartnerSkinActive(protocol)
	self.data:SetPartnerSkinActive(protocol)
end

function PartnerCtrl:OnPartnerSkinSingleOper(protocol)
	self.data:SetPartnerSkinSingleOper(protocol)
end

function PartnerCtrl:OnPartnerGiftTreeListInfo(protocol)
	self.data:SetGiftAllInfo(protocol)
end

function PartnerCtrl:OnPartnerSingleGiftTreeInfo(protocol)
	self.data:SetSingleGiftInfo(protocol)
end

function PartnerCtrl:OnPartnerSpiritPactList(protocol)
	self.data:SetPartnerSpiritPactList(protocol)
end

function PartnerCtrl:OnPartnerPactBaseAttr(protocol)
	self.data:SetPartnerPactBaseAttr(protocol)
end

function PartnerCtrl:OnPartnerPactInfo(protocol)
	self.data:SetPartnerPactInfo(protocol)
end

function PartnerCtrl:OnPartnerSoarNotice(protocol)
	self.data:SetPartnerSoarNotice(protocol)
end

function PartnerCtrl:OnNewTaskPartnerSoarInfo(protocol)
	self.data:SetNewTaskPartnerSoarInfo(protocol)
end

function PartnerCtrl:OnPartnerIntensifyInfo(protocol)
	-- ylog("OnPartnerIntensifyInfo", protocol)
	self.data:SetPartnerIntensifyInfo(protocol)
end

function PartnerCtrl:OnPartnerTopRankInfo(protocol)
	self.data:SetPartnerTopRankInfo(protocol)
end

function PartnerCtrl:SendPartnerOperate(param_t)
	local protocol = GetProtocol(CSPartnerOperate)
	protocol.oper_type = param_t.oper_type or 0
	protocol.partner = param_t.partner or 0
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	protocol.param3 = param_t.param3 or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPartnerInfo()
	local param_t = {
        oper_type = PartnerConfig.OperType.info,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerFight(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.fight,
		partner = partner
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerUp(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.up,
		partner = partner
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerWash(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.wash,
		partner = partner
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerWashSave(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.wash_save,
		partner = partner
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerActiPartner(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.acti_partner,
		partner = partner
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerSingleInfo(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.signle_info,
		partner = partner
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerUpLevel(partner, up_level)
	local param_t = {
		oper_type = PartnerConfig.OperType.up_level,
		partner = partner,
		param1 = up_level,
	}
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerResetLevel(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.reset_level,
		partner = partner
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerOpenBiography(partner, biography_id)
	local param_t = {
		oper_type = PartnerConfig.OperType.open_biography,
		partner = partner,
		param1 = biography_id,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerFetchBiography(partner, biography_id)
	local param_t = {
		oper_type = PartnerConfig.OperType.fetch_biography,
		partner = partner,
		param1 = biography_id,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerPageCompose(item_id)
	local param_t = {
		oper_type = PartnerConfig.OperType.page_compose,
		param1 = item_id,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerGiftLevel(partner,index)
	local param_t = {
		oper_type = PartnerConfig.OperType.uplevel_gift,
		partner = partner,
		param1 = index,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerGiftReset(partner,index)
	local param_t = {
		oper_type = PartnerConfig.OperType.reset_gift,
		partner = partner,
		param1 = index,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerContractPutOn(partner, index, save_index)
	local param_t = {
		oper_type = PartnerConfig.OperType.contract_put_on,
		partner = partner,
		param1 = index,
		param2 = save_index,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerContractPutOff(partner, index)
	local param_t = {
		oper_type = PartnerConfig.OperType.contract_put_off,
		partner = partner,
		param1 = index,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerContractUpStar(partner, index)
	local param_t = {
		oper_type = PartnerConfig.OperType.contract_up_star,
		partner = partner,
		param1 = index,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerContractReplace(to_partner, to_index, from_partner, from_index)
	local param_t = {
		oper_type = PartnerConfig.OperType.contract_replace,
		partner = to_partner,
		param1 = to_index,
		param2 = from_partner,
		param3 = from_index,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerSoar(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.soar,
		partner = partner,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerSoarWeapon(partner)
	local param_t = {
		oper_type = PartnerConfig.OperType.soar_weapon,
		partner = partner,
    }
	self:SendPartnerOperate(param_t)
end

function PartnerCtrl:SendPartnerSaveSkill(param_t)
	local protocol = GetProtocol(CSPartnerToSaveSkillSelection)
	protocol.partner = self.data.cur_sel_partner or 0
	protocol.index = param_t.index or 1
	protocol.skill_id = param_t.skill_id or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPartnerSkillActivate(param_t)
	local protocol = GetProtocol(CSPartnerSkillActivate)
	protocol.partner = self.data.cur_sel_partner or 0
	protocol.index = param_t.index or 1
	protocol.is_active = param_t.is_equip and 0 or 1
	SendProtocol(protocol)
end

function PartnerCtrl:SendChangeSyncFightOrder(insert_partner, target_partner)
	local protocol = GetProtocol(CSChangeSyncFightOrder)
	protocol.insert_partner = insert_partner or 0
	protocol.target_partner = target_partner or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPartnerSwitchFightPos(p1, p2)
	local protocol = GetProtocol(CSPartnerSwitchFightPos)
	protocol.partner_id1 = p1 or 0
	protocol.partner_id2 = p2 or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPartnerEqOperatorReq(op_type, partner, param1)
	local protocol = GetProtocol(CSPartnerEqOperatorReq)
	protocol.op_type = op_type or 0
	protocol.partner = partner or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPartnerQualityUpReq(param_t)
	local protocol = GetProtocol(CSPartnerQualityUpReq)
	protocol.use_type = param_t.use_type or 0
	protocol.up_type = param_t.up_type or 0
	protocol.partner_id = param_t.partner_id or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPartnerElementCrystalOper(param_t)
	local protocol = GetProtocol(CSPartnerElementCrystalOper)
	protocol.oper_type = param_t.oper_type or 0
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	protocol.param3 = param_t.param3 or 0
	protocol.param4 = param_t.param4 or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPartnerElementCrystalOperUse(partner, crystal)
	local param_t = {
		oper_type = PartnerConfig.ElementCrystalOperType.use,
		param1 = partner,
		param2 = crystal,
    }
	self:SendPartnerElementCrystalOper(param_t)
end

function PartnerCtrl:SendPartnerElementCrystalOperRemove(partner)
	local param_t = {
		oper_type = PartnerConfig.ElementCrystalOperType.remove,
		param1 = partner
    }
	self:SendPartnerElementCrystalOper(param_t)
end

function PartnerCtrl:SendPartnerElementCrystalOperUseQuick(crystals)
	local param_t = {
		oper_type = PartnerConfig.ElementCrystalOperType.use_quick,
		param1 = crystals[1],
		param2 = crystals[2],
		param3 = crystals[3],
		param4 = crystals[4],
    }
	self:SendPartnerElementCrystalOper(param_t)
end

function PartnerCtrl:SendPartnerElementCrystalOperRemoveAll()
	local param_t = {
		oper_type = PartnerConfig.ElementCrystalOperType.remove_all,
    }
	self:SendPartnerElementCrystalOper(param_t)
end

function PartnerCtrl:SendPartnerSkinReq(param_t)
	local protocol = GetProtocol(CSPartnerSkinReq)
	protocol.oper_type = param_t.oper_type or 0
	protocol.partner_id = param_t.partner_id or 0
	protocol.skin_id = param_t.skin_id or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPartnerSkinReqActive(partner_id, skin_id)
	local param_t = {
		oper_type = PartnerConfig.SkinOperType.active,
		partner_id = partner_id,
		skin_id = skin_id,
    }
	self:SendPartnerSkinReq(param_t)
end

function PartnerCtrl:SendPartnerSkinReqEquip(partner_id, skin_id)
	local param_t = {
		oper_type = PartnerConfig.SkinOperType.equip,
		partner_id = partner_id,
		skin_id = skin_id,
    }
	self:SendPartnerSkinReq(param_t)
end

function PartnerCtrl:SendPartnerSkinReqRemove(partner_id, skin_id)
	local param_t = {
		oper_type = PartnerConfig.SkinOperType.remove,
		partner_id = partner_id,
		skin_id = skin_id,
    }
	self:SendPartnerSkinReq(param_t)
end

function PartnerCtrl:SendPartnerIntensifyReq(partner_id, items, count)
	items = items or {}
	local protocol = GetProtocol(CSPartnerIntensifyReq)
	protocol.partner_id = partner_id or 0
	protocol.expend_item_1 = items[1] or {}
	protocol.expend_item_3 = items[3] or {}
	protocol.expend_item_2_count = count
	protocol.expend_item_2_list = items[2] or {}
	-- ylog("SendPartnerIntensifyReq", protocol)
	SendProtocol(protocol)
end

function PartnerCtrl:SendPatnerPetRankReq(rank_type)
	local protocol = GetProtocol(CSPatnerPetRankReq)
	protocol.rank_type = rank_type or 0
	SendProtocol(protocol)
end

function PartnerCtrl:SendPatnerPetRankReqPartner()
	self:SendPatnerPetRankReq(1)
end

--符玉
function PartnerCtrl:SCAllFuYuInfo(protocol)
	self.data:SetFuYuAllInfo(protocol.info_list)
end

function PartnerCtrl:SCFuYuInfo(protocol)
	self.data:SetFuYuInfo(protocol)
end

function PartnerCtrl:CSFuYuReq(req_type, param1, param2, param3, param4, param5, param6)
	local protocol = GetProtocol(CSFuYuReq)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	protocol.param3 = param3 or 0
	protocol.param4 = param4 or 0
	protocol.param5 = param5 or 0
	protocol.param6 = param6 or 0 
	SendProtocol(protocol)
end
function PartnerCtrl:ReqFuYuInfo()
	self:CSFuYuReq(0)
end
function PartnerCtrl:ReqFuYuPutOn(partner_id, index, col, bag_index)
	self:CSFuYuReq(1, partner_id, index, col, bag_index)
end
function PartnerCtrl:ReqFuYuTakeOff(partner_id, index)
	self:CSFuYuReq(2, partner_id, index)
end
function PartnerCtrl:ReqFuYuDecompose(col, bag_index)
	self:CSFuYuReq(3, col, bag_index)
end
--身上强化
function PartnerCtrl:ReqFuYuUpgrade(partner_id, index, cost_item, use_protect)
	self:CSFuYuReq(4, partner_id, index, cost_item, use_protect)
end
--背包强化
function PartnerCtrl:ReqFuYuUpgrade2(col, bag_index, cost_item, use_protect)
	self:CSFuYuReq(5, col, bag_index, cost_item, use_protect)
end
--身上转换 要转换的特效1 特效2没有填0
function PartnerCtrl:ReqFuYuConvert(partner_id, index, sp_id1, sp_id2)
	self:CSFuYuReq(6, partner_id, index, sp_id1, sp_id2)
end
--背包转换 要转换的特效1 特效2没有填0
function PartnerCtrl:ReqFuYuConvert2(col, bag_index, sp_id1, sp_id2)
	self:CSFuYuReq(7, col, bag_index, sp_id1, sp_id2)
end
--身上升华 消耗15阶同名天赐符玉  10/15阶非天赐符玉
function PartnerCtrl:ReqFuYuSublime(partner_id, index, col, bag_index, col2, bag_index2)
	self:CSFuYuReq(8, partner_id, index, col, bag_index, col2, bag_index2)
end
--背包升华 消耗15阶同名天赐符玉  10/15阶非天赐符玉
function PartnerCtrl:ReqFuYuSublime2(col, bag_index, col1, bag_index1, col2, bag_index2)
	self:CSFuYuReq(9, col, bag_index, col1, bag_index1, col2, bag_index2)
end