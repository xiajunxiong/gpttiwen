--#40820 任务 G6-A线 LineA - 自动抓宠方案设计
FightAutoSealSetView = FightAutoSealSetView or DeclareView("FightAutoSealSetView","fight/fight_auto_seal_set_view")
VIEW_DECLARE_MASK(FightAutoSealSetView,ViewMask.BgBlock)

function FightAutoSealSetView:FightAutoSealSetView()
	self.Board:SetData(self:GetType(),Language.Fight.Seal.Title, Vector2.New(584, 623))
	self.data = FightCtrl.Instance:Data()
	GuideManager.Instance:RegisterGetGuideUi(
        "EnterAutoSealBtn",
        function()
           return self.GoToBtnObj,function ()
                self:OnGotoClick()
           end
        end
	)
end
-- param_t.target_pets = {{pet_id,num},{}}
function FightAutoSealSetView:LoadCallback(param_t)
	if self.handle == nil then
        self.handle = BagData.Instance:Care(BindTool.Bind(self.CheckWeekTask, self))
    end
	--self.MaskObj:SetActive(BehaviorData.Instance.auto_behavior > 0)
	param_t = param_t or {}
	if param_t ~= nil and param_t.scene_id ~= nil then
		local scene_data = self.data:GetFightSceneDataById(param_t.scene_id)
		self.data.cur_sel_field:Set(scene_data)
		self.scene_id = param_t.scene_id
	end
	self.cur_sel_field = self.data.cur_sel_field
	local pets = self.data:GetCurSceneCanSealPets()
	local pet_data = {}
	for i = 1,#pets do
		local pet_t = {}
		pet_t.pet_id = tonumber(pets[i])
		table.insert(pet_data,pet_t)
	end
	if next(pet_data) ~= nil and param_t.target_pets and next(param_t.target_pets) ~= nil then
		for _,v in pairs(pet_data) do
			for i,v2 in pairs(param_t.target_pets) do
				if v.pet_id == v2.pet_id then
					v.is_target = true
					v.target_num = v2.num or 1
					break
				end
			end
		end
	end
	-- 是通过指引打开的界面默认选中第一个
	if self.data.is_guide_seal then
		if pet_data[1] then
            pet_data[1].is_target = true
			pet_data[1].target_num = 1
		end
	end
	self.PetList:SetData(pet_data)
	local card_List_data = {
		{item_id = 26078,qua = 1 },
		{item_id = 26079,qua = 2 },
		{item_id = 26080,qua = 3 },
    }
    self.CardList:SetData(card_List_data)
    local pet_list_num = table.nums(PetData.Instance:GetPetList())
    local pet_bag_num = PetData.Instance:GetBagCapacity()
    if pet_list_num >= pet_bag_num then
    	UH.SetText(self.PetBagNum,string.format("<color=#FF3E3E>%s/%s</color>",pet_list_num,pet_bag_num))
    else
    	UH.SetText(self.PetBagNum,string.format("%s/%s",pet_list_num,pet_bag_num))
    end
	
	self:CheckWeekTask()
end

function FightAutoSealSetView:CloseCallback()
	self.data.is_guide_seal = false
	local seal_data = self.data:GetAutoSealData()
	if seal_data.param_t == nil or seal_data.param_t.scene_seq == nil then
		FightCtrl.Instance:ClearAutoSealData()
		self.data:SetAutoSealFlag(false)
	end
	if self.scene_id then
		self.data.cur_sel_field.click_handle()
	end
	if BehaviorData.Instance.auto_behavior > 0 and not self.is_go_func then
		BehaviorData.Instance.auto_behavior = 0
	end
	TimeHelper:CancelTimer(self.time_ht)
	BagData.Instance:UnCare(self.handle)
end

-- 商人的烦恼任务判断
function FightAutoSealSetView:CheckWeekTask()
	if BehaviorData.Instance.auto_behavior < 1 then
		return
	end
	-- 判断普卡是否满足7张
	local item_num = Item.GetNum(26078)
	if item_num < 7 then
		if BagData.Instance:TryFindEmptyInBag(26078) then
			ViewMgr:OpenView(BuyPetSealCardView, {item_id = 26078, is_auto_buy = true, num = 7 - item_num, cancel_func = function ()
				ViewMgr:CloseView(FightAutoSealSetView)
				BehaviorData.Instance.auto_behavior = 0
			end})
		else
			PublicPopupCtrl.Instance:Error(Language.Fight.Seal.BuyTip)
			BehaviorData.Instance.auto_behavior = 0
		end
	else
		if self.CardList:GetItem(1) then
			self.CardList:GetItem(1):FlushNum()
			self.CardList:GetItem(1):OnAddClick(7)
		end
		TimeHelper:CancelTimer(self.time_ht)
		self.time_ht = Invoke(function ()
			self:OnGotoClick()
		end,BehaviorData.Instance.auto_behavior)
	end
