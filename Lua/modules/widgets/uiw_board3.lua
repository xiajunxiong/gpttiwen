------Title====UI.Text
------BG====UI.Rect

UIWBoard3 = UIWBoard3 or DeclareMono("UIWBoard3")

function UIWBoard3:UIWBoard3()
	self.title_str = ""
	self.view_type = nil
	self.close_func = nil
end

function UIWBoard3:OnClickClose()
	if self.view_type ~= nil then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
		ViewMgr:CloseView(self.view_type)
	end
	if self.close_func then
		self.close_func()
	end
end

function UIWBoard3:SetData(view_type, title_str, v2)
	self.view_type = view_type
	self.title_str = title_str
	self.Title.text = self.title_str
	self.BG.sizeDelta = v2 or Vector2.New(672, 470)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)

	if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
		local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset("Board3AuditBg")
		if streaming_has then
			self.AuditBg:SetActive(true)
		end
	end
end

function UIWBoard3:ChangeTitle(title_str)
	self.Title.text = title_str
end

function UIWBoard3:OnRelease()
	self.close_func = nil
end

function UIWBoard3:SetCloseFunc(func)
	self.close_func = func
end

function UIWBoard3:SpriteName(name)
	UH.SpriteName(self.ViewBg,name)
end

--------------------------------
UIBoard3AuditBg = UIBoard3AuditBg or DeclareMono("UIBoard3AuditBg",UIWFlushPanel)
function UIBoard3AuditBg:UIBoard3AuditBg()
    self.setted = false
end

function UIBoard3AuditBg:OnEnable()
    if not self.setted then
        local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset("Board3AuditBg")
        if streaming_has then
            ChannelAgentConsts.SetRawImg(self.RawImg,path)
			self.setted = true
        end
    end
end