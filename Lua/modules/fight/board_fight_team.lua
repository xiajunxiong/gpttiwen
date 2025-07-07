
BoardFightTeam = BoardFightTeam or DeclareMono("BoardFightTeam")

function BoardFightTeam:BoardFightTeam()
	self.view_type = nil
end

function BoardFightTeam:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
	if self.close_func then
		Call(self.close_func)
		self.close_func = nil
	end
end

function BoardFightTeam:SetData(view_type,title_str)
	self.view_type = view_type
	self.Title.text = title_str
end

function BoardFightTeam:SetData2(view_type,title_str,title2_str, close_func)
	self.view_type = view_type
	self.Title.text = title_str
	self.Title2.text = title2_str
	-- self.close_func = close_func
	-- self.BG.sizeDelta = v2 or Vector2.New(831, 432)
end

function BoardFightTeam:SetData3(view_type,title_str,title2_str,title3_str)
	self.view_type = view_type
	if title3_str and title3_str ~= "" then
		self.Title.text = ""
		self.Title2.text = ""
		self.Title3.text = title3_str
	else
		self.Title.text = title_str
		self.Title2.text = title2_str
		self.Title3.text = ""
	end

end

function BoardFightTeam:SetData4(view_type, title_str, v2)
	self.view_type = view_type
	self.Title.text = ""
	self.Title2.text = ""
	self.Title3.text = title_str
	self.BG.sizeDelta = v2 or Vector2.New(831, 432)
end