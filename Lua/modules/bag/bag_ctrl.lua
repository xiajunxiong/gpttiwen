
FILE_REQ("modules/bag/bag_data")
FILE_REQ("modules/bag/bag_config")
VIEW_REQ("modules/bag/bag_view")
FILE_REQ("modules/storage/storage_config")
FILE_REQ("modules/medal/medal_config")
VIEW_REQ("modules/storage/storage_view")
VIEW_REQ("modules/bag/crystal_view")
VIEW_REQ("modules/bag/medal_view")
VIEW_REQ("modules/bag/temp_bag")
VIEW_REQ("modules/medal/medal_equip_view")
VIEW_REQ("modules/medal/medal_smelt_view")
VIEW_REQ("modules/medal/medal_fuling_view")
VIEW_REQ("modules/medal/medal_sp_tip")
VIEW_REQ("modules/medal/medal_atelier_view")
VIEW_REQ("modules/medal/medal_build_quick_buy")
VIEW_REQ("modules/medal/medal_build_rate_view")
VIEW_REQ("modules/medal/medal_build_panel")
VIEW_REQ("modules/medal/medal_lottrey_view")
VIEW_REQ("modules/medal/medal_upgrade")
VIEW_REQ("modules/medal/medal_effect_show_view")
FILE_REQ("modules/medal/medal_data")
FILE_REQ("modules/equipset/equipset_data")
VIEW_REQ("modules/medal/medal_upgrade_notice")
VIEW_REQ("modules/medal/medal_sp_change_view")
VIEW_REQ("modules/medal/medal_mix_view")
VIEW_REQ("modules/medal/medal_all_upgrade")
VIEW_REQ("modules/bag/resolve_view")
VIEW_REQ("modules/bag/equip_set_show_view")


BagCtrl = BagCtrl or BaseClass(BaseCtrl)
EQUIP_OPERA_TYPE = 
{
	OPERA_TYPE_INFO_REQ = 0,	--请求信息
	OPERA_TYPE_PUTON = 1,		--穿装备    param1  背包栏id  param2  背包格子序号  param3  穿戴的部位
	OPERA_TYPE_TAKEOFF = 2,		-- param1  要脱下的部位
	OPERA_TYPE_TAKEOFF_CRYSTAL = 3,--脱水晶
	OPERA_TYPE_VOC_LOCK = 4, -- 熔炼词条锁定
	OPERA_TYPE_INORDER = 5,				-- 6 整理背包
	OPERA_TYPE_RESONACE = 6, --神装共鸣激活
}

BAG_OPERA_TYPE = 
{
	OPERA_TYPE_REQUEST_INFO = 0, --请求背包信息 0 
	OPERA_TYPE_EXTEND = 1,		 --扩展背包 1 参数:param1  背包栏ID  param2  扩展格子数 
	OPERA_TYPE_DEPOSIT = 2,		 --存入仓库 2 参数:param1  背包栏ID  param2  物品所在序号  param3  仓库页号  
	OPERA_TYPE_WITHDRAW = 3,	 --取出仓库或临时背包 3 参数:param1  背包栏ID(仓库或临时背包)  param2  所在的序号  
	OPERA_TYPE_WITHDRAW_ALL = 4, --取出所有仓库或临时背包中的物品 4 参数:param1  背包栏ID(仓库或临时背包) 
	OPERA_TYPE_INORDER = 5,		 --整理背包 5 参数:param1  背包栏ID 
	OPERA_TYPE_DISCARD = 6,		 --丢弃(出售)物品 6 参数:param1  背包栏ID  param2  物品所在序号  param3  丢弃物品数量
}
-- 额外参数：0 - 无； 1 - 背包格子ID； 2 - 灵饰装备格ID；
JEWELRY_OPERA_TYPE =
{
	OPERA_TYPE_ALL_INFO_REQ = 0,		-- 请求所有 灵饰 信息
	OPERA_TYPE_WEAR_JEWELRY = 1,		-- 请求装备灵饰
	OPERA_TYPE_TAKE_OFF_ONE = 2,		-- 请求脱下灵饰
}

MEDAL_REQ_TYPE =
{
	INFO = 0,		--请求信息
	PUT_ON = 1,		--穿戴 p1 grid_index p2 column p3 index
	TAKE_OFF = 2,	--脱下装备 p1 grid_index
	EXCHANGE = 3,	--兑换 p1 seq
	DECOMPOSE = 4,  --分解 p1 column p2 index
	LOTTREY = 5,    --抽奖 p1 seq p2 times
	UPGRADE = 6,    --对背包进阶 p1 column p2 index p3 consume_column p4 consume_index
	UPGRADE_EQUIP = 7, -- 对穿戴进阶 p1 wear_index,p3 consume_column p4 consume_index
	SPE_EXCHANGE = 8, -- 天赐兑换 p1 column p2 index p3 target_sp_id p4 target_second_sp_id p5 is_wear(0背包，1穿戴)
}

