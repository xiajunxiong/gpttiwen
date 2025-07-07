
function SocietyData:MailConfig()
end

function SocietyData:MailData()
    self.mail_list = {}
end

function SocietyData:SetMailListAck(protocol)
    local cur_sel_mail_index = nil ~= self.cur_sel_mail and self.cur_sel_mail.mail_index or nil
    if 1 == protocol.is_init then
        for _, mail in pairs(self.mail_list) do
            self:MailInfoRecycle(mail)
        end
        self.mail_list = {}
    end

    for i = 1, protocol.count do
        local mail_info = protocol.mail_info_list[i]
        local mail_index = mail_info.mail_index
        self.mail_list[mail_index] = self.mail_list[mail_index] or self:MailInfoGet()
        self.mail_list[mail_index]:MailInfo(mail_info)
    end
    self:MailRedPoint()
    self.cur_sel_mail = self.mail_list[cur_sel_mail_index]
    if nil == self.cur_sel_mail then ViewMgr:FlushView(SocietyView, "SocietyViewMailDetail") end
end

function SocietyData:SetMailDetailAck(protocol)
    local mail_detail = protocol.mail_detail
    local mail_index = mail_detail.mail_index
    if self.mail_list[mail_index] then
        self.mail_list[mail_index]:MailDetail(mail_detail)
    end
end

function SocietyData:SetFetchAttachmentAck(protocol)
    local fetch_info = protocol.fetch_info
    if 0 == fetch_info.ret then
        if self.mail_list[fetch_info.mail_index] then
            self.mail_list[fetch_info.mail_index]:ReadMail()
            self.mail_list[fetch_info.mail_index]:FetchMail()
        end
    end
    self:MailRedPoint()
end

function SocietyData:SetRecvNewMail(protocol)
    local mail_info = protocol.mail_info
    local mail_index = mail_info.mail_index
    self.mail_list[mail_index] = self:MailInfoGet()
    self.mail_list[mail_index]:MailInfo(mail_info)
    self:MailRedPoint(true)
    if not self.is_mail_shield_tip then
        PublicPopupCtrl.Instance:Center(Language.Society.MailTips)
    else
        if self.mail_shield_tip_list == nil or next(self.mail_shield_tip_list) == nil then
            self.mail_shield_tip_list = self.mail_shield_tip_list or {}
            table.insert(self.mail_shield_tip_list,Language.Society.MailTips)
        end
    end
end

-- 释放屏蔽的邮件提示
function SocietyData:ReleaseMailShieldTip()
    for i,v in pairs(self.mail_shield_tip_list) do
        PublicPopupCtrl.Instance:Center(v)
    end
    self.mail_shield_tip_list = {}
end

function SocietyData:SetMailDeleteAck(protocol)
    if 0 == protocol.ret then
        self:MailInfoRecycle(self.mail_list[protocol.mail_index])
        self.mail_list[protocol.mail_index] = nil
        self.cur_sel_mail = nil
    end
    self:MailRedPoint()
end

function SocietyData:GetMailList()
    local list = {}

    for _, v in pairs(self.mail_list) do
        table.insert(list, v)
    end
    table.sort(list, function (a, b)
        if not a or not b then return a end
        local ar = a.is_read and 1 or 0
        local br = b.is_read and 1 or 0
        if ar == br then
            if a.recv_time == b.recv_time then
                return a.sort_index > b.sort_index
            end
            return a.recv_time > b.recv_time
        end
        return ar < br
    end)
    return list
end

function SocietyData:MailRedPoint(new_mail)
    -- if new_mail then
    --     self.mail_red_point.val = true
    --     return
    -- end
    local mrp = false
	for _, mail in pairs(self.mail_list) do
		mrp = mrp or mail.mail_red_point.val
	end
	self.mail_red_point.val = mrp
end