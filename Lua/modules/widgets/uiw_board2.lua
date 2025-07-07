------Title====UI.Text
------BG====UI.Rect
------SizeX===Int
------SizeY===Int
------HideJiaoHua===bool
------HideDiwen===bool
UIWBoard2 = UIWBoard2 or DeclareMono("UIWBoard2")

function UIWBoard2:UIWBoard2()
	self.title_str = ""
	self.view_type = nil
	self.close_func = nil

end

function UIWBoard2:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
		if self.close_func then
			self.close_func()
		end
	end
end

function UIWBoard2:SetData(view_type, title_str, v2, title_m)
	self.view_type = view_type
	self:SetTitle(title_str)
	self:SetTitleMid(title_m or false)
	if v2 ~= nil then
		self.BG.sizeDelta = v2
	else
		if self.SizeX ~= nil and self.SizeX > 0 then
			self.BG.sizeDelta = Vector2.New(self.SizeX,self.SizeY)
		end
	end
	if self.HideJiaoHua then
		self:SetHideJiaoHua()
	end
	if self.HideDiwen then
		self:SetHideDiwen()
	end
	if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
		local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset("Board2AuditBg")
		if streaming_has then
			self.AuditBg:SetActive(true)
		end
	end
end

function UIWBoard2:OnRelease()
	self.close_func = nil
end

function UIWBoard2:SetCloseFunc(func)
	self.close_func = func
end

function UIWBoard2:SetTitle(title_str)
	self.title_str = title_str
	self.Title.text = self.title_str
	self.TitleM.text = self.title_str
end

function UIWBoard2:SetTitleMid(value)
	self.Title:SetObjActive(not value)
	self.TitleM:SetObjActive(value)
end

function UIWBoard2:SetHideJiaoHua()
	self.RightJiaoHua:SetActive(not self.HideJiaoHua)
	self.LeftJiaoHua:SetActive(not self.HideJiaoHua)
end

function UIWBoard2:SetHideDiwen()
	self.DiWen:SetActive(not self.HideDiwen)
end

--------------------------------
UIBoard2AuditBg = UIBoard2AuditBg or DeclareMono("UIBoard2AuditBg",UIWFlushPanel)
function UIBoard2AuditBg:UIBoard2AuditBg()
    self.setted = false
end

function UIBoard2AuditBg:OnEnable()
    if not self.setted then
        local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset("Board2AuditBg")
        if streaming_has then
            ChannelAgentConsts.SetRawImg(self.RawImg,path)
			self.setted = true
        end
    end
end