CowGoodLuckView = CowGoodLuckView or DeclareView("CowGoodLuckView","serveractivity/cow_good_luck_view",Mod.SpringFestival.CowGoodLuck)
function CowGoodLuckView:CowGoodLuckView()
end

function CowGoodLuckView:OpenCallback()
end

function CowGoodLuckView:LoadCallback(param_t)
	self.CurrencySub:DefaultSet()
	self.ToggleList[param_t.mod_key == Mod.SpringFestival.CowGoodLuck and 1 or 2].isOn = true
end

function CowGoodLuckView:CloseCallback()
end

function CowGoodLuckView:OnClickShowCurrency()
	self.CurrencySub:DefaultSet()
end

CowGoodLuckViewPanel = CowGoodLuckViewPanel or DeclareMono("CowGoodLuckViewPanel", UIWFlushPanel)
--EnterInter=============InteractorHandle
--Times==================Text
function CowGoodLuckViewPanel:CowGoodLuckViewPanel()
	self.cow_data = CowGoodLuckData.Instance	
	self.data_cares = {
		{data = self.cow_data.intimacy, func = self.FreshIntimacy,init_call = false},
		{data = self.cow_data.show_all, func = self.FreshShowAll,init_call = true},
		{data = self.cow_data.buy_count_change, func = self.FlushShopInfo,init_call = false},
	}

	self.ui_role_show = UIChDrawer.New()
	self.ModelShow:SetShowData({
		ui_obj = self.ui_role_show,
		view = self,
		show_shadow = false,
	})
    self.show_models = self.cow_data:GetShowRoleList()
    self.show_model_index = 1
	self.uptate_model_time = 10
end

function CowGoodLuckViewPanel:OnDestroy()
    TimeHelper:CancelTimer(self.time_func)
    UIWFlushPanel.OnDestroy(self)
	if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
		self.ui_obj = nil
    end
    if self.ui_role_show ~= nil then
        self.ui_role_show:DeleteMe()
		self.ui_role_show = nil
    end
end

function CowGoodLuckViewPanel:FreshShowAll()
	local cow_open = ActivityData.IsOpen(ACTIVITY_TYPE.RAND_COW_LUCKY_EXCHANGE)
	local shop_open = ActivityData.IsOpen(ACTIVITY_TYPE.RAND_COW_STORE)
	self.CowToggleObj:SetActive(cow_open)
	self.ShopToggleObj:SetActive(shop_open)
	self.TabEff[1]:SetActive(cow_open and shop_open)
	UH.LocalPosG(self.TabEff[2], (cow_open and shop_open) and Vector3.New(-545, -117, 0) or Vector3.New(-545, 88, 0))

	if cow_open then
		self:FreshIntimacy()
		self:FreshShowItem()
		self:ShowModel()
		-- self:FlushPetModel()
		-- self:FlushPetInfo()
	end
	if shop_open then
		self:FlushShopInfo()
	end
end

function CowGoodLuckViewPanel:FreshIntimacy()
	local need_intimacy = self.cow_data:GetIntimacyCanGet()
	local have_intimacy = self.cow_data.intimacy.num

	-- local pos_have = 135
	-- local pos_have_y = -148
	-- if have_intimacy >=1000 then
	-- 	pos_have = 155
	-- end
	-- if have_intimacy >=10000 then
	-- 	pos_have = 165
	-- 	pos_have_y = -160
	-- end
	-- UH.LocalPosG(self.TextPosHave,Vector3.New(pos_have, pos_have_y, 0)) 

	UH.SetText(self.NeedIntimacy,need_intimacy)
	UH.SetText(self.HaveIntimacy,have_intimacy)
	if have_intimacy >= need_intimacy then
		self.InteractorHandler.Interactable = true
	else
		self.InteractorHandler.Interactable = false
	end
	self.RedPos:SetNum(self.cow_data:GetIntimacyRemind())
end

function CowGoodLuckViewPanel:FreshShowItem()
	local item_list = self.cow_data:GetShowItemList()
	self.RewardShow:SetData(item_list)
end

function CowGoodLuckViewPanel:FlushPetModel()
	if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
        })
    end
	local res_id = self.cow_data:GetShowPetResId()
	self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id))
	self.ui_obj:PlayAnim(ChAnimType.Show)
