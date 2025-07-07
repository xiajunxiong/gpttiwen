PetPieces = PetPieces or DeclareView("PetPieces", "pet/pet_pieces", Mod.PetPiecesCompose.Main)
VIEW_DECLARE_MASK(PetPieces, ViewMask.Block)

function PetPieces:PetPieces()
end

function PetPieces:LoadCallback()
	self.Board:SetData(self:GetType(), Language.Pet.PiecesCompose, Vector2.New(499, 625))
end
------------------------------
PetPiecesFlush = PetPiecesFlush or DeclareMono("PetPiecesFlush", UIWFlushPanel)
function PetPiecesFlush:PetPiecesFlush()
	self.data_cares = {
		{data = BagData.Instance.item_grid[ItemColumnType.Task], func = self.FlushInfo},
		{data = BagData.Instance.item_grid[ItemColumnType.Item], func = self.FlushInfo},
	}
	self.pet_data = PetData.Instance
    self.language = Language.Pet
    self.cur_add_num = 0

    GuideManager.Instance:RegisterGetGuideUi("PetPiecesComposeBtn", function()
		if self.PetPiecesComposeBtn then
			return self.PetPiecesComposeBtn
		end
	end)
end

function PetPiecesFlush:FlushInfo()
	if self.composeInfo == nil then
		local item_info = ItemInfoCtrl.Instance.data:ItemInfo()
		--TopItem(宠物图标)
		self.composeInfo, self.common_piece_compose = PetData.GetComposeInfo(item_info.item_list[1].item_id)
		if self.composeInfo == nil then
			if not IS_EDITOR then
				BuglyReport("PetPiecesView can not find compose cfg of item_id", item_info.item_list[1].item_id)
			else
				LogError("PetPiecesView can not find compose cfg of item_id", item_info.item_list[1].item_id)
			end
			PublicPopupCtrl.Instance:Center(Language.Pet.PiecesCannotFindConfig)
			ViewMgr:CloseView(PetPieces)
			return
		end
	end
	
	if self.common_piece_compose then
		self:FlushCommonPieceCompose()
		return
	end
	UH.AnchoredPosition(self.TopInfoRect, Vector2.New(0,0))
	UH.AnchoredPosition(self.ComposeInfoRect, Vector2.New(0,-6))
	self.MidInfoObj:SetActive(true)
	local pet_cfg_list = PetData.Instance:PetGroupById(self.composeInfo.reward_group)
	local pet_cfg = PetData.Instance:GetPetCfg(pet_cfg_list[1].pet_id)
	if pet_cfg then
		self.pet = Pet.New(pet_cfg)
		self.TopItem:SetData(self.pet)
		UH.SetText(self.TopItemName, pet_cfg.name)
	end
	--MidPetItem(宠物碎片图标+通用碎片图标)
	local midCell = Item.New({item_id = self.composeInfo.patch_id})
	self.MidPetItem:SetData(midCell)
	if self.cur_add_num == 0 then
		self.cur_add_num = Item.GetNum(self.composeInfo.patch_id)
	end
	if self.composeInfo then
		--这里获取的数量重新从背包再获取一次，可能有绑定和非绑定的要一个总数而不是点击的那个格子里的数量
		if self.composeInfo.use_universal_patch <= 0 then
			local str_color = self.cur_add_num >= self.composeInfo.personal_patch_num and COLORSTR.Green7 or COLORSTR.Red10
			UH.SetText(self.StuffNum, string.format(Language.Common.Cstr, str_color, self.cur_add_num, self.composeInfo.patch_num))
		end
	end
	--CommonItem(通用碎片图标)
	if self.pet then
		if self.composeInfo.use_universal_patch <= 0 then
			self.CommonInfoObj:SetActive(false)
			UH.SetText(self.SpecialNoticeText, Language.Pet.PiecesCannotUseCommonTip[self.composeInfo.use_universal_patch] == nil and Language.Pet.PiecesCannotUseCommonNotice or Language.Pet.PiecesCannotUseCommonTip[self.composeInfo.use_universal_patch])
			-- self.SpecialNoticeObj:SetActive(true)
			self.ReduceBtnObj:SetActive(false)
			self.AddBtnObj:SetActive(false)
			return
		else
			self.CommonInfoObj:SetActive(true)
			UH.SetText(self.SpecialNoticeText, "")
			-- self.SpecialNoticeObj:SetActive(false)
			self.ReduceBtnObj:SetActive(true)
			self.AddBtnObj:SetActive(true)
			UH.SetText(self.StuffNumTip, string.format(Language.Pet.PiecesComposeCondition, self.composeInfo.personal_patch_num, pet_cfg.name))
		end
	end
	local commonItemId = PetData.Instance.fragment_item_id[self.composeInfo.use_universal_patch]
	local commonCell = Item.Init(commonItemId)
	self.CommonItem:SetData(commonCell)
	self.common_num = Item.GetNum(commonItemId)
	UH.SetText(self.CommonLeftNum, Language.Common.MultpSign..self.common_num)
	UH.SetText(self.StuffNoticeText, string.format(Language.Pet.PetPiecesNotice, commonCell:Name()))
	UH.SetText(self.StuffCommonLeft, string.format(Language.Pet.PetPiecesCommonLeft, commonCell:Name()))

	if self.composeInfo then
		--这里获取的数量重新从背包再获取一次，可能有绑定和非绑定的要一个总数而不是点击的那个格子里的数量
		local str_color = self.cur_add_num + self.common_num >= self.composeInfo.patch_num and self.cur_add_num >= self.composeInfo.personal_patch_num and COLORSTR.Green7 or COLORSTR.Red10
		UH.SetText(self.StuffNum, string.format(Language.Common.Cstr, str_color, self.cur_add_num, self.composeInfo.patch_num))
	end
