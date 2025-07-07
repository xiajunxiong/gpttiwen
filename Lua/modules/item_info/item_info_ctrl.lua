
FILE_REQ("modules/item_info/item_info_config")
FILE_REQ("modules/item_info/item_info_data")
FILE_REQ("modules/item_info/getway_data")
FILE_REQ("modules/item_info/link_mark_data")

VIEW_REQ("modules/item_info/item_info_view_details")
VIEW_REQ("modules/item_info/item_info_view_funcs")
VIEW_REQ("modules/item_info/item_info_view_items")
VIEW_REQ("modules/item_info/item_info_view")
VIEW_REQ("modules/item_info/item_sell_view")
VIEW_REQ("modules/item_info/item_pre_range_view")
VIEW_REQ("modules/item_info/item_pre_rangea_view")
VIEW_REQ("modules/item_info/item_pre_range_single_view")
VIEW_REQ("modules/item_info/item_heal_view")
VIEW_REQ("modules/item_info/item_getway_view")

VIEW_REQ("modules/item_info/activity_info_view")
VIEW_REQ("modules/item_info/attr_explain_view")
VIEW_REQ("modules/item_info/money_box_view")

VIEW_REQ("modules/item_info/item_rank_item")
VIEW_REQ("modules/item_info/batch_use_view")
VIEW_REQ("modules/item_info/quick_buy_normal")

ItemInfoCtrl = ItemInfoCtrl or BaseClass(BaseCtrl)

function ItemInfoCtrl:__init()
	if ItemInfoCtrl.Instance ~= nil then
		Debuger.LogError("[ItemInfoCtrl] attempt to create singleton twice!")
		return
	end

	ItemInfoCtrl.Instance = self
	self.data = ItemInfoData.New()
	self.link_mark = LinkMarkData.New()
end

function ItemInfoCtrl:__delete()
	if ItemInfoCtrl.Instance == self then
		ItemInfoCtrl.Instance = nil
	end
end
function ItemInfoCtrl:OnUnloadGameLogic()
	self.data.quick_buy_tips = {}
end

-- info = {
-- 	-- Item及其派生类对象List
-- 	item_list,
-- 	-- 按钮TypeList(nil不显示按钮)
-- 	func,
--  show_send = false
-- }

function ItemInfoCtrl:ItemInfoView(info)
	self.data:SetItemInfo(info)
	if info.item_list and info.item_list[1].item_id ~= 0 then
		info.item_list[1].item_type = info.item_list[1].item_type or Item.GetType(info.item_list[1].item_id)
		if GetWayData.CheckQuickGetWay(info.item_list[1].item_id) then 
			ViewMgr:OpenView(GetWayView, {item = info.item_list[1],func = info.func})
		else
		    ViewMgr:OpenView(ItemInfoView)
		end 
	end
	-- 打开物品详情是玩家的主动操作，取消自动任务之类的
	BehaviorData.Instance.auto_behavior = 0
end

function ItemInfoCtrl:ItemDeriveView(derive_view, param)
	if nil ~= self.data.derive_view then
		ViewMgr:CloseView(self.data.derive_view)
		self.data:ItemDeriveView(true)
	end
	self.data.derive_view = derive_view
	if nil ~= derive_view then
		self.data:ItemDeriveView(false)
		ViewMgr:OpenView(derive_view, param)
	end
	ViewMgr:FlushView(ItemInfoView)
	if ViewMgr:IsOpen(GetWayView) then ViewMgr:FlushView(GetWayView) end 
end

-- info = {
-- 	info = ActivityInfo,
-- 	pos = Vector2 or Vector2.zero
-- }

function ItemInfoCtrl:ActivityInfoView(info)
	self.data:SetActivityInfo(info)
	ViewMgr:OpenView(ActivityInfoView)
end
-- info = {
-- 	item_id = xxx,
--	pos = v3
-- }
function ItemInfoCtrl:GetWayInfoView(info)
	ViewMgr:OpenView(ItemGetWayView,info)
end

--出售请求
function ItemInfoCtrl:SendSellReq(item)
	BagCtrl.Instance:SendKnapsackOperaReq{
		oper_type = ItemInfoConfig.BagOperType.sell,
		param_1 = item.column_type,
		param_2 = item.index,
		param_3 = item.num or 1
	}
	ViewMgr:CloseView(ItemInfoView)
end

--外部调用ItemInfoUseFunc
function ItemInfoCtrl:ItemInfoUseFunc(param_t)
	local use_func = ItemInfoUseFunc[ItemType.Weapon]
	if use_func then
		use_func(param_t)
	else
		BagCtrl.Instance:SendUseItem(param_t)
	end
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
end

function ItemInfoCtrl:EffectShow(item)
	local info = {
		item_list = {item},
	}
	self.data:SetItemInfo(info)
	self.data.effect_show = true
	if info.item_list and info.item_list[1].item_id ~= 0 then
		info.item_list[1].item_type = info.item_list[1].item_type or Item.GetType(info.item_list[1].item_id)
		ViewMgr:OpenView(ItemInfoView)
	end
end
--param_t = {desc = "", item = item, use_func = func}
function ItemInfoCtrl:BatchUse(param_t)
	ViewMgr:OpenView(BatchUseView, param_t)
end
--param_t = {key = ItemInfoConfig.QuickBuyKey, desc "", item_list = {{item = item, lack = 0}, ...}, buy_func = func, auto_buy = true}
function ItemInfoCtrl:QuickBuyItems(param_t)
	ViewMgr:OpenView(QuickBuyNormal, param_t)
end
function ItemInfoCtrl:IsShowQuick(key)
	return self.data.quick_buy_tips[key]
end

function ItemInfoCtrl:ShowItemInfo(item_id)
    local item = Item.Create({item_id = item_id})
    local info = {
		item_list = {item},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end