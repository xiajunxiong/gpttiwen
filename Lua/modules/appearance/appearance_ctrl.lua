FILE_REQ("modules/appearance/appearance_data")
FILE_REQ("modules/appearance/appearance_config")
VIEW_REQ("modules/appearance/appearance_view")
VIEW_REQ("modules/appearance/appearance_main_view")
VIEW_REQ("modules/appearance/appearance_public_view")
VIEW_REQ("modules/appearance/appearance_bubbling_view")
VIEW_REQ("modules/appearance/appearance_title_view")
VIEW_REQ("modules/appearance/appearance_tail_tag_view")
VIEW_REQ("modules/appearance/appearance_show_view")

AppearanceCtrl = AppearanceCtrl or BaseClass(BaseCtrl)

function AppearanceCtrl:__init()
	if AppearanceCtrl.Instance ~= nil then
		Debuger.LogError("[AppearanceCtrl] attempt to create singleton twice!")
		return
    end
	AppearanceCtrl.Instance = self
	-- 是否拦截激活窗口，涉及转盘抽奖类的要延迟自己做展示
	self.is_shield_appear_show = false
	-- 拦截的激活窗口的消息
	self.shield_param_t = nil
    self.data = AppearanceData.New()
	self:RegisterAllProtocols()
end

function AppearanceCtrl:__delete()
	self:UnRegisterAllProtocols()
    AppearanceCtrl.Instance = nil
	Delete(self,"data")    
end

function AppearanceCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSFaceScoreRequest)
	self:RegisterProtocol(SCFaceScoreAllInfo,"OnFaceScoreAllInfo")
	self:RegisterProtocol(SCFaceListGridInfo,"OnFaceListGridInfo")
	self:RegisterProtocol(SCFaceResStateInfo,"OnFaceResStateInfo")
	self:RegisterProtocol(SCFaceLevelAndExps,"OnFaceLevelAndExps")
	self:RegisterProtocol(SCFaceTimeAllInfo,"OnFaceTimeAllInfo")
	self:RegisterProtocol(SCFaceTimeSingleInfo,"OnFaceTimeSingleInfo")
	--注册红点逻辑
	RemindCtrl.Instance:Remind():Register(Mod.Appearance.Bubbling,self.data.bubble_red_point,function()
		return self.data:GetBubbleRedPointNum()
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Appearance.Title,self.data.title_red_point,function()
		return self.data:GetTitleRedPointNum()
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Appearance.TailTag,self.data.tail_tag_red_point,function()
		return self.data:GetTailTagRedPointNum()
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Illusion.Appearance, self.data.remind_red_point, function ()
		return self.data:GetRedPointNum()
	end)
end

function AppearanceCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSFaceScoreRequest)
	self:UnRegisterProtocol(SCFaceScoreAllInfo)
	self:UnRegisterProtocol(SCFaceListGridInfo)
	self:UnRegisterProtocol(SCFaceLevelAndExps)
	self:UnRegisterProtocol(SCFaceTimeAllInfo)
	self:UnRegisterProtocol(SCFaceTimeSingleInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Appearance.Bubbling)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Appearance.Title)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Appearance.TailTag)
end

--操作请求
function AppearanceCtrl:SendAddReq(op_type,param1,param2)
	local protocol = GetProtocol(CSFaceScoreRequest)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0         
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

----------------------服务端数据下发----------------------
--所有信息
function AppearanceCtrl:OnFaceScoreAllInfo(protocol)
	self.data:SetFaceScoreAllInfo(protocol)
end

function AppearanceCtrl:OnFaceListGridInfo(protocol)
	self.data:SetFaceListGridInfo(protocol)
end

function AppearanceCtrl:OnFaceResStateInfo(protocol)
	self.data:SetFaceResStateInfo(protocol)
	if protocol.status == 1 then
		local appearance_cfg = Config.face_cfg_auto.resource[protocol.res_seq]
		if appearance_cfg ~= nil and appearance_cfg.add_exp > 0 then
			TimeHelper:AddDelayTimer(BindTool.Bind(self.OpenAppearanceShowFace, self, appearance_cfg), 0.5)
		end
	end
end

function AppearanceCtrl:OnFaceLevelAndExps(protocol)
	self.data:SetFaceLevelAndExps(protocol)
end

--颜值系统 道具时间戳下发
function AppearanceCtrl:OnFaceTimeAllInfo(protocol)
	self.data:SetFaceTimeAllInfo(protocol)
end

--颜值系统 单个道具时间戳下发
function AppearanceCtrl:OnFaceTimeSingleInfo(protocol)
	self.data:SetFaceTimeSingleInfo(protocol)
end

--[[
	show_type, 展示类型
	show_param,展示参数
	item_name,名字
	attr_list,属性列表
	click_func,点击回调
	desc_str,显示文字
]]
function AppearanceCtrl:OpenAppearanceShowView(param_t)
	if not self.is_shield_appear_show then
		ViewMgr:OpenView(AppearanceShowView, param_t)
	else
		self.shield_param_t = param_t
	end