end

function CowGoodLuckViewPanel:FlushPetInfo()
	local pet_name = self.cow_data:GetShowPetName()
	UH.SetText(self.showname, pet_name)
--[[	local total_attr = self.cow_data:GetShowPetInfo()
	for k, v in pairs(total_attr) do
        if tostring(k) == "maxhp" then
            self.ProgMaxHp:SetProgress(1)
            self.ProgMaxHp:SetText(string.format(Language.Common.Progress2TxtColorCFormat, v, v))
        elseif tostring(k) == "maxmp" then
            self.ProgMaxMp:SetProgress(1)
            self.ProgMaxMp:SetText(string.format(Language.Common.Progress2TxtColorCFormat,v, v))
		elseif self[tostring(k)] ~= nil then
            UH.SetText(self[tostring(k)], v)
        end
    end--]]
	local pet_cfg = self.cow_data:GetShowPetData():PetCfg()
	local main_arrt = self.cow_data:GetShowPetData():MainAttr()

	UH.SetText(self.ValueList[1],pet_cfg["con"].."/"..pet_cfg["con"])
	UH.SetText(self.ValueList[2],pet_cfg["str"].."/"..pet_cfg["str"])
	UH.SetText(self.ValueList[3],pet_cfg["def"].."/"..pet_cfg["def"])
	UH.SetText(self.ValueList[4],pet_cfg["agi"].."/"..pet_cfg["agi"])
	UH.SetText(self.ValueList[5],pet_cfg["magic"].."/"..pet_cfg["magic"])
   
	UH.Color(self.ValueList[main_arrt],COLORS.Red16)
	UH.Color(self.NameList[main_arrt],COLORS.Red16)
	local skill_list = self.cow_data:GetSkillAndGiftGroupList()
	self.GiftSkill:SetData(skill_list)
end

function CowGoodLuckViewPanel:FlushShopInfo()
	local shop_list = self.cow_data:SortGiftList()
	self.ShopList:SetData(shop_list)
end

function CowGoodLuckViewPanel:OnClickGetReward()
	if self.InteractorHandler.Interactable then
		ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_LUCKY_EXCHANGE,1)
	else
		PublicPopupCtrl.Instance:Error(Language.RACowGoodLuck.Limit)
	end
end

function CowGoodLuckViewPanel:OnClickGetAway()
	ViewMgr:OpenViewByKey(Mod.Activity.Daily)
	ViewMgr:CloseView(CowGoodLuckView)
	ViewMgr:CloseView(SpringFestivalView)
end

function CowGoodLuckViewPanel:OnClickClose()
	ViewMgr:CloseView(CowGoodLuckView)
end

function CowGoodLuckViewPanel:OnClickGift(cell)
    local data = cell:GetData()
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        ViewMgr:OpenView(PetSkillTip)
    elseif data.show_lingwu then
        PetLingwuSkillTip.SetSlotIndex(data.index)
        ViewMgr:OpenView(PetLingwuSkillTip)
    end
end

function CowGoodLuckViewPanel:OnClickPage(lerp)
	self.show_model_index = self.show_model_index + lerp
    self.show_model_index = self.show_model_index > #self.show_models and 1 or self.show_model_index
	self.show_model_index = self.show_model_index < 1 and #self.show_models or self.show_model_index
	self:ShowModel()
end

