FILE_REQ("modules/marbas/marbas_config")
FILE_REQ("modules/marbas/marbas_extral")
FILE_REQ("modules/marbas/marbas_data")
FILE_REQ("modules/marbas/marbas_plus_data")
FILE_REQ("modules/marbas/marbas_config")
VIEW_REQ("modules/marbas/marbas_view")
VIEW_REQ("modules/marbas/marbas_base")
VIEW_REQ("modules/marbas/marbas_bag")
VIEW_REQ("modules/marbas/marbas_build")
VIEW_REQ("modules/marbas/marbas_left_equip")
VIEW_REQ("modules/marbas/marbas_refine")
VIEW_REQ("modules/marbas/marbas_bathe")
VIEW_REQ("modules/marbas/marbas_bathe_select")
VIEW_REQ("modules/marbas/marbas_resolve")
VIEW_REQ("modules/marbas/marbas_blood")
VIEW_REQ("modules/marbas/marbas_blood_select")
VIEW_REQ("modules/marbas/marbas_blood_gem")
VIEW_REQ("modules/marbas/marbas_vigorous")
VIEW_REQ("modules/marbas/marbas_blood_tips")
VIEW_REQ("modules/marbas/marbas_vigorous_show_view")
VIEW_REQ("modules/marbas/marbas_demons")
VIEW_REQ("modules/marbas/marbas_neo_complete")
VIEW_REQ("modules/marbas/marbas_suit_show")
VIEW_REQ("modules/marbas/marbas_resolve_reward")
VIEW_REQ("modules/marbas/marbas_get_way")
VIEW_REQ("modules/marbas/marbas_gem_resolve_view")

-- 造化装备·ctrl组件
MarbasCtrl = MarbasCtrl or BaseClass(BaseCtrl)
function MarbasCtrl:__init()
	if MarbasCtrl.Instance ~= nil then
		Debuger.LogError("[MarbasCtrl] attempt to create singleton twice!")
		return
	end	
    MarbasCtrl.Instance = self

	self.data = MarbasData.New()
    self:RegisterAllProtocols()

	-- 角色登录追加协议
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,
    BindTool.Bind(self.OnRoleNoticeComplete, self))
	
end

function MarbasCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSZaoHuaEquipmentOperaReq)
	self:RegisterProtocol(SCZaoHuaBagListInfo,"RecvZaoHuaBagListInfo")
	self:RegisterProtocol(SCZaoHuaBagItemChange,"RecvZaoHuaBagItemChange")
	self:RegisterProtocol(SCZaoHuaEquipmentInfoAck,"RecvZaoHuaEquipmentInfoAck")
	self:RegisterProtocol(SCZaoHuaEquipmentGridChange,"RecvZaoHuaEquipmentGridChange")
	self:RegisterProtocol(SCZaoHuaEquipmentComposeResults,"RecvZaoHuaEquipmentComposeResults")
	self:RegisterProtocol(CSZaoHuaEquipmentReq)
	self:RegisterProtocol(SCZaoHuaReFineInfo,"RecvZaoHuaReFineInfo")
	self:RegisterProtocol(SCZaoHuaLongZhuInfo,"RecvZaoHuaLongZhuInfo")
	self:RegisterProtocol(SCZaoHuaLongZhuSingleInfo,"RecvZaoHuaLongZhuSingleInfo")
	self:RegisterProtocol(CSZaoHuaYuanHunZhuReq)
	self:RegisterProtocol(CSYuanHunZhuUpLevelReq)
	self:RegisterProtocol(SCZaoHuaAppearance,"RecvZaoHuaAppearance")
	self:RegisterProtocol(CSZaoHuaEquipmentDiscomposeReq)
	self:RegisterProtocol(SCZaoHuaEquipmentCommonDataInfo,"RecvZaoHuaEquipmentCommonDataInfo")
end

function MarbasCtrl:OnUnloadGameLogic()
	-- self.data:ClearNewItem()

	
	self.data:ClearData()
end

