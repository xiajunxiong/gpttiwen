UserProtocolView = UserProtocolView or DeclareView("UserProtocolView", "login/user_protocol",0,ViewLayer.Top)
-- VIEW_DECLARE_LEVEL(Workshop, ViewLevel.L1)
function UserProtocolView:UserProtocolView()
	self.link_mark = {}
end

function UserProtocolView:LoadCallback(param_t)
    self.board:SetData(self:GetType(),Language.Login.LoginUserProtocolTitle)

    local text_data = LoginData.Instance:GetUserProtocol()
	local user_text = RichTextHelper.ParseAllFunc(text_data.user_agree_url, true ,function(paramt) self.link_mark[paramt.mark] = paramt.address end)
	local privacy_text = RichTextHelper.ParseAllFunc(text_data.privacy_url, true ,function(paramt) self.link_mark[paramt.mark] = paramt.address end)
	local content_format = RichTextHelper.ParseAllFunc(text_data.content, true)
	UH.SetText(self.text,Format(content_format,user_text,privacy_text))
	RichTextHelper.Bind(self.text, BindTool.Bind(self.OnClickLink, self))
	-- UH.SetText(self.text,text_data.content)
end 

function UserProtocolView:OnClickLink(dict)
	if nil == dict then return end
    local t = RichTextHelper.DictionarySSToTable(dict, true)
    local key = t[1]
    if "weblink" == key then
		if self.link_mark[t[2]] ~= nil then
            UnityApp.OpenURL(self.link_mark[t[2]])
        end
    end
end

function UserProtocolView:OnClickOk()
    ViewMgr:CloseView(UserProtocolView)
end

AgeNoticeView = AgeNoticeView or DeclareView("AgeNoticeView", "login/age_notice",0, ViewLayer.Top)
function AgeNoticeView:AgeNoticeView()
end

function AgeNoticeView:LoadCallback()
	self.board:SetData(self:GetType(),Language.Login.AgeNotice)
	UH.SetText(self.text, Language.Login.AgeNoticeTip)
end

function AgeNoticeView:OnClickOk()
	ViewMgr:CloseView(AgeNoticeView)
end