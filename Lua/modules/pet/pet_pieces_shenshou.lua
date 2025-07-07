PetPiecesShenshou = PetPiecesShenshou or DeclareView("PetPiecesShenshou", "pet/pet_pieces_shenshou", Mod.PetPiecesShenshouCompose.Main)
VIEW_DECLARE_MASK(PetPiecesShenshou, ViewMask.BlockClose)
function PetPiecesShenshou:PetPiecesShenshou()
    self.pet_data = PetData.Instance
	self.language = Language.Pet    
	
end
function PetPiecesShenshou:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.PiecesShenshou, Vector2.New(674, 625))
	local pet_list = PetData.Instance:GetConverList()
	self.PetList:SetData(pet_list)
	PetData.Instance:GetConverList()
	-- self.ScrollPage.OnPageChanged:AddListener(function(i)
	-- 	local item = self.PetList:GetItem(self.ScrollPage.CurrentPage + 1)
	-- 	if self.cur_item == nil then
	-- 		self.cur_item = item
	-- 		item:SetMiddle()
	-- 	elseif self.cur_item ~= item then
	-- 		self.cur_item:SetNormal()
	-- 		item:SetMiddle()
	-- 		self.cur_item = item
	-- 	end
	-- end)
	local commonInfo =  PetData.PetComposeInfoSeq(45)
	self.composeInfo = commonInfo
end


function PetPiecesShenshou:CloseCallback()
	self.pet = nil
	self.composeInfo = nil
	
end

function PetPiecesShenshou:OnClickCancel()
	ViewMgr:CloseView(PetPiecesShenshou)
end

function PetPiecesShenshou:OnClickCompose()
	if self.composeInfo then
		local has_num = Item.GetNum(self.composeInfo.patch_id)
		if has_num < self.composeInfo.patch_num then
			PublicPopupCtrl.Instance:Center(Language.Pet.PiecesShenshouNotEnough)
		else	
			PetCtrl.SendPetOp(PET_OP_TYPE.Convert, 45)-- PetCtrl.SendPetOp(PET_OP_TYPE.Compose, self.composeInfo.seq)
		end
		-- ViewMgr:CloseView(PetPiecesShenshou)
	end
end

function PetPiecesShenshou:OnClickClose()
	ViewMgr:CloseView(PetPiecesShenshou)
end 

--------------------------------------
PetPiecesShenshouFlush = PetPiecesShenshouFlush or DeclareMono("PetPiecesShenshouFlush", UIWFlushPanel)
function PetPiecesShenshouFlush:PetPiecesShenshouFlush()
	self.pet_data = PetData.Instance

	self.data_cares = {
		-- {data = self.pet_data.new_pet_smart, func = self.FlowToGet, keys = {"new_pet"}, init_call = false},
		{data = BagData.Instance.item_grid, func = self.FlushShow},
    }
end

function PetPiecesShenshouFlush:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.flowRunner)
	self.flowRunner = nil
end

function PetPiecesShenshouFlush:FlowToGet()
	local new_pet = self.pet_data.new_pet
	-- LogError("new_pet name ",new_pet:Name())
	local pet_list = PetData.Instance:GetConverList()
	for i,v in ipairs(pet_list) do
		if v:ID() == new_pet:ID() then
				local rate = 0
				if i ~= (self.ScrollPage.CurrentPage + 1) then
					rate = i - (self.ScrollPage.CurrentPage + 1) > 0 and 1 or -1
				end
				local count = math.abs(i - (self.ScrollPage.CurrentPage + 1))
				-- LogError("FlowToGet i =",i,"rate", rate,"count", count,"CurrentPage", self.ScrollPage.CurrentPage)
				if self.flowRunner ~= nil then
					TimeHelper:CancelTimer(self.flowRunner)
					self.flowRunner = nil
				end
				self.flowRunner = TimeHelper:AddRunTimer(function()
					if self.ScrollPage.CurrentPage + rate == i - 1 then
						local item = self.PetList:GetItem(i)
						if item ~= nil then
							item:SetMiddle()
							if self.pet_data.new_pet ~= nil then
								TimeHelper:AddDelayTimer(function()
									ViewMgr:CloseView(PetPiecesShenshou)
									ViewMgr:OpenView(PetGain,{is_xilian = false})
								end, 1.2)
							end
						end
					end
					-- LogError("FlowToGet CurrentPage",self.ScrollPage.CurrentPage)
					self.ScrollPage:ScrollToPage(self.ScrollPage.CurrentPage + rate)
				end, 0.15, count - 1 <= 0 and 1 or count - 1)
			break
		end
	end
end

function PetPiecesShenshouFlush:FlushShow()
	local commonInfo =  PetData.PetComposeInfoSeq(45)
	local commonCell = Item.New({item_id = commonInfo.patch_id})
	self.CommonItem:SetData(commonCell)

	local cost_num =  commonInfo.patch_num
	local color_ = cost_num < Item.GetNum(commonInfo.patch_id) and COLORSTR.Green3 or COLORSTR.Red7
	local str = string.format( Language.Common.StuffNumColorFirst,color_, Item.GetNum(commonInfo.patch_id),cost_num)
	UH.SetText(self.CommonRighttNum, str )
	UH.SetText(self.StuffNoticeText, string.format(Language.Pet.PetPiecesShenshouTip, tostring(cost_num)))
end 

--------------------------------------
PetPiecesShenshouItem = PetPiecesShenshouItem or DeclareMono("PetPiecesShenshouItem", UIWidgetBaseItem)
-- Icon = UIImageSpreteSet
-- Petname = Text
-- Quality = UIImageSpreteSet
function PetPiecesShenshouItem:PetPiecesShenshouItem()
end

function PetPiecesShenshouItem:SetData(data)
	self.data = data
	CellFlushs.SetIcon(self, self.data.data_:IconId())
	UH.SpriteName(self.Quality, self.data.data_:QuaIcon())
	self:SetNormal()
end

function PetPiecesShenshouItem:SetNormal()
	self.IconRect.sizeDelta = Vector2.New(66, 66)
	self.QuaRect.sizeDelta = Vector2.New(76, 76)
	UH.SetText(self.PetName, string.format(Language.Common.Xstr, COLORSTR.Red4, self.data.data_:Name()))
end

function PetPiecesShenshouItem:SetMiddle()
	self.IconRect.sizeDelta = Vector2.New(86, 86)
	self.QuaRect.sizeDelta = Vector2.New(106, 106)
	UH.SetText(self.PetName, string.format(Language.Common.Xstr, COLORSTR.Yellow8, self.data.data_:Name()))
end

function PetPiecesShenshouItem:OnClick()
	PetData.Instance:SetLocalConvertIndex(self.data.index)
	ViewMgr:OpenView(PetConvert)
end 