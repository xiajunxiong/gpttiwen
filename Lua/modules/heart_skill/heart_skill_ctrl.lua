FILE_REQ("modules/heart_skill/heart_skill_data")
VIEW_REQ("modules/heart_skill/heart_skill_view")
VIEW_REQ("modules/heart_skill/heart_skill_change_view")

HeartSkillCtrl = HeartSkillCtrl or BaseClass(BaseCtrl)

function HeartSkillCtrl:__init()
	if HeartSkillCtrl.Instance ~= nil then
		Debuger.LogError("[HeartSkillCtrl] attempt to create singleton twice!")
		return
	end	
    HeartSkillCtrl.Instance = self
    self.data = HeartSkillData.New()
	self:RegisterAllProtocols()
end

function HeartSkillCtrl:OnUnloadGameLogic()
end

function HeartSkillCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSHeartSkillReq)
	self:RegisterProtocol(CSHeartSkillDisplaceReq)
	self:RegisterProtocol(SCHeartSkillAllInfo,"OnHeartSkillAllInfo")
	self:RegisterProtocol(SCHeartSkillSingleInfo,"OnHeartSkillSingleInfo")
	self:RegisterProtocol(SCHeartSkillOtherInfo,"OnHeartSkillOtherInfo")
	local heart_skill_func = function()
		return self.data:GetRemindNum()
	end
	Remind.Instance:Register(Mod.Role.RoleHeartSkill, BagData.Instance.item_grid,heart_skill_func)
	Remind.Instance:Register(Mod.Role.RoleHeartSkill, RoleData.Instance.base_data,heart_skill_func,"level")
	Remind.Instance:Register(Mod.Role.RoleHeartSkill, self.data.red_point_data,heart_skill_func)
end

function HeartSkillCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSHeartSkillReq)
	self:UnRegisterProtocol(CSHeartSkillDisplaceReq)
	self:UnRegisterProtocol(SCHeartSkillAllInfo)
	self:UnRegisterProtocol(SCHeartSkillSingleInfo)
	self:UnRegisterProtocol(SCHeartSkillOtherInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Role.RoleHeartSkill)
end

function HeartSkillCtrl:__delete()
	self:UnRegisterAllProtocols()
	HeartSkillCtrl.Instance = nil
end

--[[
HEART_SKILL_REQ_TYPE : 0  //!< 请求下发所有信息
HEART_SKILL_REQ_TYPE : 1  //!< 请求学习心法技能 P1 heart_skill_seq	P2 HEART_SKILL_LEARN_TYPE
HEART_SKILL_REQ_TYPE : 2  //!< 请求装备心法		P1 SKIll_STATUS(HEART_SKILL_STATUS_MAX) P2 INDEX(对照2217技能列表)
HEART_SKILL_REQ_TYPE : 3  //!< 请求拆卸心法		P1 SKIll_STATUS(HEART_SKILL_STATUS_MAX)
HEART_SKILL_REQ_TYPE : 4  //!< 请求合成		P1 compose_target
HEART_SKILL_REQ_TYPE : 5  //!< 请求转换		P1 column P2 index P3 target_item_id
HEART_SKILL_REQ_TYPE : 6  //!< 请求解锁技能格子	p1 SKIll_STATUS(HEART_SKILL_STATUS_MAX)			
]]
function HeartSkillCtrl:SendHeartSkillReq(op_type,param1,param2,param3)
	local protocol = GetProtocol(CSHeartSkillReq)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	protocol.param3 = param3 or 0
	SendProtocol(protocol)
end

function HeartSkillCtrl:SendDisplaceReq(info_list)
	local protocol = GetProtocol(CSHeartSkillDisplaceReq)
	protocol.count = #info_list or 0
	protocol.info_list = info_list or {}
	SendProtocol(protocol)
end

function HeartSkillCtrl:OnHeartSkillAllInfo(protocol)
	self.data:SetHeartSkillAllInfo(protocol)
end

function HeartSkillCtrl:OnHeartSkillSingleInfo(protocol)
	local info = protocol.info or {}
	self.data:SetHeartSkillSingleInfo(protocol)
	if info.level == 1 then
		local config = self.data:GetSkillConfig(info.seq)
		SkillData.Instance:PushNewSkillPopup({skill_id = config.heart_skill_id,skill_level = info.level,is_heart_skill = true})
		ViewMgr:OpenView(NewSkillPopupView)
	else
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
	end
end

function HeartSkillCtrl:OnHeartSkillOtherInfo(protocol)
	self.data:SetHeartSkillOtherInfo(protocol)
end