FILE_REQ("modules/announcement/announcement_data")

VIEW_REQ("modules/announcement/announcement_view")

AnnouncementCtrl = AnnouncementCtrl or BaseClass(BaseCtrl)
function AnnouncementCtrl:__init()
	if AnnouncementCtrl.Instance ~= nil then
		Debuger.LogError("[AnnouncementCtrl] attempt to create singleton twice!")
		return
	end	
	AnnouncementCtrl.Instance = self
	self.data = AnnouncementData.New()
end

function AnnouncementCtrl:__delete()
	if AnnouncementCtrl.Instance == self then
		AnnouncementCtrl.Instance = nil
	end
end

function AnnouncementCtrl:OnUnloadGameLogic()
	self.data:SetAnnouncementInfo({})
end

-- 请求游戏内公告信息（是否强行发送公告，正常需要检查上一次发送的时间）
function AnnouncementCtrl:RequestAnnouncement(flag)
    local send_flag = (flag ~= nil) or (TimeCtrl.Instance:GetServerTime() - self.data.last_send_time > self.data.send_interval)
            LogError("RequestAnnouncement:请求公告信息, flag = " .. flag)
    if send_flag then
        self.last_send_time = TimeCtrl.Instance:GetServerTime()
        local url = GLOBAL_CONFIG:Urls().popup_notice_url or self.data.announcement_url
        url = url .. "?spid=" .. LoginData.Instance:AccountData().spid .."&server_id=" .. MainProber.server_id
        LogError("RequestAnnouncement:请求公告信息, url = " .. url)
        WebRequester:Request(url, function(data, error)
            if StringIsEmpty(error) == true then
                local remote_config, suc = TryParseJson(data)
                if suc == true and remote_config.data ~= cjson.null and remote_config.ret == 0 then
                    self.data:SetAnnouncementInfo(remote_config.data)
                else
                    self.data:SetAnnouncementInfo({})
                end
            else
                self.data:SetAnnouncementInfo({})
            end
        end)
    end
end