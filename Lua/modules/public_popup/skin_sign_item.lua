
--皮肤限定标记
SkinSignItem = DeclareMono("SkinSignItem", UIWidgetBaseItem)

function SkinSignItem:SetData(data)
	-- self.ImageShow.SpriteName = string.format("_Loc%s", data.limit_sign)
	local is_show = not StringIsEmpty(data.limit_sign)
	if is_show then
		self.ImageShow:SetByPath(string.format("skin_sign/_Loc%s.png", data.limit_sign))
	end
	self.ImageShow:SetObjActive(is_show)
end
