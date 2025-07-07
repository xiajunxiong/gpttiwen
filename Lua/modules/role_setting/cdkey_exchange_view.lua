CDKeyExchangeView = CDKeyExchangeView or DeclareView("CDKeyExchangeView", "role_setting/cdkey_exchange")
VIEW_DECLARE_MASK(CDKeyExchangeView,ViewMask.BlockClose)

--CDKeyInput=====InpuField

function CDKeyExchangeView:OnClickExchange()
    self:Exchange(self.CDKeyInput.text)
end

function CDKeyExchangeView:OnClickClose()
    ViewMgr:CloseView(CDKeyExchangeView)
end

function CDKeyExchangeView:Exchange(cdkey)
    if StringIsEmpty(cdkey) then
        PublicPopupCtrl.Instance:Center(Language.CDKeyExchange.EmptyHint);
        return
    end
    local key = "33cc62b07ae98fffddd923b178aa0a14"
	local url = GLOBAL_CONFIG:Urls().gift_fetch_url
	if url == nil then
		Debuger.LogError("CDKeyExchangeView:Exchange ERROR:gift_fetch_url is nil")
		return
	end
    local params = {}
    local user_id = string.format("%s_%s",tostring(LoginData.Instance:GetPlatformName()),tostring(LoginData.Instance:GetLastLoginServer()))
	params.spid = LoginData.Instance:GetAccountSpid()								--平台ID
	params.server = LoginData.Instance:GetLastLoginServer()				            --服ID
	params.user = user_id									                        --平台帐号
	params.role = RoleData.Instance:GetRoleId()		  			                    --角色ID
	params.level = RoleData.Instance:GetRoleLevel()	  				                --角色等级
	params.card = cdkey or ""														--卡号
	params.time = os.time()												    		--时间戳
	params.sign = GameUtil.StrMd5(params.spid .. params.server .. params.user .. params.role .. params.level .. params.card .. params.time .. key)   --签名

    local req_fmt = "%s?spid=%s&server=%s&user=%s&role=%s&level=%s&card=%s&time=%s&sign=%s"
    local req_str = string.format(req_fmt, url, params.spid, tostring(params.server), params.user, 
        tostring(params.role), tostring(params.level), params.card, tostring(params.time), params.sign)
    -- LogError("req_str==",req_str)
    WebRequester:Request(req_str,function(data,error) 
        if not StringIsEmpty(error)  then
            Debuger.LogError("CDKeyExchangeView:Exchange WebRequest ERROR:%s,URL=%s",error,req_str)
        else
            local ret_data,suc = TryParseJson(data)
            if suc then 
                -- LogError("SUC====",ret_data)
                local ret_code = ret_data.ret
                PublicPopupCtrl.Instance:Center(Language.CDKeyExchange.RetCode[ret_code] or string.format(Language.CDKeyExchange.OtherError,ret_code))
                if ret_code == 0 then
                    ViewMgr:CloseView(CDKeyExchangeView)
                end
            else
                Debuger.LogError("CDKeyExchangeView:Exchange WebRequst ParseRetJson ERROR data=%s,URL=%s",data,url)
            end
        end
    end)

end