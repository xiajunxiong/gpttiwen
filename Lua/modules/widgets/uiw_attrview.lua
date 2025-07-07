--对外接口 SetData
--data 展示参数
--hide_zero 默认不设置(true 数值为0的不展示)
--示例 partner_view_attr_up

UIWAttr = UIWAttr or DeclareMono("UIWAttr")

function UIWAttr:SetData(data, hide_zero)
end

function UIWAttr:SetText(obj, text)
	if obj ~= nil then
		obj.text = text
	end
end

function UIWAttr:SetProgress(progress, params)
	if progress ~= nil then
		progress:SetProgress(params.p_val)
		progress:SetText(params.t_val)
	end
end

function UIWAttr:SetActive(obj, val)
	if obj ~= nil then
		obj:SetActive(val ~= 0)
	end
end

-- 基础属性
UIWAttr1 = UIWAttr1 or DeclareMono("UIWAttr1", UIWAttr)

function UIWAttr1:SetData(data, hide_zero)
	self:SetText(self.ValAtk, data[BattleAttriType.Attack])
	self:SetText(self.ValDef, data[BattleAttriType.Defense])
	self:SetText(self.ValDex, data[BattleAttriType.Agility])
	self:SetText(self.ValWil, data[BattleAttriType.Mental])
	self:SetText(self.ValRec, data[BattleAttriType.Recovery])

	local cur_hp = data[BattleAttriType.CurHp] or 0
	local max_hp = data[BattleAttriType.MaxHp] or 1
	max_hp = max_hp == 0 and 1 or max_hp
	self:SetProgress(self.ValHp, {
		p_val = cur_hp / max_hp,
		t_val = data.hp_text or string.format(self.IsDeep and Language.Common.Progress2TxtColorDFormat or Language.Common.Progress2TxtColorBFormat, cur_hp, max_hp),
	})
	local cur_mp = data[BattleAttriType.CurMp] or 0
	local max_mp = data[BattleAttriType.MaxMp] or 1
	max_mp = max_mp == 0 and 1 or max_mp
	self:SetProgress(self.ValMp, {
		p_val = cur_mp / max_mp,
		t_val = data.mp_text or string.format(self.IsDeep and Language.Common.Progress2TxtColorDFormat or Language.Common.Progress2TxtColorBFormat, cur_mp, max_mp),
	})

	if not hide_zero then return end

	self:SetActive(self.ObjAtk, data[BattleAttriType.Attack])
	self:SetActive(self.ObjDef, data[BattleAttriType.Defense])
	self:SetActive(self.ObjDex, data[BattleAttriType.Agility])
	self:SetActive(self.ObjWil, data[BattleAttriType.Mental])
	self:SetActive(self.ObjRec,data[BattleAttriType.Recovery])
end

local function PercentCheck(value,retain)
	local color
	if "string" == type(value) then
		_, _, color = string.find(value, "#(%w+)>")
		value = string.gsub(value, "<color=#%w+>", "")
		value = string.gsub(value, "</color>", "")
		value = tonumber(value)
	end
	return color and RichTextHelper.ColorStr(Percent(value, retain), color) or Percent(value, retain)
end

-- 抵抗属性
UIWAttr2 = UIWAttr2 or DeclareMono("UIWAttr2", UIWAttr)

function UIWAttr2:SetData(data, hide_zero)
	self:SetText(self.ValDu, PercentCheck(data[BattleAttriType.AntiPosition]))
	self:SetText(self.ValShiHua, PercentCheck(data[BattleAttriType.AntiPetrify]))
	self:SetText(self.ValHunShui, PercentCheck(data[BattleAttriType.AntiSleep]))
	self:SetText(self.ValHunLuan, PercentCheck(data[BattleAttriType.AntiChaos]))
	self:SetText(self.ValZui, PercentCheck(data[BattleAttriType.AntiDrunk]))
	self:SetText(self.ValSilence, PercentCheck(data[BattleAttriType.AntiSilence]))
	self:SetText(self.ValServerInjuer, PercentCheck(data[BattleAttriType.AntiSevereInjuer]))

	if not hide_zero then return end

	self:SetActive(self.ObjDu, data[BattleAttriType.AntiPosition])
	self:SetActive(self.ObjShiHua, data[BattleAttriType.AntiPetrify])
	self:SetActive(self.ObjHunShui, data[BattleAttriType.AntiSleep])
	self:SetActive(self.ObjHunLuan, data[BattleAttriType.AntiChaos])
	self:SetActive(self.ObjZui, data[BattleAttriType.AntiDrunk])
end

-- 修正属性
UIWAttr3 = UIWAttr3 or DeclareMono("UIWAttr3", UIWAttr)

function UIWAttr3:SetData(data, hide_zero)
	self:SetText(self.ValBiSha, PercentCheck(data[BattleAttriType.Critical]))
	self:SetText(self.ValMingZhong, PercentCheck(data[BattleAttriType.Hit]))
	self:SetText(self.ValShanBi, PercentCheck(data[BattleAttriType.Dodge]))
	self:SetText(self.ValFanJi, PercentCheck(data[BattleAttriType.CounterAttack]))

	self:SetText(self.ValMoGong, data[BattleAttriType.MagicAttack])
	self:SetText(self.ValMoKang, data[BattleAttriType.MagicDefense])

	self:SetText(self.ValBiShaShangHai, PercentCheck(data[BattleAttriType.CriticalIncValue]))
	self:SetText(self.ValJianRen, PercentCheck(data[BattleAttriType.CriticalDecValue]))

	if not hide_zero then return end

	self:SetActive(self.ObjBiSha, data[BattleAttriType.Critical])
	self:SetActive(self.ObjMingZhong, data[BattleAttriType.Hit])
	self:SetActive(self.ObjShanBi, data[BattleAttriType.Dodge])
	self:SetActive(self.ObjFanJi, data[BattleAttriType.CounterAttack])
	self:SetActive(self.ObjMoGong, data[BattleAttriType.MagicAttack])
	self:SetActive(self.ObjMoKang, data[BattleAttriType.MagicDefense])
	self:SetActive(self.ObjBiShaShangHai, data[BattleAttriType.CriticalIncValue])
	self:SetActive(self.ObjJianRen, data[BattleAttriType.CriticalDecValue])
end


-- 属性提升
-- 已应用：partner_lock
UIWAttrChange = UIWAttrChange or DeclareMono("UIWAttrChange")
function UIWAttrChange:UIWAttrChange()
end

function UIWAttrChange:SetData(data)
	for k,v in pairs(data.attr_list) do 
		local attr_name = AttrNameForType[k]
		if attr_name and self["Attr_"..attr_name] then 
			self["Attr_"..attr_name].text = DataHelper.Percent(k,v)
		end 
	end 

	for k,v in pairs(data.attr_neo_list) do
		local attr_name = AttrNameForType[k]
		if attr_name and self["Neo_"..attr_name] then 
			self["Neo_"..attr_name].text = DataHelper.Percent(k,v)
		end 
	end
end