AnnouncementData = AnnouncementData or BaseClass()
function AnnouncementData:__init()
	if AnnouncementData.Instance ~= nil then
		Debuger.LogError("[AnnouncementData] attempt to create singleton twice!")
		return
	end	
    AnnouncementData.Instance = self
    -- 没有的话用这个
    self.announcement_url = GLOBAL_CONFIG:Urls().popup_notice_url or "http://cls.tt06.bluegames.cn/api/c2s/fetch_popup_notice.php"
    self.last_send_time = 0
    -- 点开的时间超过了5分钟就会去再检测
    self.send_interval = 300
    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
    self.is_show_data = SmartData.New({flag = false})
    -- 这个是有分割符的，现在废除
    self.config_data = SmartData.New({info = {}})
    -- 这个直接显示一个文本
    self.config_data2 = SmartData.New({info = {}})
    -- 是否显示红点
    self.red_remind = SmartData.New({flag = true})
end

function AnnouncementData:RoleNoticeComplete()
    -- self.config_data:Set({info = {}})
    self.config_data2:Set({info = {}})
    self.red_remind:Set({flag = true})
    AnnouncementCtrl.Instance:RequestAnnouncement(true)
end

function AnnouncementData:__delete()
	AnnouncementData.Instance = nil
end

function AnnouncementData:SetAnnouncementInfo(config)
    -- self.config_data.info = config
    self.config_data2.info = config
    self.is_show_data:Notify()
end

function AnnouncementData:GetAnnouncementInfo()
    local tab = {}
    if not TableIsEmpty(self.config_data2.info) then
        for i, v in ipairs(self.config_data2.info) do
            tab[i] = {}
            tab[i].title = v.title
            tab[i].content = {}
            -- 分隔这块直接砍掉
            -- local sp = Split(v.content, "@")
            -- for j, w in ipairs(sp) do
            --     if w == "" then
            --         table.remove(sp, j)
            --     end
            -- end
            -- if #sp % 2 == 1 then
            --     table.insert(sp, "分隔符错误")
            -- end
            -- local start_time = os.date("*t", v.beg_time)
            -- local end_time = os.date("*t", v.end_time)
            -- local first_data = {}
            -- first_data.title = Language.Announcement.TimeTitle
            -- first_data.content = Format(Language.Announcement.TimeContent,
            --     start_time.year, start_time.month, start_time.day, start_time.hour, start_time.min,
            --     end_time.year, end_time.month, end_time.day, end_time.hour, end_time.min)
            -- table.insert(tab[i].content, first_data)
            -- for j = 1, #sp, 2 do
            --     local t = {}
            --     t.title = sp[j]
            --     t.content = sp[j + 1]
            --     table.insert(tab[i].content, t)
            -- end
            table.insert(tab[i].content, {content = v.content})
        end
    end

    return tab
end
