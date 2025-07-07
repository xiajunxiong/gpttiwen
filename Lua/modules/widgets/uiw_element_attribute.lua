UIWElementAttribute = UIWElementAttribute or DeclareMono("UIWElementAttribute")

--{BattleAttriType1=%d,BattleAttriType2=%d}
function UIWElementAttribute:SetData(data, play_effect, is_chaos)
	is_chaos = is_chaos or false
	local list = {}
	for k,v in pairs(data) do
		if v ~= 0 then
			table.insert(list, {type = k,value = v})
		end
	end
	if #list > 1 then
		list = self:Sortlist(list)
		list[1].secondType = list[2].type
	elseif #list < 1 and is_chaos == false then
		self.ElementList[1]:SetObjActive(false)
		return
	end
	if self.ShowText then
		list[1].ShowText = true
	end
	if list[1] == nil then
		list[1] = {}
	end
	list[1].is_chaos = is_chaos
	self.ElementList[1]:SetData(list[1])
	self.ElementList[1]:SetEffect(play_effect)
	self.ElementList[1]:SetObjActive(true)
	self.ElementList[2]:SetObjActive(false)
end

--排序
function UIWElementAttribute:Sortlist(list)
	local sort = {[43] = 4,[40] = 3,[41] = 2,[42] = 1}
	if list[1].type + list[2].type == 85 then
		table.sort(list,function(a,b)return a.type < b.type end)
	else
		for i=1,#list do
			list[i].sort = sort[tonumber(list[i].type)]
		end
		table.sort(list,function(a,b)return a.sort > b.sort end)
	end
	return list
end

function UIWElementAttribute:OnClickHelp()
	ViewMgr:OpenView(PetRestraintView)
end

----------------------------ElementAttributeItem----------------------------
ElementAttributeItem = ElementAttributeItem or DeclareMono("ElementAttributeItem", UIWidgetBaseItem)

function ElementAttributeItem:SetData(data)
	-- UH.SetText(self.val,data.value or "")
	-- LogError("ElementAttributeItem SetData",data)
	if data.secondType ~= nil and data.type ~= nil then
		UH.SpriteName(self.ele,"ShuXing_"..tostring(data.type)..tostring(data.secondType))
		UH.SetText(self.eleName, data.ShowText and Language.ElementName["ShuXing_"..tostring(data.type)..tostring(data.secondType)] or "")
	elseif data.is_chaos == true then
		UH.SpriteName(self.ele, "HunDunShuXing")
	else
		UH.SpriteName(self.ele,ElementSpriteName[data.type or 0])
		UH.SetText(self.eleName, data.ShowText and Language.ElementName["ShuXing_"..tostring(data.type)] or "")
	end
end

function ElementAttributeItem:SetEffect(active)
	if active and self.effect.isActiveAndEnabled then
		self.effect:Play(10010279)
	end
	self.effect:SetObjActive(active)
end