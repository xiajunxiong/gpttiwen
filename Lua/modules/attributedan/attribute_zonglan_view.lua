AttributeZongLanView = AttributeZongLanView or DeclareView("AttributeZongLanView", "attributedan/attribute_zonglan")
function AttributeZongLanView:AttributeZongLanView()
    self.lang = Language.AttributeDan
end

function AttributeZongLanView:CloseCallback()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function AttributeZongLanView:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.TiTle1, Vector2.New(638, 424))
end

AttributeZongLanPanel = AttributeZongLanPanel or DeclareMono("AttributeZongLanPanel", UIWFlushPanel)
function AttributeZongLanPanel:AttributeZongLanPanel()
    self.data = AttributeDanData.Instance
	self.lang = Language.AttributeDan
end

function AttributeZongLanPanel:OnEnable()
    self:ALLAddAttr()
end

function AttributeZongLanPanel:ALLAddAttr()
	local Hptext 
	local Mptext 
	local Atktext 
	local Deftext 
	local Agilitytext 
	local Mentaltext 
	local Recoverytext
	
	local t_text = self.lang.Text1
	if self.data.now_attributedan_type == 2 then 
		local pet = PetData.Instance:GetCurSelect()
		if pet then
			self.RectTransform.sizeDelta = Vector2.New(248, 35)
			local info = pet.info
			t_text =  string.format(self.lang.Text2,info.name)
		end

	end	
	
	if self.data.now_attributedan_type == 3 then 
		local data = PartnerCtrl.Instance:Data():GetPartnerInfoById(self.data.cur_sel_partner)
		if data then
			local info = data.info
			t_text =  string.format(self.lang.Text2,info.name)
		end
	end
	self.TitleText.text = t_text
	
	for i=0,6 do
		local use_count = 0
		local item_id_index = 0
		local value = 0
		if self.data.now_attributedan_type == 1 then 
			use_count = self.data:GetRoleAttibuteDanUseInfo(i)
			if i > 4 then
				item_id_index = 27800 + i - 5
			else
				item_id_index = 27802 + i
			end
			value = self.data:GetAttibuteDanByItemId(item_id_index).attr_value
		end 
		
		if self.data.now_attributedan_type == 2 then 
			use_count = self.data:GetPetAttibuteDanUseInfo(i)
			if i > 4 then
				item_id_index = 27807 + i - 5
			else
				item_id_index = 27809 + i
			end
			value = self.data:GetAttibuteDanByItemId(item_id_index).attr_value
		end 

		if self.data.now_attributedan_type == 3 then 
			use_count = self.data:GetPartnerAttibuteDanUseInfo(self.data.cur_sel_partner,i)
			if i > 4 then
				item_id_index = 27814 + i - 5
			else
				item_id_index = 27816 + i
			end
			value = self.data:GetAttibuteDanByItemId(item_id_index,self.data.cur_sel_partner).attr_value
		end 		
		
		if i == 5 then
			Hptext = use_count * value
		end			
		if i == 6 then
			Mptext = use_count * value
		end		
		if i == 0 then
			Atktext = use_count * value
		end		
		if i == 1 then
			Deftext = use_count * value
		end	
		if i == 2 then
			Agilitytext = use_count * value
		end	
		if i == 3 then
			Recoverytext = use_count * value
		end		
		if i == 4 then
			Mentaltext = use_count * value
		end		
	end
	self.Hp.text = Hptext
	self.Mp.text = Mptext
	self.Atk.text = Atktext
	self.Def.text = Deftext
	self.Agility.text = Agilitytext
	self.Mental.text = Mentaltext
	self.Recovery.text = Recoverytext
end

function AttributeZongLanPanel:OnClickClose()
	ViewMgr:CloseView(AttributeZongLanView)
end