end

-- 将拦截的激活窗口的消息放出来
function AppearanceCtrl:ReleaseShieldParam()
	if self.shield_param_t then
		ViewMgr:OpenView(AppearanceShowView, self.shield_param_t)
	end
	self.is_shield_appear_show = false
	self.shield_param_t = nil
end

function AppearanceCtrl:OpenAppearanceShowIllusion(id)
	local info = IllusionData.Instance:GetFashionInfoById(id).info
	local param_t = {show_type = AppearanceShowViewType.Illusion}
	if info.type == IllusionConfig.FashionType.cloth or info.type == IllusionConfig.FashionType.weapon then
		param_t.item_name = IllusionData.Instance:GetItemName(info.active_stuff_id)
	else
		param_t.item_name = info.surface_name
	end
	param_t.attr_list = {}
	local surface_cfg = Cfg.SurfaceValue(id, RoleData.Instance:GetRoleProf())
	for k, v in pairs(AttrNameForType) do
		if surface_cfg[v] ~= nil and surface_cfg[v] ~= 0 then
			table.insert(param_t.attr_list, {name = Language.Illusion.AttrNames[v][1] .. ":", value = "+" .. surface_cfg[v],})
		end
	end
	local appearance = TableCopy(RoleData.Instance:GetApearance())
	if info.type == IllusionConfig.FashionType.cloth then
		appearance.color = 0
		appearance.fashion_body_id = info.res_id
		appearance.special_appearance_type = 0
        appearance.special_appearance_param = 0
	elseif info.type == IllusionConfig.FashionType.weapon then
		appearance.weapon_id = 2001 + RoleData.Instance:ProfType() * 100
		appearance.fashion_weapon_id = info.res_id
	elseif info.type == IllusionConfig.FashionType.mount then
		appearance.ride_id = info.res_id
	end
	param_t.show_param = {
		is_ride = info.type == IllusionConfig.FashionType.mount,
		appearance = appearance,
	}
	param_t.click_func = function()
		ViewMgr:OpenViewByKey(Mod.Illusion.HuanHua)
	end
	self:OpenAppearanceShowView(param_t)
end

function AppearanceCtrl:OpenAppearanceShowPet(skin_cfg)
	local param_t = {show_type = AppearanceShowViewType.Pet}
	param_t.item_name = skin_cfg.name
	param_t.attr_list = {}
	for i, v in ipairs(Language.Pet.AttrClass) do
		local value = skin_cfg[v[1]]
		if value > 0 then
			table.insert(param_t.attr_list, {name = v[3], value = "+" .. value})	
		end
	end
	if skin_cfg.potential > 0 then
		table.insert(param_t.attr_list, {name = Language.Pet.Potential .. ":", value = "+" .. skin_cfg.potential})	
	end
	param_t.show_param = {
		res_path = DrawerHelper.GetNpcPath(skin_cfg.res_id)
	}
	param_t.click_func = function()
		ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)
	end
	self:OpenAppearanceShowView(param_t)
end

function AppearanceCtrl:OpenAppearanceShowPartner(skin_id)
	local skin_cfg = PartnerData.Instance.partner_skin_ids[skin_id]
	if skin_cfg == nil then
		return
	end
	local param_t = {show_type = AppearanceShowViewType.Partner}
	param_t.item_name = skin_cfg.name
	param_t.attr_list = {}
	local add_list = DataHelper.FormAttrList(skin_cfg)
	for i, v in ipairs(add_list) do
		table.insert(param_t.attr_list, {name = v.attr_name .. ":", value = "+" .. v.attr_value})
	end
	param_t.show_param = {
		res_path = DrawerHelper.GetNpcPath(skin_cfg.res_id)
	}
	param_t.click_func = function()
		ViewMgr:OpenViewByKey(Mod.Partner.PartnerAttr)
	end
	self:OpenAppearanceShowView(param_t)
end

function AppearanceCtrl:OpenAppearanceShowFace(appearance_cfg)
	local param_t = {show_type = AppearanceShowViewType.Face}
	local info = self.data:GetAppearancInfo()
	param_t.item_name = string.format(Language.Appearance.ShowFaceName1, info.face_level)
	param_t.attr_list = {}
	local up_cfg = self.data:GetCurAddData(info.face_level + 1)
	if up_cfg ~= nil then
		param_t.desc_str = Language.Appearance.ShowFaceName2
		for i, v in ipairs(up_cfg.add_list) do
			table.insert(param_t.attr_list, {name = v.attr_name .. ":", value = "+" .. v.attr_value})
		end
	else
		param_t.desc_str = ""
	end
	param_t.show_param = {
		res_id = appearance_cfg.res_id,
		res_type = appearance_cfg.res_type,
		title_id = appearance_cfg.title_id,
	}
	param_t.click_func = function()
		ViewMgr:OpenViewByKey(Mod.Appearance.Main)
	end
	self:OpenAppearanceShowView(param_t)
end
