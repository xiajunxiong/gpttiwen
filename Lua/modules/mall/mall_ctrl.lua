FILE_REQ("modules/mall/mall_config")
FILE_REQ("modules/mall/mall_data")
VIEW_REQ("modules/mall/mall_view")
VIEW_REQ("modules/mall/mall_items")
VIEW_REQ("modules/mall/pet_detail_view")
VIEW_REQ("modules/mall/pet_detail_small_view")
VIEW_REQ("modules/mall/mall_other")
VIEW_REQ("modules/mall/sell_view")
--商城系统
MallCtrl = MallCtrl or BaseClass(BaseCtrl)

function MallCtrl:__init()
	if MallCtrl.Instance ~= nil then
		Debuger.LogError("[MallCtrl] attempt to create singleton twice!")
		return
	end
	MallCtrl.Instance = self
	self.data = MallData.New()
end

function MallCtrl:__delete()
	MallCtrl.Instance = nil
end

--货币不足的跳转提示
function MallCtrl.IsNotCurrency(money_type,money_price,item_id)
	if money_type == CurrencyType.Gold then
		if RoleData.Instance:GetCurrency(CurrencyType.Gold) < money_price then
			local info = {
				content = Language.Mall.NotCurrency[3],
				close = true,
				cancel = {
					name = Language.Mall.NotCurrency[4],
					func = function()
						TradeData.Instance:SetJumpItemData(Item.Init())
						ViewMgr:OpenViewByKey(Mod.Mall.Trade)
						PublicPopupCtrl.Instance:CloseDialogTips()
					end
				},
				confirm = {
					name = Language.Mall.NotCurrency[5],
					func = function()
						ViewMgr:OpenViewByKey(Mod.Store.Recharge)
						PublicPopupCtrl.Instance:CloseDialogTips()
					end
				},
			}
			BehaviorData.Instance.auto_behavior = 0
			if GLOBAL_CONFIG:Flags().block_recharge then
				info.no_confirm = true
			end
			PublicPopupCtrl.Instance:DialogTips(info)
			return false
		end
	elseif money_type == CurrencyType.CoinBind then
		if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) < money_price then
			PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2],function()
				ShoppingCtrl.Instance:OpenStoreView(0)
			end)
			return false
		end
	elseif money_type == CurrencyType.ImmortalCoin then
		if RoleData.Instance:GetCurrency(CurrencyType.ImmortalCoin) < money_price then
			MallData.JumpItemNum = MarketData.Instance:GetNeedGoldNum(money_price)
			CurrencyTypeConfig[CurrencyType.ImmortalCoin].on_click_add()
			return false
		end
	elseif HuoBi[money_type] ~= nil then
		if RoleData.Instance:GetCurrency(money_type) < money_price then
			MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = HuoBi[money_type]})})
			return false
		end
	elseif item_id ~= nil then
		if Item.GetNum(item_id) < money_price then
			MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = item_id})})
			return false
		end
	end
	return true
end

--【废弃 商店跳转统一接口 item_id:物品id is_auto 是否自动找商店npc  避免使用这个接口，最好使用GetWayData.OnOpenPanel这个接口实现跳转】
function MallCtrl:Jump(item_id,is_auto,mod_key)
	local key,config = self.data:GetShopItemConfig(item_id,mod_key)
	if key ~= 0 then
		if self.data.shop_config_func[key] then
			self.data.shop_config_func[key](config,is_auto)
		end
		if tonumber(key) then
			ViewMgr:OpenViewByKey(key)
		end
	else
		LogError("商店配置里没有这个道具: " .. item_id)
	end
	return key
end

function MallCtrl:OpenGetView(price_type)
	if price_type == CurrencyType.Chivalrous then
        ViewMgr:OpenView(TeamKnightlyView)
    elseif price_type == CurrencyType.Colosseum then
        ViewMgr:OpenViewByKey(Mod.Arena.Main)
    elseif price_type == CurrencyType.Arena then
        ViewMgr:OpenViewByKey(Mod.ArenaMatch.Arena)
    elseif price_type == CurrencyType.Blessing then
        if not FunOpen.Instance:GetFunIsOpened(Mod.Reward.FortuneBag) then
            PublicPopupCtrl.Instance:Center(Language.Store.FortuneNoOpenTip)
            return
        end
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FORTUNE_BAG) then
            PublicPopupCtrl.Instance:Center(Language.Store.FortuneNoOpenTip)
            return
        end
        ViewMgr:OpenViewByKey(Mod.Reward.FortuneBag)
    elseif price_type == CurrencyType.LeaderCoin then
		ViewMgr:OpenView(TeamSalaryView)
	elseif price_type == CurrencyType.CloudArena then
		ViewMgr:CloseView(StoreView)
		CloudArenaCtrl.Instance:OpenCloudArenaView()
	elseif price_type == CurrencyType.DustCoin then
		ViewMgr:CloseView(EquipView)
		ViewMgr:CloseView(StoreView)
		ViewMgr:OpenViewByKey(Mod.DeepDarkFantasy.Main)
	elseif price_type == CurrencyType.PartnerLevelItem then
		ViewMgr:CloseView(StoreView)
		ViewMgr:OpenView(FightFBView)
    end
end

function MallCtrl.IsCanRequire(type,value)
    if type == 1 then
        return RoleData.Instance:GetRoleLevel() < value
    elseif type == 2 then
		return PrestigeData.Instance:GetPrestigeLevel(1) < value
	elseif type == 3 then
		return ArenaData.Instance:GetShopRequire(value)
	elseif type == 4 or type == 5 then
		return CloudArenaData.Instance:GetShopRequire(type, value)
	elseif type == 6 then
		return BattlefieldData.Instance:IsShopRequireTip(value)
	elseif type == 7 then
		local info = PartnerData.Instance:GetPartnerInfoById(value)
		return PartnerConfig.LockState.lock == info.vo.is_unlocked
    end
    return false
end