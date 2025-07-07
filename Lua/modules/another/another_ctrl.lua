FILE_REQ("modules/another/another_data")
VIEW_REQ("modules/another/another_view")
VIEW_REQ("modules/another/another_role_view")
VIEW_REQ("modules/another/another_equip_view")
VIEW_REQ("modules/another/another_pet_view")
VIEW_REQ("modules/another/another_role_point_view")
VIEW_REQ("modules/another/another_medal_equip")

AnotherCtrl = AnotherCtrl or BaseClass(BaseCtrl)

BattlefieldRankType = {
    Friends = 1,
    Main = 2,
    Cross = 3,
}
--化身模块信息请求
ANOTHER_INFO_TYPE = {
    ALL_INFO = 0,		--所有信息
    RENWU = 1,			--化身人物信息
    POINT = 2,			--化身加点信息
    KNAPSACK = 3,		--背包信息
    PET = 4,			--所有宠物
    PET_POINT = 5,		--宠物加点信息  p1 : pet_index
    FIRST_ENTER = 6,	--首次进入活动场景,重置相关信息并下发
	FINISH_GUIDE = 7    --完成化身指引
}

AnotherPetOpType = {
	SetPetState = 0, 		--设置宠物状态，p1 index，p2 状态（1出战，2跟随，3待机1，4待机2，5待机3）
	RefreshPoints = 1,		--洗点 p1 index
	PetAttrsReq = 2,		--请求宠物属性 p1 index
	PetEquipmentPutOn = 3,	--宠物装备穿戴 p1 index p2 背包格子
	PetEquipmentPutOff = 4,	--宠物装备穿戴 p1 index
}

--化身装备操作请求
ANOTHER_EQUIP_TYPE = 
{
    INFO_REQ = 0,			-- 0 请求信息
    PUTON = 1,				-- 1 穿戴装备 \param param1 0  \param param2 背包格子序号  \param param3 穿戴的部位 \see EQUIPMENT_INDEX \ param param4 role_type 
    TAKEOFF = 2,			-- 2 脱下装备 \param param1 要脱下的部位 \see EQUIPMENT_INDEX \ param2 role_type
    TAKEOFF_CRYSTAL = 3,	-- 3 脱下元素水晶
    PUTON_CRYSTAL = 4,		-- 5 装备元素水晶 \param param1 化身背包格子序号
    INORDER = 5,			-- 6 整理背包
}

function AnotherCtrl:__init()
	if AnotherCtrl.Instance ~= nil then
		Debuger.LogError("[AnotherCtrl] attempt to create singleton twice!")
		return
	end	
    AnotherCtrl.Instance = self
    self.data = AnotherData.New()
	self:RegisterAllProtocols()
end

function AnotherCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function AnotherCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSHuaShenInfoReq)
	self:RegisterProtocol(SCHuaShenSeasonInfo,"OnSeasonInfo")
	--化身人物
	self:RegisterProtocol(CSHuaShenBattlePosition)
	self:RegisterProtocol(CSHuaShenAttributePlanPointReq)
	self:RegisterProtocol(CSHuaShenAttributePlanChangeName)
	self:RegisterProtocol(SCHuaShenRoleInfoAck,"OnRoleInfoAck")
	self:RegisterProtocol(SCHuaShenRoleAttrList,"OnRoleAttrList")
	self:RegisterProtocol(SCHuaShenRoleAttributePlanAllInfo,"OnRoleAttributePlanAllInfo")
	self:RegisterProtocol(SCHuaShenRoleAttributePlanInfo,"OnRoleAttributePlanInfo")
	self:RegisterProtocol(SCHuaShenBattlePosition,"OnBattlePosition")
	--化身装备
	self:RegisterProtocol(CSHuaShenMedalReq)
	self:RegisterProtocol(CSHuaShenJewelryHandleReq)
	self:RegisterProtocol(CSHuaShenEquipmentOperaReq)
	self:RegisterProtocol(SCHuaShenMedalInfo,"OnMedalInfo")
	self:RegisterProtocol(SCHuaShenEquipmentInfoAck,"OnEquipmentInfoAck")
	self:RegisterProtocol(SCHuaShenEquipmentGridChange,"OnEquipmentGridChange")
	self:RegisterProtocol(SCHuaShenJewelryAllInform,"OnJewelryAllInform")
	self:RegisterProtocol(SCHuaShenJewelryInfoChange,"OnJewelryInfoChange")
	self:RegisterProtocol(SCHuaShenElementCrystalChangeNotice,"OnElementCrystalChangeNotice")
	self:RegisterProtocol(SCHuaShenKnapsackInfoAck,"OnKnapsackInfoAck")
	self:RegisterProtocol(SCHuaShenKnapsackItemChange,"OnKnapsackItemChange")
	self:RegisterProtocol(SCHuaShenMedalTakeOffInfo,"OnMedalTakeOffInfo")
	--化身宠物
	self:RegisterProtocol(CSHuaShenPetOp)
	self:RegisterProtocol(CSHuaShenPetAssignAttributePoints)
	self:RegisterProtocol(SCHuaShenSendPetAllInfo,"OnPetAllInfo")
	self:RegisterProtocol(SCHuaShenSendPetSingleInfo,"OnPetSingleInfo")
	self:RegisterProtocol(SCHuaShenSendPetOtherInfo,"OnPetOtherInfo")
	self:RegisterProtocol(SCHuaShenSendPetAttrListInfo,"OnPetAttrListInfo")