end

function PetPiecesFlush:FlushCommonPieceCompose()
	UH.AnchoredPosition(self.TopInfoRect, Vector2.New(0,-20))
	UH.AnchoredPosition(self.ComposeInfoRect, Vector2.New(0,20))
	self.MidInfoObj:SetActive(false)
	
	local patch_item = Item.Init(self.composeInfo.target,1,1)
	self.TopItem:SetData(patch_item)
	UH.SetText(self.TopItemName, patch_item:Name())

	local compose_item = Item.Init(self.composeInfo.stuff_id_1,1,1)
	self.CommonItem:SetData(compose_item)
	UH.SetText(self.StuffNumTip, string.format(Language.Pet.CommonPiecesComposeCondition, self.composeInfo.stuff_num_1, compose_item:Name()))
	UH.SetText(self.StuffNoticeText, string.format(Language.Pet.PetCommonPieceCompose, patch_item:Name()))
	self.common_num = Item.GetNum(self.composeInfo.stuff_id_1)
	UH.SetText(self.CommonLeftNum, string.format(Language.Common.Xstr, self.common_num >= self.composeInfo.stuff_num_1 and COLORSTR.Green7 or COLORSTR.Red10, Language.Common.MultpSign..self.common_num))
	UH.SetText(self.StuffCommonLeft, string.format(Language.Pet.PetPiecesCommonLeft, compose_item:Name()))
end

function PetPiecesFlush:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.cur_add_num = 0
	self.pet = nil
	self.composeInfo = nil
	self.common_piece_compose = nil
	GuideManager.Instance:UnRegisterGetGuideUi("PetPiecesComposeBtn")
end

function PetPiecesFlush:OnClickCancel()
	ViewMgr:CloseView(PetPieces)
end

function PetPiecesFlush:OnClickCompose()
	if self.composeInfo then
		if self.common_piece_compose then
			if self.common_num < self.composeInfo.stuff_num_1 then
				MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = self.composeInfo.stuff_id_1, jump_need_num = self.composeInfo.stuff_num_1 - self.common_num})})
			else
				--发送合成协议
				-- PublicPopupCtrl.Instance:Center("发送合成逻辑暂未完成")
				WorkshopCtrl.Instance:SendCompoundReq(self.composeInfo.seq,1,0)
				ViewMgr:CloseView(PetPieces)
			end
			return
		end
		if self.composeInfo.patch_id ~= 0 then
			if self.cur_add_num < self.composeInfo.personal_patch_num then
				if self.composeInfo.use_universal_patch == 2 then
					PublicPopupCtrl.Instance:Center(Language.Pet.PiecesNotEnough)
				elseif self.composeInfo.use_universal_patch == 1 then
					MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = self.composeInfo.patch_id, jump_need_num = self.composeInfo.personal_patch_num - self.cur_add_num})})
				end
				return
			end
			local has_num = Item.GetNum(self.composeInfo.patch_id)
			has_num = self.cur_add_num <= has_num and self.cur_add_num or has_num
			if has_num < self.composeInfo.patch_num then
				if self.composeInfo.use_universal_patch > 0 then
					local has_common = Item.GetNum(PetData.Instance.fragment_item_id[self.composeInfo.use_universal_patch])
					if has_common < self.composeInfo.patch_num - has_num then
						local item = Item.Create({
									item_id = PetData.Instance.fragment_item_id[self.composeInfo.use_universal_patch], 
									need_num = self.composeInfo.patch_num - has_num - has_common,
								})
						MainOtherCtrl.Instance:GetWayView({item = item})
						PublicPopupCtrl.Instance:Center(string.format(Language.Common.NotEnough, item:Name()))
						return
					end
				else
					PublicPopupCtrl.Instance:Center(Language.Pet.PiecesNotEnough)
					return
				end
			end
		end
		local common_cost = self.pet:Quality() >= 5 and 0 or self.composeInfo.patch_num - self.cur_add_num
		common_cost = common_cost <= 0 and 0 or common_cost
		PetCtrl.SendPetOp(PET_OP_TYPE.Compose, self.composeInfo.seq, common_cost)
		ViewMgr:CloseView(PetPieces)
	end
end

function PetPiecesFlush:OnClickAdd()
	if self.cur_add_num == self.composeInfo.patch_num then
		return
	end
	local item_info = ItemInfoCtrl.Instance.data:ItemInfo()
	local has_num = Item.GetNum(item_info.item_list[1].item_id)
	if self.cur_add_num == has_num then
		PublicPopupCtrl.Instance:Center(Language.Pet.PiecesNotEnough)
		return
	end
	self.cur_add_num = self.cur_add_num + 1
	local str_color = self.cur_add_num + self.common_num >= self.composeInfo.patch_num and self.cur_add_num >= self.composeInfo.personal_patch_num and COLORSTR.Green7 or COLORSTR.Red10
	UH.SetText(self.StuffNum, string.format(Language.Common.Cstr, str_color,self.cur_add_num, self.composeInfo.patch_num))
end

function PetPiecesFlush:OnClickReduce()
	if self.cur_add_num == 1 then
		return
	end
	self.cur_add_num = self.cur_add_num - 1
	local str_color = self.cur_add_num + self.common_num >= self.composeInfo.patch_num and self.cur_add_num >= self.composeInfo.personal_patch_num and COLORSTR.Green7 or COLORSTR.Red10
	UH.SetText(self.StuffNum, string.format(Language.Common.Cstr, str_color, self.cur_add_num, self.composeInfo.patch_num))
end