function BagCtrl:__init()
	if BagCtrl.Instance ~= nil then
		Debuger.LogError("[BagCtrl] attempt to create singleton twice!")
		return
	end	
	BagCtrl.Instance = self
	self.data = BagData.New()
	self.medal_data = MedalData.New()
	self.equipset_data = EquipSetData.New()

	self:RegisterProtocol(CSKnapsackOperaReq)
	self:RegisterProtocol(CSUseItem)
	self:RegisterProtocol(CSUseOptionalGift)
	self:RegisterProtocol(CSUseOptionalGift2)
	self:RegisterProtocol(SCUseSpecialLogicItem)
	self:RegisterProtocol(SCActiveSpecialLogicItem)
	self:RegisterProtocol(SCHearsayInfoReq)
	-- 1500
	self:RegisterProtocol(SCKnapsackInfoAck,"RecvFullBag",true)
	self:RegisterProtocol(SCKnapsackItemChange,"RecvBagItemChange",true)
	self:RegisterProtocol(SCKnapsackExtend,"OnKnapsackExtend",true)
	self:RegisterProtocol(SCLackItem,"RecvLackItem",true)
	self:RegisterProtocol(SCItemIntoKnpackSpecialProtocal,"RecvItemIntoKnpackSpecialProtocal",true)
	self:RegisterProtocol(SCUseItemSuc,"RecvUseItemSuc")
	self:RegisterProtocol(SCKnapsackGetItemNotice,"RecvKnapsackGetItemNotice")
	self:RegisterProtocol(SCHongBaoUseRet,"OnHongBaoUseRet")
	-- 1600
	self:RegisterProtocol(SCEquipmentInfoAck,"RecvEquipmentInfo",true)
	self:RegisterProtocol(SCEquipmentGridChange,"RecvEquipmentGridChange",true)
	self:RegisterProtocol(SCElementCrystalChangeNotice,"RecvElementCrystalChangeNotice",true)
	self:RegisterProtocol(SCEquipmentLockInfo,"RecvEquipmentLockInfo",true)
	self:RegisterProtocol(SCEquipmentJewerlyUpGreadeInfo,"RecvSCEquipmentJewerlyUpGreadeInfo",true)
	self:RegisterProtocol(SCEquipmentJewerlyResonaceInfo,"RecvSCEquipmentJewerlyResonaceInfo",true)
	self:RegisterProtocol(CSEquipmentOperaReq)
	-- 1700
	self:RegisterProtocol(SCJewelryAllInform,"RecvJewelryAllInform",true)
	self:RegisterProtocol(SCJewelryInfoAlter,"RecvJewelryInfoAlter",true)
	self:RegisterProtocol(CSJewelryHandleReq)
	--1900
	self:RegisterProtocol(CSBagDecompose)
	-- 2100
	self:RegisterProtocol(SCMedalInfo, "RecvMedalInfo")
	self:RegisterProtocol(SCMedalTakeOff, "RecvMedalTakeOff")
	self:RegisterProtocol(SCMedalUpgradeNotice, "RecvMedalUpgradeNotice")
	self:RegisterProtocol(SCMedalSpecialExchangeNotice, "RecvMedalSpecialExchangeNotice")
	self:RegisterProtocol(CSMedalReq)
	self:RegisterProtocol(CSMedalOneKeyGradeReq)
	self:RegisterProtocol(SCLottreyInfo, "RecvLottreyInfo")
	self:RegisterProtocol(CSMedalUpGradeSpecialReq)

	GuideManager.Instance:RegisterGuideCheck(19,BindTool.Bind(self.CheckGuideFunc, self))

	--注册红点逻辑
	RemindCtrl.Instance:Remind():Register(Mod.Bag.Main,self.data.crystal_red_piont,function()
		if self.data:GetCrystalRedPiont() == 1 then
			return 1
		else
			return AccrssoryCtrl.Instance:GetAccrssoryRemind()
		end
	end)

	self.data:Care(function ()
		self.data:NotifyItems()
	end)
end
function BagCtrl:OnUnloadGameLogic()
	self.data:ClearNewItem()
	
	self.data:ClearData()
end

