

UIWAttrItemContain = UIWAttrItemContain or DeclareMono("UIWAttrItemContain")

function UIWAttrItemContain:SetData(data)
	for i = 1, self.AttrItems:Length() do
		self.AttrItems[i]:SetData(data[i])
	end
end

UIWAttrItem = UIWAttrItem or DeclareMono("UIWAttrItem")

function UIWAttrItem:UIWAttrItem()
	self.language = Language.AttrItems
	self.attr_names = self.language.AttrNames
end

function UIWAttrItem:SetData(data)
	self.data = data
end

function UIWAttrItem:HideZero(value)
	self.ItemObj:SetActive(not(value and self.IsHideZero))
end

UIWAttrItem1 = UIWAttrItem1 or DeclareMono("UIWAttrItem1", UIWAttrItem)

function UIWAttrItem1:UIWAttrItem1()
	if self.ProgressWidth then self.Progress:SetProgWidth(self.ProgressWidth) end
end

function UIWAttrItem1:SetData(data)
	UIWAttrItem.SetData(self, data)
	self.Name.text = data.name
	local value_max = data.value_max
	if "string" == type(value_max) then
		value_max = string.gsub(value_max, "<color=#%w+>", "")
		value_max = string.gsub(value_max, "</color>", "")
		value_max = tonumber(value_max)
	end
	self.Progress:SetProgress(data.value_cur / value_max)
	self.Progress:SetText(string.format(Language.Common.Progress2TxtColorBFormat, data.value_cur, data.value_max))
end

-- self.AttrType: 0=HP, 1=MP
function UIWAttrItem1:DataTransBattleAttrNormal(data)
	local value_max = data and data[self.AttrType == 0 and BattleAttriType.MaxHp or BattleAttriType.MaxMp] or 0
	return {
		name = self.AttrType == 0 and self.attr_names.hp or self.attr_names.mp,
		value_cur = data and data[self.AttrType == 0 and BattleAttriType.CurHp or BattleAttriType.CurMp] or 0,
		value_max = 0 == value_max and 1 or value_max,
	}
end

UIWAttrItem2 = UIWAttrItem2 or DeclareMono("UIWAttrItem2", UIWAttrItem)

function UIWAttrItem2:SetData(data)
	UIWAttrItem.SetData(self, data)
	self.Name.text = data.name
	self.Value.text = data.value

	self:HideZero(data.value == 0)
end

-- self.AttrType与BattleAttr对应
function UIWAttrItem:DataTransBattleAttrNormal(data)
	return {
		name = self.attr_names[self.AttrType],
		value = data and data[self.AttrType] or 0,
	}
end

UIWAttrItem3 = UIWAttrItem3 or DeclareMono("UIWAttrItem3", UIWAttrItem)

function UIWAttrItem3:SetData(data)
	UIWAttrItem.SetData(self, data)
	self.Name.text = data.name
	self.Value.text = data.value or 0
	if data.add_value and data.add_value ~= 0 then
		self:FlushAddPoint(data.add_value)
	end
	self.AddValue:SetObjActive(data.add_value and data.add_value ~= 0)
	self:HideZero(data.value == 0)
end

function UIWAttrItem3:DataTransBattleAttrNormal(data,add_data)
	local data_list = {}
	data_list.name = self.attr_names[self.AttrType]
	if data then
		data_list.value = data[self.AttrType]
	end
	if add_data then
		data_list.add_value = add_data[self.AttrType]
	end
	return data_list
end

function UIWAttrItem3:FlushAddPoint(value)
	local COLOR = value > 0 and COLORSTR.Green10 or COLORSTR.Red10
	local add_value = value > 0 and "+" .. value or value
	UH.SetText(self.AddValue,Format(Language.Common.Xstr,COLOR,add_value))
	if self.Anim then
		if self.add_value ~= value and COLOR == COLORSTR.Green10 then
			self.add_value = value
			self.Anim:Play()
		end
	end
end


-- 外部接口列表
-- 新数据类型的请自行添加trans接口及队应的item接口,并添加需要传入的data注释(如下)
-- 新外观样式的请自行添加新的item,并添加到预设生成工具

-- 注意:
-- 1.外观样式的不同才需要添加新的item,如血条和蓝条的不同或不想显示线条直接修改预设工具生成的item实例即可
-- 2.外观样式相同但是当前转换接口不符合可添加新的DataTrans(命名请与功能对应),contain(外部)和item(内部)各一份

-- data = {
-- 	[BattleAttriType[attr_type]] = value,
-- 	...
-- }
-- example:partner_view_attr_up Attr1
function UIWAttrItemContain:DataTransBattleAttrNormal(data,add_data)
	local data_t = {}
	for i = 1, self.AttrItems:Length() do
		data_t[i] = self.AttrItems[i]:DataTransBattleAttrNormal(data,add_data)
	end
	self:SetData(data_t)
end



-- 最简洁的
UIWAttrItem4 = UIWAttrItem4 or DeclareMono("UIWAttrItem4", UIWidgetBaseItem)
function UIWAttrItem4:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	local name = data.name or data.attr_name or ""
	local value_str = data.value_text or data.value or data.attr_percent_value or ""
	if self.IsAdd then
		value_str = "+" .. value_str
	end
	if name == "" and data.attri_type then
		local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
		self:SetData(attri_info)
		return
	end
	UH.SetText(self.Name, name)
	UH.SetText(self.Value, value_str)
end

UIWAttrItem5 = UIWAttrItem5 or DeclareMono("UIWAttrItem5", UIWidgetBaseItem)
function UIWAttrItem5:SetData(data)
	local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
	UH.SetText(self.Text, string.format("%s:%s",attri_info.attr_name, attri_info.attr_percent_value))
end