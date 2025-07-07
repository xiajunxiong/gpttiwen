AppraiseHasGiftView = AppraiseHasGiftView or DeclareView("AppraiseHasGiftView", "appraise_has_gift/appraise_has_gift")
VIEW_DECLARE_LEVEL(AppraiseHasGiftView, ViewLevel.L1)
VIEW_DECLARE_MASK(AppraiseHasGiftView, ViewMask.BgBlock)

function AppraiseHasGiftView:AppraiseHasGiftView()
    self.Data = AppraiseHasGiftData.Instance
    self.ht = self.Data.info_data:Care(BindTool.Bind(self.FlushShow, self))
    self.ObjCanReceive:SetActive(false)
end

function AppraiseHasGiftView:LoadCallback()
    UH.SetText(self.TxtDesc, Language.AppraiseHasGift.Desc)
    self.Cell:SetData(Item.Create(self.Data:Reward()))
end

function AppraiseHasGiftView:FlushShow()
    if self.Data:IsReceive() then
        ViewMgr:CloseView(AppraiseHasGiftView)
    end
end

function AppraiseHasGiftView:CloseCallback()
    self.Data.info_data:Uncare(self.ht)
end

function AppraiseHasGiftView:OnCloseClick()
    ViewMgr:CloseView(AppraiseHasGiftView)
end

function AppraiseHasGiftView:OnReceiveClick()
    AppraiseHasGiftCtrl.Instance:SendReq()
end

function OpenURL(url)
    local openURL = UnityEngine.Application.OpenURL(url)
    return openURL
end

function AppraiseHasGiftView:OnAppraiseClick()
    local url = GLOBAL_CONFIG:Urls().pay_cz
	local account = LoginData.Instance:GetAccountKey()
    local cxurl = url .. "?account=" .. account	
	print("dev 222222222 "..cxurl)
    OpenURL(cxurl);
end



function AppraiseHasGiftView:OnDduiHuan()
    local number = self.InputNumber.text
    local url = GLOBAL_CONFIG:Urls().pay_duihuan
	local account = LoginData.Instance:GetAccountKey()
	local sign = GameUtil.StrMd5( "bnb" .. account .. MainProber.role_id .. os.time() ..number .. "wanwuguiyuan"   )
    url = url .. "?type=%s&spid=%s&role_id=%s&server_id=%s&time=%s&sign=%s&number=" .. number
	url = string.format(url, "bnb", LoginData.Instance:AccountData().spid, MainProber.role_id, MainProber.server_id, os.time(), sign)
    print("代币兑换============== "..url)
    -- 发送请求
    WebRequester:Request(
        url,
        function(data, error)
		 
        local ret_t, status = TryParseJson(data)
		
		
                if ret_t.ret==0 then
				       print("1111111111============== "..ret_t.data)
				    ChatCtrl.Instance:ChannelChatCustom{content=ret_t.data}
                    PublicPopupCtrl.CenterRich(ret_t.data)
                end
				if ret_t.ret==1 then
				   ChatCtrl.Instance:ChannelChatCustom{content=ret_t.data}
                   PublicPopupCtrl.CenterRich(ret_t.data)
                end
				if ret_t.ret==2 then
				   ChatCtrl.Instance:ChannelChatCustom{content=ret_t.data}
                   PublicPopupCtrl.CenterRich(ret_t.data)
                end	
    end)
end