function BagCtrl:__delete()
	self:UnRegisterProtocol(SCKnapsackInfoAck)
	self:UnRegisterProtocol(SCKnapsackItemChange)
	self:UnRegisterProtocol(SCKnapsackExtend)
	self:UnRegisterProtocol(SCLackItem)
	self:UnRegisterProtocol(SCItemIntoKnpackSpecialProtocal)

	self:UnRegisterProtocol(SCEquipmentInfoAck)
	self:UnRegisterProtocol(SCEquipmentGridChange)
	self:UnRegisterProtocol(SCElementCrystalChangeNotice)
	
	self:UnRegisterProtocol(SCJewelryAllInform)
	self:UnRegisterProtocol(SCJewelryInfoAlter)
	
	RemindCtrl.Instance:Remind():UnRegister(Mod.Bag.Main)

	if BagCtrl.Instance == self then
		BagCtrl.Instance = nil
	end
end

function BagCtrl:CheckGuideFunc(step_cfg)
	if step_cfg.step_id == 19 and step_cfg.index == 1 then
        local item_id = GuideData.Instance:GetBgaEquipId()
		local index = self.data:GetBagIndexById(ItemColumnType.Item,item_id)
        if index == nil then
            GuideManager.Instance:ForceStop()
        end
    end
end

function BagCtrl:RecvFullBag(protocol)
	for col_t = ItemColumnType.Start, ItemColumnType.Count - 1 do
		local size = protocol.valid_grid_num_list[col_t]
		local extend_times = protocol.extend_times_list[col_t]
		self.data:SetColumnSize(col_t, size, extend_times)
	end
	for i = 1, protocol.item_count do
		self.data:SetItemData(protocol.item_list[i], protocol.reason)
	end
	self.data:ClearEmptyItem(protocol.item_list)
	FightData.Instance:CheckDungeonRedPonits()
end

function BagCtrl:OnKnapsackExtend(protocol)
	self.data:SetColumnSize(protocol.column, protocol.grid_num, protocol.extend_times)
end

function BagCtrl:RecvBagItemChange(protocol)
	self.data:SetItemData(protocol, protocol.reason, protocol.put_reason)
	if protocol.param and protocol.param.monster_id and protocol.param.monster_id > 0 then
		local param_t = {
			content = Language.SuperMap.MapTip,
			confirm = {
				func = function()
					ViewMgr:CloseView(Compass)
					BagCtrl.Instance:SendUseItem({column = protocol.column, index = protocol.index})
					MainOtherCtrl.Instance:CloseKeyUse()
					PublicPopupCtrl.Instance:CloseDialogTips()
				end
			}
		}
		PublicPopupCtrl.Instance:DialogTips(param_t)
	end
end

function BagCtrl:SendKnapsackOperaReq(param_t)
	local protocol = GetProtocol(CSKnapsackOperaReq)
	protocol.oper_type = param_t.oper_type or 0
	protocol.param_1 = param_t.param_1 or 0
	protocol.param_2 = param_t.param_2 or 0
	protocol.param_3 = param_t.param_3 or 0
	protocol.param_4 = param_t.param_4 or 0
	SendProtocol(protocol)
end

function BagCtrl:RecvLackItem(protocol)
	local item = Item.New()
	item.item_id = protocol.item_id
	PublicPopupCtrl.Instance:Center(string.format(ErrorText[5],item:Name(), protocol.count))
end

--背包获得道具下发
function BagCtrl:RecvItemIntoKnpackSpecialProtocal(protocol)
	-- LogError("RecvItemIntoKnpackSpecialProtocal",protocol)
	if BagConfig.GET_RESON_FUNC[protocol.get_reson] then
		BagConfig.GET_RESON_FUNC[protocol.get_reson](protocol)
	end
end

function BagCtrl:RecvUseItemSuc(protocol)
	if 24391 == protocol.item_id and protocol.num > 0 then
		local co = Item.GetConfig(protocol.item_id)
		PublicPopupCtrl.Instance:Center(string.format(Language.ItemInfo.UseTiLiTips, co.param1 * protocol.num))
	elseif protocol.num > 0 then
		local co = Item.GetConfig(protocol.item_id)
		if co ~= nil then
			local big_type = Item.GetBigType(protocol.item_id)
			if big_type == ITEM_BIG_TYPE.EXPENSE then
				if co.use_type == ItemUseType.ReduceKillValue then
					local kill_value = RoleData.Instance:GetBaseData().kill_value
					PublicPopupCtrl.Instance:Center(string.format(Language.ItemInfo.UseReduceKillValueTips, co.param1 * protocol.num, kill_value))
					return
				end
			end
		end
	end
end

