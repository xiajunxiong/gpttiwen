FeedBackView = FeedBackView or DeclareView("FeedBackView", "role_setting/feedback_view")
VIEW_DECLARE_MASK(FeedBackView,ViewMask.BlockClose)

function FeedBackView:FeedBackView()

end 

function FeedBackView:LoadCallback()
    self.AdviceToggle.isOn = true
    self.feed_type = 1
end

function FeedBackView:OnFlush()

end

function FeedBackView:OnClose()
    ViewMgr:CloseView(FeedBackView)
end

function FeedBackView:OnClickFeedType(_type)
    self.feed_type = _type
end

function FeedBackView:OnClickSubmit()
    if StringIsEmpty(self.Input.text) then
        PublicPopupCtrl.Instance:Center(Language.RoleSetting.FeedBackEmptyTip)
        return
    end
	local pkg_config = GLOBAL_CONFIG:PkgInfo()
    local platform = LoginData.Instance:GetPlatformName()
    
    local info_catch = {}
    info_catch.zone_id = pkg_config.agent_id
    info_catch.server_id = LoginData.Instance:GetLastLoginServer()
    info_catch.user_id = string.gsub(platform,pkg_config.agent_id .. "_", "")
    info_catch.role_id = RoleData.Instance:GetRoleId()
    info_catch.role_name = RoleData.Instance:GetBaseData().name
    info_catch.role_level = RoleData.Instance:GetRoleLevel()
    info_catch.role_vip = 0
    info_catch.role_gold = RoleData.Instance:GetCurrency(CurrencyType.Gold)
    info_catch.role_scene = SceneData.Instance:SceneId()
    info_catch.user_device = string.gsub(tostring(UnityEngine.SystemInfo.deviceModel)," ","_")
    info_catch.user_ver = pkg_config.version
    info_catch.issue_type = self.feed_type
    info_catch.issue_subject = Language.RoleSetting.FeedBackType[self.feed_type]
    info_catch.issue_content = self.Input.text

    local url_format = GLOBAL_CONFIG:Urls().gm_report_url or "http://api.tt06.bluegames.cn/client/gm/report"
    local url = string.format(url_format.."?zone_id=%s&server_id=%s&user_id=%s&role_id=%s&role_name=%s&role_level=%s&role_gold=%s&role_vip=%s&role_scene=%s&issue_type=%s&user_device=%s&user_ver=%s&issue_subject=%s&issue_content=%s"
        ,info_catch.zone_id,info_catch.server_id,info_catch.user_id,info_catch.role_id,info_catch.role_name,info_catch.role_level,info_catch.role_gold,info_catch.role_vip
        ,info_catch.role_scene,info_catch.issue_type,info_catch.user_device,info_catch.user_ver,info_catch.issue_subject,info_catch.issue_content)

        WebRequester:Request(url,function(data,error)
		if StringIsEmpty(error) == true then
			local remote_config,suc = TryParseJson(data)
            if suc == true then
                PublicPopupCtrl.Instance:Center(Language.RoleSetting.FeedBack)
            else	--remote data error 
                Debuger.LogError("FeedBack ERROR,ret=%s",tostring(remote_config))
			end
		else	--remote request failed
		end
	end)
    ViewMgr:CloseView(FeedBackView)
end