function CowGoodLuckViewPanel:ShowModel()
	if #self.show_models < 1 then
        return
    end
	if #self.show_models > 1 then
        TimeHelper:CancelTimer(self.time_func)
        self.time_func = Invoke(BindTool.Bind(self.AutoToNext,self), self.uptate_model_time)
    end
	local item_id = self.show_models[self.show_model_index]
	local item_cfg = Item.GetConfig(item_id)
	if item_cfg.item_type == ItemType.Fashion then
		local surface_cfg = Cfg.SurfaceInfoById(item_cfg.param1)
        if surface_cfg and surface_cfg.res_id ~= 0 then
			local appearance = TableCopy(RoleData.Instance:GetApearance())
			if surface_cfg.type == IllusionConfig.FashionType.cloth then
				appearance.color = 0
				appearance.fashion_body_id = surface_cfg.res_id
				appearance.special_appearance_type = 0
				appearance.special_appearance_param = 0
			elseif surface_cfg.type == IllusionConfig.FashionType.weapon then
				appearance.weapon_id = 2001 + RoleData.Instance:ProfType() * 100
				appearance.fashion_weapon_id = surface_cfg.res_id
			end
			self.ui_role_show:SetData(appearance)

			--时装属性专精特效
			local surface_value_cfg = Cfg.SurfaceValue(item_cfg.param1, RoleData.Instance:GetRoleProf())
			local count = 1
			for k, v in pairs(AttrNameForType) do
				if surface_value_cfg[v] ~= nil and surface_value_cfg[v] ~= 0 then
					local str = Format(Language.ItemInfo.ItemTitles.IllusionAttr, Language.Illusion.AttrNames[v][1], DataHelper.Percent(k, surface_value_cfg[v]))
					UH.SetText(self.NameList[count], str)
					UH.SetText(self.ValueList[count], "")
					count = count + 1
				end
			end
			for i = count, 2, 1 do
				UH.SetText(self.NameList[count], "")
				UH.SetText(self.ValueList[count], "")
			end
		end
	end
end

function CowGoodLuckViewPanel:AutoToNext()
	self.show_model_index = self.show_model_index + 1
    self.show_model_index = self.show_model_index > #self.show_models and 1 or self.show_model_index
	self:ShowModel()
end

CowRewardListItem = CowRewardListItem or DeclareMono("CowRewardListItem", UIWidgetBaseItem)
function CowRewardListItem:CowRewardListItem()
end

function CowRewardListItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item_data = Item.New()
    item_data.item_id = data.item_id
    item_data.num = data.num
	self.Cell:SetData(item_data)
end

----------------------------CowShopItem----------------------------
CowShopItem = CowShopItem or DeclareMono("CowShopItem", UIWidgetBaseItem)
function CowShopItem:SetData(data)
    data.item = Item.New(data.reward_item)
    self.Cell:SetData(data.item)
    UH.SetText(self.Name,data.item:QuaName(true))
	local price = data.price
    local original_price = data.original_price
	if data.price_type == 3 then
		price = price / 10
		original_price = original_price / 10
		UH.SetGold(self.Gold,999)
	else
		UH.SetGold(self.Gold,data.price_type)
	end
	UH.SetText(self.Num,price == 0 and Language.OpenAct.LimitShop.Free or price)
    UH.SetText(self.OldNum,Language.RACowGoodLuck.ShowValve .. original_price)
    self.BtnFree:SetActive(price == 0)

    local limit_times = CowGoodLuckData.Instance:GetGiftCanBuyCount(data.seq) - CowGoodLuckData.Instance:GetGiftBuyCount(data.seq)
    UH.SetText(self.Times,Format(Language.Shop.LimitTimesList[data.limit_type],limit_times))

    self.EnterInter.Interactable = limit_times > 0
    self.BtnFreeInter.Interactable = limit_times > 0
	self.ImgOver:SetActive(not (limit_times > 0))

    UIWidgetBaseItem.SetData(self, data)
end

function CowShopItem:OnClickBuy()
    if self.data.price_type == 3 then
		if IS_EDITOR then
			local gm_cmd = tostring(self.data.price).." "..tostring(BUY_ITEM_TYPE.TYPE_COW_STORE).." "..tostring(self.data.type).." " .."0"
			GMCmdCtrl.Instance:SendGMCommand("buyitem",gm_cmd)
		else
			ChannelAgent:RequestPay((self.data.price/10),BUY_ITEM_TYPE.TYPE_COW_STORE,self.data.type)
		end
	elseif self.data.price_type == 0 then
        if MallCtrl.IsNotCurrency(CurrencyType.Gold, self.data.price) then
			ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_STORE,1,self.data.type,self.data.seq)
        end
    else
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_STORE,1,self.data.type,self.data.seq)
    end
--[[    self.data.confirm_func = function()
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_STORE,1,self.data.type,self.data.item_seq)
    end
    ViewMgr:OpenView(BatchBuyView,self.data)--]]
end







