Reward2View = Reward2View or DeclareView("Reward2View", "public_popup/reward2")
VIEW_DECLARE_MASK(Reward2View,ViewMask.BgBlockClose)
VIEW_DECLARE_CACHE(Reward2View)
---HintObj======GameObject
---HintLabel====Text
---ItemList=====UIWidgetList[Reward2ViewItem]

function Reward2View:LoadCallback(param_t)
	param_t = param_t or {}
	local data = PublicPopupData.Instance:Reward2Data()
	if param_t.is_tip then
		PublicPopupCtrl.Instance:ShowCustom(data.items)
	end
	local item_list_mono
	if param_t.show_type == 1 then
		item_list_mono = self.ExpandItemList
		self.ExpandItemList:SetObjActive(true)
		self.ItemList:SetObjActive(false)
	else
		item_list_mono = self.ItemList
		self.ExpandItemList:SetObjActive(false)
		self.ItemList:SetObjActive(true)
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
	if StringIsEmpty(data.hint_text) then
		self.HintObj:SetActive(false)
	else
		self.HintObj:SetActive(true)
		self.HintLabel.text = data.hint_text
	end
	item_list_mono:SetData(data.items)
	self.Anim.enabled = false
	self.Anim.enabled = true
	if data.title_name ~= nil then
		UH.SpriteName(self.TitleImg,data.title_name)
	end
end

function Reward2View:OnClose()
	ViewMgr:CloseView(Reward2View)
end

function Reward2View:CloseCallback()
	local data = PublicPopupData.Instance:Reward2Data()
	if data.close_func ~= nil then
		local f = data.close_func
		data.close_func = nil
		f()
	end
end

Reward2ViewItem = Reward2ViewItem or DeclareMono("Reward2ViewItem",UIWidgetBaseItem)
---Item======Substitute[Item]
---Name======Text
---SpecialIcon======IImageSpriteSet
---SpecialQua======IImageSpriteSet
---ItemSubs=======GameObject

function Reward2ViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
	if data.virtual_type == nil then 
		if self.SpecialObj then 
			self.SpecialObj:SetActive(false)
			self.ItemSubs:SetActive(true)
		end 
		self.Item:SetData(self.data)
		self.Name.text = self.data:Name() --string.format("%s * %s",self.data:Name(),self.data:Num())
	elseif data.virtual_type and data.virtual_type == REWARD_VIRTUAL_TYPE.TYPE_PET then 
		if self.SpecialObj then 
			self.ItemSubs:SetActive(false)
			self.SpecialObj:SetActive(true)
			UH.SetIcon(self.SpecialIcon, data.icon_id)
			UH.SpriteName(self.SpecialQua, data.qua_frame)
		end 
		self.Name.text = data.name
	end
end

Reward2ViewExpandItem = Reward2ViewExpandItem or DeclareMono("Reward2ViewExpandItem",UIWidgetBaseItem)
---Item======Substitute[Item]
---Name======Text
---SpecialIcon======IImageSpriteSet
---SpecialQua======IImageSpriteSet
---ItemSubs=======GameObject

function Reward2ViewExpandItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
	if data.virtual_type == nil then 
		if self.SpecialObj then 
			self.SpecialObj:SetActive(false)
			self.ItemSubs:SetActive(true)
		end 
		self.Item:SetData(self.data)
		self.Name.text = string.format("%s",self.data:Name())
	elseif data.virtual_type and data.virtual_type == REWARD_VIRTUAL_TYPE.TYPE_PET then 
		if self.SpecialObj then 
			self.ItemSubs:SetActive(false)
			self.SpecialObj:SetActive(true)
			UH.SetIcon(self.SpecialIcon, data.icon_id)
			UH.SpriteName(self.SpecialQua, data.qua_frame)
		end 
		self.Name.text = data.name
	end
end