end

function FightAutoSealSetView:OnGotoClick()
	-- 是通过指引前往封印的直接去
	if self.data.is_guide_seal then
		RoleSettingData.Instance.shield_fight_monster = true
		ViewMgr:CloseView(FightAutoSealSetView)
		ViewMgr:CloseView(FightView)
		SceneLogic.Instance:AutoToPos(self.data.cur_sel_field.scene_id,nil,nil,nil,true)
		SceneLogic.Instance.move_to_pos_cache.guaji = true
		SceneLogic.Instance:GoOnPatrol()
		FightCtrl.Instance:GuideEnterSeal()
		return
	end
    if RoleData.Instance:GetRoleLevel() < self.data.cur_sel_field.level_min then
		PublicPopupCtrl.Instance:Center(ErrorInfo[506])
		return
	end

	if self.data:GetSumSealPetNum() <= 0 then
		PublicPopupCtrl.Instance:Center(Language.Fight.Seal.LimitTip1)
		return
	end

	if not self.CardList:GetItem(1) or not self.CardList:GetItem(2) or not self.CardList:GetItem(3) then
		return
	end

	if self.CardList:GetItem(1).data.use_num <= 0 and self.CardList:GetItem(2).data.use_num <= 0 and self.CardList:GetItem(3).data.use_num <=0 then
		PublicPopupCtrl.Instance:Center(Language.Fight.Seal.LimitTip2)
		return
	end

	if nil ~= self.data.cur_sel_field.click_handle then
		local param_t = {}
		param_t.max_seal_card_common = self.CardList:GetItem(1).data.use_num
		param_t.max_seal_card_silver = self.CardList:GetItem(2).data.use_num
		param_t.max_seal_card_gold = self.CardList:GetItem(3).data.use_num
		param_t.auto_catch_info_num = #self.PetList.item_list
		param_t.setting_list = {}
		param_t.scene_seq = self.cur_sel_field.scene_seq
		param_t.scene_name = self.data.cur_sel_field._scene.name
		param_t.scene_id = self.data.cur_sel_field.scene_id
		local pet_min_qua = math.huge
		local is_100_succ = false    --是否百分之百可以抓到至少一只宝宝
		for i=1,#self.PetList.item_list do
			local seal_data = self.PetList:GetItem(i).data
			local pet_qua = PetData.Instance:GetPetCfg(seal_data.pet_id).quality
			if seal_data.seal_num > 0 and pet_qua < pet_min_qua then
				pet_min_qua = pet_qua
			end
			param_t.setting_list[i] = {}
			param_t.setting_list[i].monster_id = self.data:GetMonstersIdByPetId(seal_data.pet_id) or 0
			param_t.setting_list[i].max_catch_num = seal_data.seal_num
			if not is_100_succ then
				local need_card_num
				if pet_qua > 1 then
					need_card_num = 10
				else
					need_card_num = 7
				end
				--need_card_num = need_card_num * seal_data.seal_num
				if self.CardList:GetItem(pet_qua).data.use_num >= need_card_num then
					is_100_succ = true
				end
			end
		end
		local crad_max_qua = 0
		for i=1,3 do
			if self.CardList:GetItem(i).data.use_num > 0 then
				if self.CardList:GetItem(i).data.qua > crad_max_qua then
					crad_max_qua = self.CardList:GetItem(i).data.qua
			    end
			end
		end
		--最高的封印卡品质小于最小的要抓的宠物品质
		if crad_max_qua < pet_min_qua then
			PublicPopupCtrl.Instance:Center(string.format(Language.Fight.Seal.LimitTip3,Language.Fight.Seal.CardQua[crad_max_qua]))
			return
		end
		local go_func = function ()
			self.is_go_func = true
		    self.data:SetSealing(true)
			self.data.cur_sel_field.click_handle()
			FightCtrl.Instance:SendSetAutoCatchPet(param_t)
			self.data:SetAutoSealRoleId(RoleData.Instance:GetRoleId())
			self.data.auto_seal_pet_data.param_t = param_t
			ViewMgr:CloseView(FightAutoSealSetView)
			ViewMgr:CloseView(PetView) --可能是从宠物的获取途径过来的
		end
		if crad_max_qua == pet_min_qua and not is_100_succ then
			local need_card_num = 10
			local need_card
			if crad_max_qua == 1 then
				need_card_num = 7
				need_card = self.CardList:GetItem(1)
			else
				need_card_num = 10
				need_card = self.CardList:GetItem(2)
			end
			local param_t = {
		        content = string.format(Language.Fight.Seal.LimitTip5,Language.Fight.Seal.CardQua[pet_min_qua],need_card_num),
		        cancel = {
		            name = Language.Fight.Seal.No,
		        	func = function ()
		        		go_func()
		        		ViewMgr:CloseView(DialogTipsView)
		        	end
		    	},
		    	confirm = {
		    	    name = Language.Fight.Seal.Yes,
		            func =  function()
						need_card:SetAutoNum(true)
		                ViewMgr:CloseView(DialogTipsView)
		            end 
		        },
		    }
		    PublicPopupCtrl.Instance:DialogTips(param_t)
		else
			go_func()
		end
	end