end

function AnotherCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSHuaShenInfoReq)
	self:UnRegisterProtocol(SCHuaShenSeasonInfo)
	--化身人物
	self:RegisterProtocol(CSHuaShenBattlePosition)
	self:RegisterProtocol(CSHuaShenAttributePlanPointReq)
	self:RegisterProtocol(CSHuaShenAttributePlanChangeName)
	self:UnRegisterProtocol(SCHuaShenRoleInfoAck)
	self:UnRegisterProtocol(SCHuaShenRoleAttrList)
	self:UnRegisterProtocol(SCHuaShenRoleAttributePlanAllInfo)
	self:UnRegisterProtocol(SCHuaShenRoleAttributePlanInfo)
	self:UnRegisterProtocol(SCHuaShenBattlePosition)
	--化身装备
	self:UnRegisterProtocol(CSHuaShenMedalReq)
	self:UnRegisterProtocol(CSHuaShenJewelryHandleReq)
	self:UnRegisterProtocol(CSHuaShenEquipmentOperaReq)
	self:UnRegisterProtocol(SCHuaShenEquipmentInfoAck)
	self:UnRegisterProtocol(SCHuaShenEquipmentGridChange)
	self:UnRegisterProtocol(SCHuaShenJewelryAllInform)
	self:UnRegisterProtocol(SCHuaShenJewelryInfoChange)
	self:UnRegisterProtocol(SCHuaShenElementCrystalChangeNotice)
	self:UnRegisterProtocol(SCHuaShenKnapsackInfoAck)
	self:UnRegisterProtocol(SCHuaShenKnapsackItemChange)
	--化身宠物
	self:UnRegisterProtocol(CSHuaShenPetOp)
	self:UnRegisterProtocol(CSHuaShenPetAssignAttributePoints)
	self:UnRegisterProtocol(SCHuaShenSendPetAllInfo)
	self:UnRegisterProtocol(SCHuaShenSendPetSingleInfo)
	self:UnRegisterProtocol(SCHuaShenSendPetOtherInfo)
	self:UnRegisterProtocol(SCHuaShenSendPetAttrListInfo)
end

function AnotherCtrl:__delete()
	self:UnRegisterAllProtocols()
	AnotherCtrl.Instance = nil
	Delete(self,"data")
end

--化身模块信息请求
function AnotherCtrl:SendAllReq(req_type,param1)
	local protocol = GetProtocol(CSHuaShenInfoReq)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

--下发玩家本赛季信息
function AnotherCtrl:OnSeasonInfo(protocol)
	if protocol.season_first_enter_top == 0 and UnityPlayerPrefs.GetInt(PrefKeys.BattlefieldMatch()) == 1 then
		UnityPlayerPrefs.SetInt(PrefKeys.BattlefieldMatch(),0)
	end
	BattlefieldData.SetPrefsKey(protocol.is_finish_guide)
end

--===================================化身人物===================================
function AnotherCtrl:SendPositionReq(req_type)
	local protocol = GetProtocol(CSHuaShenBattlePosition)
	protocol.req_type = req_type or 0
	SendProtocol(protocol)
end

function AnotherCtrl:SendRolePointReq(req_type,add_point_list,plan_index)
	local protocol = GetProtocol(CSHuaShenAttributePlanPointReq)
	protocol.req_type = req_type or 0
	protocol.add_point_list = add_point_list or {}
	protocol.plan_index = plan_index or 0
	SendProtocol(protocol)
end

function AnotherCtrl:SendRolePlanChangeNameReq(plan_index,plan_name)
	local protocol = GetProtocol(CSHuaShenAttributePlanChangeName)
	protocol.plan_index = plan_index or 0
	protocol.plan_name = plan_name or ""
	SendProtocol(protocol)
end

function AnotherCtrl:OnRoleInfoAck(protocol)
	self.data:SetRoleInfoAck(protocol)
end

function AnotherCtrl:OnRoleAttrList(protocol)
	local old_list = self.data:GetRoleAttrList()
	if ViewMgr:IsOpen(AnotherView) and next(old_list) ~= nil then
		local change_list = DataHelper.GetChangeList(old_list,protocol.attr_list)
		local title_info = {fromType = AttrChangeData.ChangeFromType.Role,showName = RoleData.Instance:GetRoleName()}
		MainOtherCtrl.Instance:OpenAttrChange(change_list,title_info)
	end
	self.data:SetRoleAttrList(protocol)
end

function AnotherCtrl:OnRoleAttributePlanAllInfo(protocol)
	self.data:SetRoleAttributePlanAllInfo(protocol)