function MarbasCtrl:OnInit()
	-- 基础界面红点
	local base_remind_func = BindTool.Bind(self.FlushBaseRemind, self)
	RemindCtrl.Instance:Remind():Register(
        Mod.Marbas.Base,
        self.data.better_equip,
        base_remind_func
	)
	-- 打造界面红点
    local build_remind_func = BindTool.Bind(self.FlushBuildRemind, self)
	RemindCtrl.Instance:Remind():Register(
        Mod.Marbas.Build,
        self.data.build_remind,
        build_remind_func
	)
	RoleData.Instance:GetCurrencyData():Care(function()
		self.data.build_remind:Notify()
	end)
	BagData.Instance:ListenItems(self.data:BuildRemindItemList(), function()
		self.data.build_remind:Notify()
	end)
	-- 精炼界面红点
    local refine_remind_func = BindTool.Bind(self.FlushRefineRemind, self)
	RemindCtrl.Instance:Remind():Register(
        Mod.Marbas.Refine,
        self.data.refine_remind,
        refine_remind_func
	)
	BagData.Instance:ListenItems(self.data:RefineRemindItemList(), function()
		self.data.refine_remind:Notify()
	end)
	-- 珑铸界面红点
    local bathe_remind_func = BindTool.Bind(self.FlushBatheRemind, self)
	RemindCtrl.Instance:Remind():Register(
        Mod.Marbas.Bathe,
        self.data.bathe_remind,
        bathe_remind_func
	)
	BagData.Instance:ListenItems(self.data:BatheRemindItemList(), function()
		self.data.bathe_remind:Notify()
	end)
	-- 血祭界面红点
	local blood_remind_func = BindTool.Bind(self.FlushBloodRemind, self)
	RemindCtrl.Instance:Remind():Register(
        Mod.Marbas.Blood,
        self.data.equip_list,
        blood_remind_func
	)
	RemindCtrl.Instance:Remind():Register(
        Mod.Marbas.Blood,
        BagData.Instance.item_grid,
        blood_remind_func
	)
	-- 罡气界面红点
	local vigorous_remind_func = BindTool.Bind(self.FlushVigorousRemind, self)
	RemindCtrl.Instance:Remind():Register(
        Mod.Marbas.Vigorous,
        self.data.equip_list,
        blood_remind_func
	)
end

function MarbasCtrl:OnUnloadGameLogic()
    self.data:ResetAll()
end

function MarbasCtrl:FlushBuildRemind()
	return self.data:BuildAllRemind()
end

function MarbasCtrl:FlushBaseRemind()
	return self.data:BaseAllRemind()
end

function MarbasCtrl:FlushRefineRemind()
	return self.data:RefineAllRemind()
end

function MarbasCtrl:FlushBatheRemind()
	return self.data:BatheAllRemind()
end

function MarbasCtrl:FlushBloodRemind()
	return self.data:BloodAllRemind()
end

function MarbasCtrl:FlushVigorousRemind()
	return self.data:VigorousAllRemind()
end

function MarbasCtrl:SendEquipOper(oper,param_1,param_2,param_3)
    local protocol = GetProtocol(CSZaoHuaEquipmentOperaReq)
    protocol.oper_type = oper or 0
	protocol.param_1 = param_1 or 0
	protocol.param_2 = param_2 or 0
	protocol.param_3 = param_3 or 0
	-- LogDG("健哥哥,我要发协议了哦？1640",protocol)
    SendProtocol(protocol)
end

-- 更新造化背包
function MarbasCtrl:RecvZaoHuaBagListInfo(protocol)
	-- LogDG("健哥哥,是造化背包信息哦？1641",protocol)
	self.data:SetMarbasBag(protocol)
end 

-- 造化背包物品更新
function MarbasCtrl:RecvZaoHuaBagItemChange(protocol)
	-- LogDG("健哥哥,是造化背包个别更新哦？1642",protocol)
	self.data:SetItemData(protocol)
end 

-- 造化装备列表更新
function MarbasCtrl:RecvZaoHuaEquipmentInfoAck(protocol)
	-- LogDG("健哥哥,是造化装备列表更新哦？1643",protocol)
	self.data:SetEquipList(protocol)
end 

-- 造化装备孔位更新
function MarbasCtrl:RecvZaoHuaEquipmentGridChange(protocol)
	-- LogDG("健哥哥,是造化装备孔位更新哦？1644",protocol)
	self.data:ChangeEquipList(protocol)

	self.data:ShowDemonsSuccess()
end 

-- 造化装备打造返回
function MarbasCtrl:RecvZaoHuaEquipmentComposeResults(protocol)
	-- LogDG("健哥哥,是造化装备打造成功哦？1645",protocol)
    self.data:SetBuildResult(protocol)
    if Item.GetBigType(protocol.item.item_id) ~= ITEM_BIG_TYPE.MARBAS then
        return
    end 
    if protocol.result == 1 then
        TimeHelper:AddDelayTimer(function ()
            ViewMgr:OpenView(MarbasNeoComplete,{type = MarbasConfig.NeoType.Build})
        end, 0.1)
    end
end

-- 造化装备操作2请求
function MarbasCtrl:SendEquipOperPing(req_type, param1, param2, param3)
    local protocol = GetProtocol(CSZaoHuaEquipmentReq)
    protocol.req_type = req_type
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	protocol.param3 = param3 or 0
	-- LogDG("平哥哥,我要发协议了哦？5120",protocol)
    SendProtocol(protocol)
end