end

function FightAutoSealSetView:OnTipClick()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Language.Fight.Seal.Desc})
end


FightAutoSealSetItem = FightAutoSealSetItem or DeclareMono("FightAutoSealSetItem",UIWidgetBaseItem)
function FightAutoSealSetItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self.PetCell:SetData(Pet.New({item_id = data.pet_id}))
	if data.is_target and data.is_target == true and data.target_num then
		data.seal_num = data.target_num
		self.TargetFlagObj:SetActive(true)
		FightData.Instance:SetAutoSealPet(self.data.pet_id,self.data.seal_num)
	else
		self.TargetFlagObj:SetActive(false)
		data.seal_num = 0
	end
	UH.SetText(self.InputText,data.seal_num.."")
	self.InputText.onValueChanged:AddListener(BindTool.Bind(self.OnInputClick, self))
end

function FightAutoSealSetItem:OnAddClick()
	if self.data == nil then
		return
	end
	self.data.seal_num = self.data.seal_num + 1
	local max_bag_num = FightData.Instance:GetAutoSealPetMaxNum() + self.data.seal_num - 1
	if self.data.seal_num > max_bag_num then
		self.data.seal_num = max_bag_num
		TimeHelper:CancelTimer(self.add_ht)
		PublicPopupCtrl.Instance:Center(Language.Fight.Seal.PetBagTip)
	end
	UH.SetText(self.InputText,self.data.seal_num.."")
	FightData.Instance:SetAutoSealPet(self.data.pet_id,self.data.seal_num)
end

function FightAutoSealSetItem:OnAddPressDown()
	TimeHelper:CancelTimer(self.add_ht)
	self.add_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnAddClick,self),0.2)
end

function FightAutoSealSetItem:OnAddPressUp()
	TimeHelper:CancelTimer(self.add_ht)
end

function FightAutoSealSetItem:OnMinusClick()
	self.data.seal_num = self.data.seal_num - 1
	if self.data.seal_num < 0 then
		self.data.seal_num = 0
		TimeHelper:CancelTimer(self.minus_ht)
		PublicPopupCtrl.Instance:Center(Language.Fight.Seal.MinusTip)
	end
	UH.SetText(self.InputText,self.data.seal_num.."")
	FightData.Instance:SetAutoSealPet(self.data.pet_id,self.data.seal_num)
end

function FightAutoSealSetItem:OnMinusPressDown()
	TimeHelper:CancelTimer(self.minus_ht)
	self.minus_ht =TimeHelper:AddRunTimer(BindTool.Bind(self.OnMinusClick,self),0.2)
end

function FightAutoSealSetItem:OnMinusPressUp()
	TimeHelper:CancelTimer(self.minus_ht)
end

function FightAutoSealSetItem:OnInputClick()
	local num = tonumber(self.InputText.text) or 0
	local max_bag_num = FightData.Instance:GetAutoSealPetMaxNum() + self.data.seal_num
	if num > max_bag_num then
		num = max_bag_num
		PublicPopupCtrl.Instance:Center(string.format(Language.Fight.Seal.PetBagRemain,max_bag_num))
	end
	if num < 0 then
		num = 0
	end
	self.data.seal_num = num
	FightData.Instance:SetAutoSealPet(self.data.pet_id,self.data.seal_num)
	UH.SetText(self.InputText,self.data.seal_num.."")
end
function FightAutoSealSetItem:OnDestroy()
	TimeHelper:CancelTimer(self.add_ht)
	TimeHelper:CancelTimer(self.minus_ht)
	UIWidgetBaseItem.OnDestroy(self)
end


FightAutoSealSetCardItem = FightAutoSealSetCardItem or DeclareMono("FightAutoSealSetCardItem",UIWidgetBaseItem)
function FightAutoSealSetCardItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self.Cell:SetData(Item.Create({item_id = data.item_id}))
	self.InputText.onValueChanged:AddListener(BindTool.Bind(self.OnInputClick, self))
	if self.handle == nil then
        self.handle = BagData.Instance:Care(BindTool.Bind(self.FlushNum, self))
    end
	if self.data_care == nil then
		self.pet_data = FightData.Instance:GetAutoSealData()
		self.data_care = self.pet_data:Care(BindTool.Bind(self.FlushUseNum, self))
	end
    self.min_pet_qua = FightData.Instance:GetCurSceneCanSealPetsMinQua()
    self:FlushNum()
	self:FlushUseNum()