end

function AnotherCtrl:OnRoleAttributePlanInfo(protocol)
	self.data:SetRoleAttributePlanInfo(protocol)
end

function AnotherCtrl:OnBattlePosition(protocol)
	self.data:SetBattlePosition(protocol)
end

--===================================化身装备===================================
function AnotherCtrl:SendMedalReq(param_t)
	local protocol = GetProtocol(CSHuaShenMedalReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param1 = param_t.param_1 or 0
	protocol.param2 = param_t.param_2 or 0
	SendProtocol(protocol)
end

function AnotherCtrl:SendJewelryReq(param_t)
	local protocol = GetProtocol(CSHuaShenJewelryHandleReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param1 = param_t.param_1 or 0
	SendProtocol(protocol)
end

function AnotherCtrl:SendEquipReq(param_t)
	local protocol = GetProtocol(CSHuaShenEquipmentOperaReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param1 = param_t.param_1 or 0
	protocol.param2 = param_t.param_2 or 0
	protocol.param3 = param_t.param_3 or 0
	protocol.param4 = param_t.param_4 or 0
	SendProtocol(protocol)
end

function AnotherCtrl:OnMedalInfo(protocol)
	self.data:SetMedalInfo(protocol)
end

function AnotherCtrl:OnMedalTakeOffInfo(protocol)
	self.data:SetMedalTakeOffInfo(protocol)
end

function AnotherCtrl:OnEquipmentInfoAck(protocol)
	self.data:SetEquipmentInfoAck(protocol)
end

function AnotherCtrl:OnEquipmentGridChange(protocol)
	self.data:SetEquipmentGridChange(protocol)
end

function AnotherCtrl:OnJewelryAllInform(protocol)
	self.data:SetJewelryAllInform(protocol)
end

function AnotherCtrl:OnJewelryInfoChange(protocol)
	self.data:SetJewelryInfoChange(protocol)
end

function AnotherCtrl:OnElementCrystalChangeNotice(protocol)
	self.data:SetElementCrystalChangeNotice(protocol)
end

function AnotherCtrl:OnKnapsackInfoAck(protocol)
	self.data:SetKnapsackInfoAck(protocol)
	if ViewMgr:IsOpen(CrystalView) then
		ViewMgr:FlushView(CrystalView)
	end
end

function AnotherCtrl:OnKnapsackItemChange(protocol)
	self.data:SetKnapsackItemChange(protocol)
	if ViewMgr:IsOpen(CrystalView) then
		ViewMgr:FlushView(CrystalView)
	end
end

--===================================化身宠物===================================
function AnotherCtrl.SendPetOp(req_type,param1,param2)
	local protocol = GetProtocol(CSHuaShenPetOp)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function AnotherCtrl.SendPetPointReq(index, param_t)
	local protocol = GetProtocol(CSHuaShenPetAssignAttributePoints)
	-- protocol.req_type = req_type or 0
	protocol.index = index or 0
	protocol.con = param_t.con or 0
	protocol.str = param_t.str or 0
	protocol.def = param_t.def or 0
	protocol.agi = param_t.agi or 0
	protocol.magic = param_t.magic or 0
	SendProtocol(protocol)
end

function AnotherCtrl:OnPetAllInfo(protocol)
	self.data:SetPetAllInfo(protocol)
	for k,v in pairs(protocol.pet_list) do
		self.SendPetOp(AnotherPetOpType.PetAttrsReq, v.index)
	end
end

function AnotherCtrl:OnPetSingleInfo(protocol)
	if protocol.pet_info.config ~= nil then
		self.data:SetPetSingleInfo(protocol.pet_info)
		self.SendPetOp(AnotherPetOpType.PetAttrsReq, protocol.pet_info.index)
	end
end

function AnotherCtrl:OnPetOtherInfo(protocol)
	self.data:SetPetOtherInfo(protocol.pet_other_info)
end

function AnotherCtrl:OnPetAttrListInfo(protocol)
	local info = self.data:GetPetInfo(protocol.index)
	local old_list = self.data:GetPetAttrInfo(protocol.index)
	if ViewMgr:IsOpen(AnotherView) and old_list ~= nil and info ~= nil then
		local title_info = {fromType = AttrChangeData.ChangeFromType.Pet,showName = info:Name()}
		local change_list = DataHelper.GetChangeList(old_list,protocol.attr_list)
		MainOtherCtrl.Instance:OpenAttrChange(change_list,title_info)
	end
	self.data:SetPetAttrList(protocol.index, protocol.attr_list)
end

function AnotherCtrl:SendRemoveEquip()
    local pet = self.data:GetCurSelect()
    self.SendPetOp(AnotherPetOpType.PetEquipmentPutOff, pet.index)
end

function AnotherCtrl:SelectFunc(param_t)
	if self.select_func then
		self.select_func(param_t)
	end
end

function AnotherCtrl:SetSelectFunc(func)
	self.select_func = func
end