-- 精炼下发
function MarbasCtrl:RecvZaoHuaReFineInfo(protocol)
	-- LogDG("平哥哥,是造化装备精炼下发哦？5121",protocol)
	self.data:SetRefineInfo(protocol)
end

-- 珑铸登录或者解锁下发
function MarbasCtrl:RecvZaoHuaLongZhuInfo(protocol)
	-- LogDG("平哥哥,是造化装备珑铸登录下发哦？5124，部位0和部位1",protocol.info[0],protocol.info[1])
	-- LogDG("平哥哥,是造化装备珑铸登录下发哦？5124，部位2和部位3",protocol.info[2],protocol.info[3])
	-- LogDG("平哥哥,是造化装备珑铸登录下发哦？5124，部位4和部位5",protocol.info[4],protocol.info[5])
	-- LogDG("平哥哥,是造化装备珑铸登录下发哦？5124，部位6和部位7",protocol.info[6],protocol.info[7])
	self.data:SetBatheAllInfo(protocol)
end

-- 珑铸单个孔位下发
function MarbasCtrl:RecvZaoHuaLongZhuSingleInfo(protocol)
	-- LogDG("平哥哥,是造化装备珑铸单个孔位下发哦？5125",protocol)
	self.data:SetBatheSingleSlotInfo(protocol)
end

-- 造化·血祭装备 元魂穿脱
function MarbasCtrl:SendBloodGemOper(param)
    local protocol = GetProtocol(CSZaoHuaYuanHunZhuReq)
    protocol.req_type = param.req_type
	protocol.param1 = param.param1 or 0
	protocol.param2 = param.param2 or 0
	protocol.param3 = param.param3 or 0
	protocol.param4 = param.param4 or 0
	protocol.param5 = param.param5 or 0
	protocol.param6 = param.param6 or 0
	protocol.param7 = param.param7 or 0
	protocol.param8 = param.param8 or 0
	LogDG("平哥哥,我要发协议了哦？5122",protocol)
    SendProtocol(protocol)
end

-- 造化·血祭装备·元魂升级
function MarbasCtrl:SendBloodGemPlusOper(param)
    local protocol = GetProtocol(CSYuanHunZhuUpLevelReq)
	protocol.item_src = param.item_src or 0
	protocol.index = param.index or 0
	protocol.column = param.column or 0
	protocol.slot_index = param.slot_index or 0
	-- protocol.buy_item_num = param.buy_item_num or 0
	-- protocol.num = param.num or 0
	-- protocol.consume_list = param.consume_list or {}
	-- LogDG("平哥哥,我要发协议了哦？5123",protocol)
    SendProtocol(protocol)
end

function MarbasCtrl:SendZaoHuaEquipDisconposeReq(param)
	local protocol = GetProtocol(CSZaoHuaEquipmentDiscomposeReq)
	protocol.bag_list = param.bag_list
	-- LogDG("健哥哥,我要发协议了哦？1647",protocol)
    SendProtocol(protocol)
end

-- 造化·罡气下发
function MarbasCtrl:RecvZaoHuaAppearance(protocol)
	-- LogDG("健哥哥,这是罡气信息下发哦？1646",protocol)
	self.data:SetVigorousInfo(protocol)
end

-- 造化·通用信息下发
function MarbasCtrl:RecvZaoHuaEquipmentCommonDataInfo(protocol)
	-- LogDG("健哥哥,这是通用信息下发哦？1646",protocol)
	self.data:SetCommonData(protocol)
end

function MarbasCtrl:OnRoleNoticeComplete()
	self:SendEquipOper(0)

	-- if self.data:GetLevelShowEnoughNeed(Mod.Marbas.Build) then 
	--     FunOpen.Instance:SelfCondition(Mod.Marbas.Build, function()
    -- 		return self.data:GetLevelShowEnoughWithStr(Mod.Marbas.Build) and self.data:EquipedDemons() 
	--     end)
    -- end 

	FunOpen.Instance:SelfCondition(Mod.Marbas.Build, function()
		return  self.data:EquipedDemons() 
	end)

	if self.data:GetLevelShowEnoughNeed(Mod.Marbas.Refine) then 
		FunOpen.Instance:SelfCondition(Mod.Marbas.Refine, function()
			return self.data:GetLevelShowEnoughWithStr(Mod.Marbas.Refine) 
		end)
	end 

	if self.data:GetLevelShowEnoughNeed(Mod.Marbas.Bathe) then 
	    FunOpen.Instance:SelfCondition(Mod.Marbas.Bathe, function()
			return self.data:GetLevelShowEnoughWithStr(Mod.Marbas.Bathe)
		end)
	end 

	if self.data:GetLevelShowEnoughNeed(Mod.Marbas.Blood) then 
	    FunOpen.Instance:SelfCondition(Mod.Marbas.Blood, function()
		    return self.data:GetLevelShowEnoughWithStr(Mod.Marbas.Blood)
	    end)
	end 

	if self.data:GetLevelShowEnoughNeed(Mod.Marbas.Vigorous) then 
	    FunOpen.Instance:SelfCondition(Mod.Marbas.Vigorous, function()
		    return self.data:GetLevelShowEnoughWithStr(Mod.Marbas.Vigorous)
	    end)
	end 