end

function FightAutoSealSetCardItem:FlushUseNum()
	self.data.use_num = self.data.use_num or 0
	if FightData.Instance.is_guide_seal then
        self.data.use_num = Item.GetNum(self.data.item_id)
	else
		-- 根据宠物封印最低卡片使用数量提示配置 -- 只处理一次 ，需求单号：#46416
		if not self.is_auto_set_use and self.pet_data.pet_nums then
			self.is_auto_set_use = true
			self:SetAutoNum()
		end
	end
	UH.SetText(self.InputText,self.data.use_num.."")
end

function FightAutoSealSetCardItem:FlushNum()
	if not self.data then
		return
	end
	self.data.item_num = Item.GetNum(self.data.item_id)
	UH.SetText(self.Num,self.data.item_num)
	if self.data.qua >= self.min_pet_qua then
		self.can_set = true
	else
		self.can_set = false
	end
	self.Interactor.Interactable = (self.data.item_num > 0 and self.can_set)
end

function FightAutoSealSetCardItem:SetAutoNum(is_flush_num)
	if self.pet_data.pet_nums == nil then
		return
	end
	for k,v in pairs(self.pet_data.pet_nums) do
		if PetData.Instance:GetPetCfg(k).quality == self.data.qua and v > 0 then
			if self.data.qua > 1 then
				self.data.use_num = 10
			else
				self.data.use_num = 7
			end
		end
	end
	local bag_card_num = Item.GetNum(self.data.item_id)
	self.data.use_num = self.data.use_num > bag_card_num and bag_card_num or self.data.use_num
	if is_flush_num then
		UH.SetText(self.InputText,self.data.use_num.."")
	end
end

function FightAutoSealSetCardItem:OnAddClick(num)
	if not self.data then
		return
	end
	if self.can_set == false then
		TimeHelper:CancelTimer(self.add_ht)
		PublicPopupCtrl.Instance:Center(string.format(Language.Fight.Seal.LimitTip4,Language.Fight.Seal.CardQua[self.min_pet_qua]))
		return
	end
	num = num or self.data.use_num + 1
	self.data.use_num = num
	if self.data.use_num > self.data.item_num then
		self.data.use_num = self.data.item_num
		TimeHelper:CancelTimer(self.add_ht)
		self:JumpBuyCard()
	end
	UH.SetText(self.InputText,self.data.use_num.."")
end

function FightAutoSealSetCardItem:OnAddPressDown()
	TimeHelper:CancelTimer(self.add_ht)
	self.add_ht =TimeHelper:AddRunTimer(BindTool.Bind(self.OnAddClick,self),0.2)
end

function FightAutoSealSetCardItem:OnAddPressUp()
	TimeHelper:CancelTimer(self.add_ht)
end

function FightAutoSealSetCardItem:OnMinusClick()
	if not self.data then
		return
	end
	self.data.use_num = self.data.use_num - 1
	if self.data.use_num < 0 then
		self.data.use_num = 0
		TimeHelper:CancelTimer(self.minus_ht)
		PublicPopupCtrl.Instance:Center(Language.Fight.Seal.MinusTip)
	end
	UH.SetText(self.InputText,self.data.use_num.."")
end

function FightAutoSealSetCardItem:OnMinusPressDown()
	TimeHelper:CancelTimer(self.minus_ht)
	self.minus_ht =TimeHelper:AddRunTimer(BindTool.Bind(self.OnMinusClick,self),0.2)
end

function FightAutoSealSetCardItem:OnMinusPressUp()
	TimeHelper:CancelTimer(self.minus_ht)
end

function FightAutoSealSetCardItem:OnInputClick()
	local num = tonumber(self.InputText.text) or 0
	if num > self.data.item_num then
		num = self.data.item_num
		self:JumpBuyCard()
	end
	if num < 0 then
		num = 0
	end
	self.data.use_num = num
	UH.SetText(self.InputText,self.data.use_num.."")
end

function FightAutoSealSetCardItem:OnDestroy()
	TimeHelper:CancelTimer(self.add_ht)
	TimeHelper:CancelTimer(self.minus_ht)
	BagData.Instance:UnCare(self.handle)
	self.pet_data:Uncare(self.data_care)
	UIWidgetBaseItem.OnDestroy(self)
end

function FightAutoSealSetCardItem:JumpBuyCard()
	if self.data == nil then
		return
	end
	local item_num = self.data.item_num
	local item_id = self.data.item_id
	PublicPopupCtrl.Instance:DialogTips(
        {
            content = string.format(Language.Fight.Seal.CurMaxItemNum,item_num),
            confirm = {
                func = function()
                    MallCtrl.Instance:Jump(item_id)
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
    )
end