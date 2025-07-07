LoginNoticeView = LoginNoticeView or DeclareView("LoginNoticeView","login/login_notice_view",0,ViewLayer.Top)
VIEW_DECLARE_MASK(LoginNoticeView,ViewMask.None)
LoginNoticeType = {
    Normal = 1,
    DeleteAccount = 2,
}
function LoginNoticeView:LoginNoticeView()
    self.link_mark = {}
end 

function LoginNoticeView:CloseCallback()
    LoginData.Instance.notice_view_smart.notice_type = LoginNoticeType.Normal
end

function LoginNoticeView:LoadCallback(param)
    if param and param.delete_account then
        self.Board:SetData(self:GetType(), Language.Login.DeleteAccountNotice)
        LoginData.Instance.notice_view_smart.notice_type = LoginNoticeType.DeleteAccount
        local text_data = LoginData.Instance:GetDeleteAccountNoticeData()
        if next(text_data) ~= nil then
            self.NoticeList:SetData(text_data.content)
            self.NoticeList:ClickItem(1)
        end
    else
        self.Board:SetData(self:GetType(),Language.Login.LoginNotice)
        LoginData.Instance.notice_view_smart.notice_type = LoginNoticeType.Normal
        local text_data = LoginData.Instance:GetNoticeData()
        self.NoticeList:SetData(text_data.content)
        self.NoticeList:ClickItem(1)
    end
    -- for i = 1,8 do 
    --     if text_data[i] then 
    --         self.NoticeCells[i]:SetData(text_data[i])
    --     else 
    --         self.NoticeCells[i]:CloseView()
    --     end 
    -- end 

    -- self.NoticeCells[1].Toggle.isOn = true
    -- self:OnClickItem(text_data[1])
end

function LoginNoticeView:FlushView()

end

function LoginNoticeView:WebLinkMark()
    -- body
end

function LoginNoticeView:OnClickClose()
    ViewMgr:CloseView(LoginNoticeView)
end 

function LoginNoticeView:OnClickItem(data)
    -- self.MainText.text =
    if not data.text then
        data.text = "这一些是公告的说哦\n接下来是网址哦{web_link;<color=#E75FFF>点击前往</color>;23cdf1;1;http://www.baidu.com}"
    end
    -- local str = "这一些是公告的说哦\n接下来是网址哦{web_link;<color=#E75FFF>点击前往</color>;23cdf1;1;http://www.baidu.com}"
	Debuger.LogError("OnClickItem:测试=%s",tostring(data.text))
    self.MainText.text = RichTextHelper.ParseAllFunc(data.text, true
    ,function(paramt)
        self.link_mark[paramt.mark] = paramt.address
    end)

    local split = Split(data.title,"|")
    if split[2] then 
        self.NoticeTitle.text = split[1]..split[2]
    else 
        self.NoticeTitle.text = split[1]
    end 
    RichTextHelper.Bind(self.MainText, BindTool.Bind(self.OnClickLink, self))
end

function LoginNoticeView:OnClickLink(dict)
    if nil == dict then
		return
    end
    local t = RichTextHelper.DictionarySSToTable(dict, true)
    local key = t[1]
    if "weblink" == key then
        if self.link_mark[t[2]] ~= nil then 
            UnityApp.OpenURL(self.link_mark[t[2]])
        end 
    end 
end
------------------------------------------
LoginNoticePanel = LoginNoticePanel or DeclareMono("LoginNoticePanel", UIWFlushPanel)
function LoginNoticePanel:LoginNoticePanel()
    self.data_cares = {
        {data = LoginData.Instance.notice_view_smart, func = self.FlushPanel, keys = {"flush_panel"}}
    }
end

function LoginNoticePanel:FlushPanel()
    if LoginData.Instance.notice_view_smart.notice_type == LoginNoticeType.DeleteAccount then
        local text_data = LoginData.Instance:GetDeleteAccountNoticeData()
        if next(text_data) ~= nil then
            self.NoticeList:SetData(text_data.content)
            self.NoticeList:ClickItem(1)
        end
    end
end
------------------------------------------
NoticeItemCell = NoticeItemCell or DeclareMono("NoticeItemCell",UIWidgetBaseItem)

function NoticeItemCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local split = Split(data.title,"|")
    
    if split[2] then 
        self.UnSelectTxt.text =split[1].."\n"..split[2]
        self.SelectTxt.text = split[1].."\n"..split[2]
    else 
        self.UnSelectTxt.text =split[1]
        self.SelectTxt.text = split[1]
    end 
end 

function NoticeItemCell:CloseView()
    self.UnSelectCon:SetActive(false)
    self.SelectCon:SetActive(false)
end

NoticeItemNeoCell = NoticeItemNeoCell or DeclareMono("NoticeItemNeoCell",UIWidgetBaseItem)

function NoticeItemNeoCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local split = Split(data.title,"|")

    if split[2] then 
        UH.SetText( self.NomalTwice , split[1].."\n"..split[2])
        UH.SetText( self.SelectTwice , split[1].."\n"..split[2])

        self.NomalSingle.gameObject:SetActive(false)
        self.SelectSingle.gameObject:SetActive(false)
    else 
        UH.SetText( self.NomalSingle , split[1])
        UH.SetText( self.SelectSingle , split[1])

        self.NomalTwice.gameObject:SetActive(false)
        self.SelectTwice.gameObject:SetActive(false)
    end 

end 

function NoticeItemNeoCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end