end

function MarbasCtrl:TestSendPickDown()
	self:SendEquipOper(2,3)
end

function MarbasCtrl:TestSendPickUp()
	self:SendEquipOper(1,1)
end

function MarbasCtrl:GmYourMarbas(color,level)
	local list = self.data:SearchYourMarbas(color,level)
	for k,v in pairs(list) do 
		local gm_param = v.id .." 1 0"
		GMCmdCtrl.Instance:SendGMCommand("additem", gm_param)
	end 
end

-- 穿着元魂珠
function MarbasCtrl:GemEquip(Gem)
	local equip = self.data.blood_view_data.select

	local send_param = {
		req_type = BloodGemType.Put_On,
		param1 = equip.in_bag and BloodSrcType.EquipBag or BloodSrcType.Equiped,
		param2 = equip:GridIndex(),
		param3 = 0,
		param4 = self.data.blood_view_data.select_index,
		param5 = BloodSrcType.Bag,
		param6 = Gem:GridIndex(),
		param7 = Gem:GridColumn(),
		param8 = 0,
	}

	if ViewMgr:IsOpen(MarbasView) then 
		self.data.blood_view_data.param_send = send_param
		self.data.blood_view_data.select_gem = Gem
		self.data.blood_view_data.GemEff = not self.data.blood_view_data.GemEff 
	else 
		self:SendBloodGemOper(send_param)
	end 
	
end

function MarbasCtrl:GemChange(Gem)
	self.data.blood_view_data.need_pick = true
end

-- 摘除元魂珠
function MarbasCtrl:GemUnfix(Gem,equip,select_pos)
	local equip = equip ~= nil and equip or self.data.blood_view_data.select
	local select_pos = select_pos ~= nil and select_pos or self.data.blood_view_data.select_index
	LogError("ssss",select_pos)
	self:SendBloodGemOper({
		req_type = BloodGemType.Take_Off,
		param1 = equip.in_bag and BloodSrcType.EquipBag or BloodSrcType.Equiped,
		param2 = equip:GridIndex(),
		param3 = 0,
		param4 = select_pos,
		param5 =  equip.in_bag and BloodSrcType.EquipBag or BloodSrcType.Equiped,
		param6 = Gem:GridIndex(),
		param7 = Gem:GridColumn(),
		param8 = 0,
	})
end

function MarbasCtrl:GemCompose(gem)
	local comp_cfg= self.data:GetComposeCfgByStuff(gem.item_id)
	if comp_cfg == nil then return end 

	if gem:Num() < comp_cfg.use_num then 
		PublicPopupCtrl.Instance:Center(string.format(Language.Marbas.GemComppseLackStuff,Item.GetQuaName(comp_cfg.id,true)))    	
		return 
	end 
	
	local function send_msg()
		self:SendEquipOperPing(MarbasReqTypeP.GemCompose,comp_cfg.id)
		ViewMgr:CloseView(DialogTipsView)
    end

	local info = {
		content = string.format(
			Language.Marbas.GemComposeTips,comp_cfg.use_num,Item.GetQuaName(comp_cfg.use_item,true),Item.GetQuaName(comp_cfg.id,true)),
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = send_msg
		}
	}
	PublicPopupCtrl.Instance:DialogTips(info)
end

function MarbasCtrl:GemDecompose(gem)
	local decomp_cfg = self.data:GetDeComposeCfgByTarget(gem.item_id)
	if decomp_cfg == nil then return end 
	

	local function send_msg()
		self:SendEquipOperPing(MarbasReqTypeP.GemDecompose,gem:GridIndex(),gem:GridColumn(),0)
		ViewMgr:CloseView(DialogTipsView)
    end

	local str = string.format(Language.Marbas.GemDecomposeTips,gem:QuaName(),decomp_cfg.return_money_num,Item.GetQuaName(HuoBi[CurrencyType.CanHun]))
	if decomp_cfg.return_item > 0 then 
		str = str .. string.format(Language.Marbas.GemDecomposeTips2,decomp_cfg.return_num,
		    Item.GetQuaName(decomp_cfg.return_item,true))
	end 

	local info = {
		content = str,
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = send_msg
		}
	}
	PublicPopupCtrl.Instance:DialogTips(info)
end