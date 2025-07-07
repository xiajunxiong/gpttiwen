-----1.角色头像  sworn_avatar.prefab
SwornAvatar = SwornAvatar or DeclareMono("SwornAvatar")
--[[ data = {
	avatar_type, avatar_id 显示头像
	level 显示等级。
}]]
function SwornAvatar:SetData(data)
	self.Avatar:SetAvatar(data.avatar_type, data.avatar_id,data.avatar_quality)

	-- if data.top_level and data.top_level > 0 then
	-- 	self.PeakLevel:SetActive(true)
	-- 	UH.SetText(self.Level, data.top_level)
	-- 	UH.Color(self.Level, COLORS.Red8)
	-- else
		UH.SetText(self.Level, data.level)
		UH.Color(self.Level, COLORS.White)
		self.PeakLevel:SetActive(false)
	-- end
end

--设置等级是否显示，默认显示
function SwornAvatar:SetLevelTextActive(is_show)
	self.Level.gameObject:SetActive(is_show)
end