function BagCtrl:RecvKnapsackGetItemNotice(protocol)
	local notice_type = protocol.notice_type
	local p1 = protocol.p1
	if notice_type == KnapsackGetItemNoticeType.Workshop then
		local item = Item.Create({item_id = protocol.item_id , num = protocol.num})
		PublicPopupCtrl.Instance:CenterI({item_id = protocol.item_id , num = protocol.num,color_str = item:ColorStr()})
	elseif notice_type == KnapsackGetItemNoticeType.LiveSkill then
		PublicPopupCtrl.CenterRich(Language.Live.GetAdditionTip, {sprite = "LvJianTou", w = 36, h = 42})
	elseif notice_type == KnapsackGetItemNoticeType.Deafult then
		local item = Item.Create({item_id = protocol.item_id , num = protocol.num})
		PublicPopupCtrl.Instance:CenterI({item_id = protocol.item_id , num = protocol.num,color_str = item:ColorStr()})
	end
end

function BagCtrl:OnHongBaoUseRet(protocol)
	ItemInfoData.Instance:SetHongBaoUseRet(protocol)
end

function BagCtrl:SendUseItem(param_t)
	local protocol = GetProtocol(CSUseItem)
	protocol.column = param_t.column or 0
	protocol.index = param_t.index or 0
	protocol.num = param_t.num or 1
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	protocol.param3 = param_t.param3 or 0
	SendProtocol(protocol)
	if ViewMgr:IsOpen(KeyUseView) and ViewMgr:IsOpen(BagView)then
		MainOtherCtrl.Instance:CloseKeyUse()
	end
end

--自选礼包操作
function BagCtrl:SendUseOptionalGift(column,index,select_info,num)
	local protocol = GetProtocol(CSUseOptionalGift)
	protocol.column = column or 0
	protocol.index = index or 0
	protocol.select_info = select_info or {}
	protocol.num = num or 1
	SendProtocol(protocol)
end

function BagCtrl:SendUseOptionalGift2(column, index, select_index_flag)
	local protocol = GetProtocol(CSUseOptionalGift2)
	protocol.column = column or 0
	protocol.index = index or 0
	protocol.select_index_flag = select_index_flag or 0
	SendProtocol(protocol)
end

function BagCtrl:SendUseSpecialLogicItem(notice_type, item_id, num)
	local protocol = GetProtocol(SCUseSpecialLogicItem)
	protocol.notice_type = notice_type or 0
	protocol.item_id = item_id or 0
	protocol.num = num or 1
	SendProtocol(protocol)
end

function BagCtrl:SendActiveSpecialLogicItem(notice_type, param1, param2)
	local protocol = GetProtocol(SCActiveSpecialLogicItem)
	protocol.notice_type = notice_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function BagCtrl:SendHearsayInfoReq(notice_type, param1, param2, param3)
	local protocol = GetProtocol(SCHearsayInfoReq)
	protocol.notice_type = notice_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	protocol.param3 = param3 or 0
	SendProtocol(protocol)
end

function BagCtrl:RecvEquipmentInfo(protocol)
	self.data:ViewData().element_crystal_id = protocol.element_crystal_id
	self.data:ViewData().used_times = protocol.used_times
	self.data:SetEquipList(protocol.equipment_list)
	self.data:SetCrystalRedPiont(protocol.element_crystal_id == 0)
end

function BagCtrl:RecvSCEquipmentJewerlyUpGreadeInfo(protocol)
	self.data:SetEquipSortUpgrade(protocol)
end

function BagCtrl:RecvSCEquipmentJewerlyResonaceInfo(protocol)
	DivineCostumeData.Instance:SetResonaceInfo(protocol)
end

function BagCtrl:RecvEquipmentGridChange(protocol)
	if protocol.reason ~= 0 then
		local item = Item.Create(protocol.equip_info)                      
        MainOtherCtrl.Instance:OpenGiftView({[1]= item}, GIFT_OPEN_TYPE.CONGRATULATIONS, true)
		EquipData.Instance:ClearEquipChange()
	end
	self.data:ChangeEquipList(protocol.equip_info)
end

function BagCtrl:RecvElementCrystalChangeNotice(protocol)
	self.data:ViewData().element_crystal_id = protocol.element_crystal_id
	self.data:ViewData().used_times = protocol.used_times
	ViewMgr:FlushView(CrystalView)
	self.data:SetCrystalRedPiont(protocol.element_crystal_id == 0)
end

function BagCtrl:RecvEquipmentLockInfo(protocol)
	self.data:SetEquipVocLockList(protocol)
end

