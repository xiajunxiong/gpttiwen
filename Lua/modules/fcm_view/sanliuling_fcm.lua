SanLiuLingFcmView = SanLiuLingFcmView or DeclareView("SanLiuLingFcmView", "pc_fcm/sanliuling_fcm")
VIEW_DECLARE_LEVEL(SanLiuLingFcmView, ViewLevel.L1)
VIEW_DECLARE_MASK(SanLiuLingFcmView, ViewMask.BgBlock)
function SanLiuLingFcmView:SanLiuLingFcmView()
end

function SanLiuLingFcmView:LoadCallback()
    local item_list = LoginData.Instance:GetRealNameReward()
    self.ItemList:SetData(item_list)
end

function SanLiuLingFcmView:OnClickGet()
    local name = self.NameInput.text
    local id_num = self.IdNumInput.text
    if StringIsEmpty(name) or StringIsEmpty(id_num) then
        PublicPopupCtrl.Instance:Center(Language.PCNotice.InputError)
        return
    end

    if StringIsEmpty(GLOBAL_CONFIG:Urls().app_key) then
        PublicPopupCtrl.Instance:Center(Language.PCNotice.ParamError)
        return
    end
    local url_head = "http://game.api.1360.com/authsubmit"
    local form = {}
    form.gkey = "wlshzjpc"
    form.idcard = id_num
    form.name = WebRequester:UrlEncode(name)
    form.time = os.time()
    form.qid = LoginData.Instance:GetAccountKey()
    form.sign = GameUtil.StrMd5(form.gkey .. form.idcard .. name .. form.qid .. GLOBAL_CONFIG:Urls().app_key)

    local call_back = function(data,error)
        if StringIsEmpty(error) then
            if GLOBAL_CONFIG:Flags().log_print then
                if ChannelAgent.PCFileLog then
                    ChannelAgent.PCFileLog("RealNameSign ", data, form)
                end
            end
            local remote ,status = TryParseJson(data)
            if status == true and remote.errno ~= nil then
                if remote.errno ~= 0 then
                    local tips_str = Format(Language.PCNotice.Error, tostring(remote.errno))
                    PublicPopupCtrl.Instance:Center(tips_str)
                elseif remote.data and remote.data.can_play then
                    if remote.data.can_play == 1 then
                        PublicPopupCtrl.Instance:Center(Language.PCNotice.RealNameSignSucc)
                        LoginCtrl.Instance:SendRealNameRewardReq()
                        LoginData.Instance:SetPCSdkExtraInfo(1)
                        ViewMgr:CloseView(SanLiuLingFcmView)
                    elseif not StringIsEmpty(remote.data.prompt_info) then
                        local info = {}
                        info.content = remote.data.prompt_info
                        info.no_cancel = true
                        PublicPopupCtrl.Instance:DialogTips(info)
                    end
                end
            end
        end
    end
    WebRequester:Post(url_head, call_back, form)
end