function BagCtrl:SendEquipmentOperaReq(param_t)
	local protocol = GetProtocol(CSEquipmentOperaReq)
	protocol.oper_type = param_t.oper_type or 0
	protocol.param_1 = param_t.param_1 or 0
	protocol.param_2 = param_t.param_2 or 0
	protocol.param_3 = param_t.param_3 or 0
	protocol.param_4 = param_t.param_4 or 0
	SendProtocol(protocol)
end

function BagCtrl:RecvJewelryAllInform(protocol)
	self.data:SetJewelryList(protocol.jewelry_list)
end

function BagCtrl:RecvJewelryInfoAlter(protocol)
	if protocol.reason ~= 0 then
		local item = Item.Create(protocol.jewelry)                      
        MainOtherCtrl.Instance:OpenGiftView({[1]= item}, GIFT_OPEN_TYPE.CONGRATULATIONS, true)
		EquipData.Instance:ClearEquipChange()
	end
	self.data:ChangeJewelryList(protocol.jewelry)
end

--灵饰穿着
function BagCtrl:SendJewelryOperaReq(param_t)
	local protocol = GetProtocol(CSJewelryHandleReq)
	protocol.op_type = param_t.op_type or 0
	protocol.param = param_t.param or 0
	SendProtocol(protocol)
end

function BagCtrl:RecvMedalInfo(protocol)
	self.data:SetMedalInfo(protocol)
end

function BagCtrl:RecvMedalTakeOff(protocol)
	self.data:SetMedalTakeOff(protocol)
end

function BagCtrl:RecvMedalUpgradeNotice(protocol)
	self.medal_data:SetUpgradeNotice(protocol)
end

function BagCtrl:RecvMedalSpecialExchangeNotice(protocol)
	self.medal_data:SetSpecialExchangeNotice(protocol)
end

function BagCtrl:RecvLottreyInfo( protocol )
	self.medal_data:SetLottreyInfo(protocol)
end

function BagCtrl:SendMedalReq(param_t)
	local protocol = GetProtocol(CSMedalReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	protocol.param3 = param_t.param3 or 0
	protocol.param4 = param_t.param4 or 0
	protocol.param5 = param_t.param5 or 0
	SendProtocol(protocol)
end

function BagCtrl:SendMedalUpGradeSpecialReq(param_t)
	local protocol = GetProtocol(CSMedalUpGradeSpecialReq)
	protocol.is_wear_grid = param_t.is_wear_grid or 0
	protocol.column = param_t.column or 0
	protocol.index = param_t.index or 0
	-- 天赐
	protocol.consume_column_spe = param_t.consume_column_spe or 0
	protocol.consume_index_spe = param_t.consume_index_spe or 0
	-- 普通
	protocol.consume_column = param_t.consume_column or 0
	protocol.consume_index = param_t.consume_index or 0
	SendProtocol(protocol)
end

function BagCtrl:SendMedalOneKeyGradeReq(param_t)
	local protocol = GetProtocol(CSMedalOneKeyGradeReq)

	protocol.count = param_t.count or 0
	protocol.column = param_t.column or 0
	protocol.index_flag = param_t.index_flag or {}
	SendProtocol(protocol)
end

function BagCtrl:SendBagDecompose(column, index, num)
	local protocol = GetProtocol(CSBagDecompose)
	protocol.column = column or 0
	protocol.index = index or 0
	protocol.num = num or 1
	SendProtocol(protocol)
end

function BagCtrl:RichTextTest(param_t)
end

function BagCtrl:TryItemGetAudioEff()
	if not self.item_get_block then 
		AudioMgr:PlayEffect(AudioType.UI,"item_get")
		self.item_get_block = true
		self:BlockItemGetAudioEff()
	end 
end

function BagCtrl:BlockItemGetAudioEff()
	TimeHelper:AddDelayTimer(function ()
		self.item_get_block = false
	end, 3) 
end

--快捷使用请求
function BagCtrl:UseItemByIndex(column,index)
	local item = self.data:GetItemByIndex(column,index)
	if item and item ~= nil then
		if item:IsJewelry() then
			self:SendJewelryOperaReq({op_type = JEWELRY_OPERA_TYPE.OPERA_TYPE_WEAR_JEWELRY, param = item.index})
		else 
		    self:SendUseItem({column = item.column_type,index = item.index,num = item.num or 1})
		end 
	else
		MainOtherCtrl.Instance:CloseKeyUse()
	end
end

--礼包使用请求
function BagCtrl:SendUseGiftReq(param_t)
	if BagData.IsSelectGift(param_t.item_id) then
		ViewMgr:OpenView(ChooseGiftView,param_t)
	else
		self:SendUseItem(param_t)
	end
end