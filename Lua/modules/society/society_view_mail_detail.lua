
SocietyViewMailDetail = SocietyViewMailDetail or DeclareMono("SocietyViewMailDetail", UIWFlushPanel)

function SocietyViewMailDetail:SocietyViewMailDetail()
    self.flush_events = {EventProtocol.ToEventKey(SCMailDetailAck), EventProtocol.ToEventKey(SCFetchAttachmentAck), EventProtocol.ToEventKey(SCMailDeleteAck)}
    self.data = SocietyCtrl.Instance:Data()
    self.link_mark = {}
    self.MailRemainTime = Language.Society.MailRemainTime
    RichTextHelper.Bind(self.Desc, BindTool.Bind(RichTextItem.OnClickLink, self))
end

function SocietyViewMailDetail:onFlush()
    local empty = nil == self.data.cur_sel_mail or nil == self.data.cur_sel_mail.detail
    self.TipsObj:SetActive(empty)
    self.LineObj:SetActive(not empty)
    self.DescTitleObj:SetActive(not empty)
    self.RewardObj:SetActive(not empty and #self.data.cur_sel_mail.item_list > 0)
    self.BtnDeleteObj:SetActive(not empty)
    self.BtnGetObj:SetActive(not empty)
    self.Name.text = ""
    self.Desc.text = ""
    self.Timer.text = ""
    if empty then return end
    local cur_sel_mail = self.data.cur_sel_mail
    local info = cur_sel_mail.info
    local detail = cur_sel_mail.detail
    self.Name.text = cur_sel_mail.mail_name
    self.Desc.text = RichTextHelper.ParseAllFunc(detail.content_txt, true, function (paramt)
        self.link_mark[paramt.mark] = paramt.address
    end, RTParseType.SocietyMail)
    local time = SocietyConfig.MAIL_MAX_DAY * 24 * 60 * 60 - (TimeHelper.GetServerTime() - info.recv_time)
	local hour = math.floor(time / (60 * 60))
    local day = math.floor(hour / 24)
	local hour = math.ceil(time / (60 * 60) % 24)
    self.Timer.text = day > 0 and string.format(self.MailRemainTime[1], day, hour) or string.format(self.MailRemainTime[2], hour)
    local item_list = DivineCostumeData.CheckEquip(cur_sel_mail.item_list)
    self.RewardList:SetData(item_list)
    --是否可以领取邮件
    self.GetInter.Interactable = self.data.cur_sel_mail.mail_red_point.val
end

function SocietyViewMailDetail:OnClickGet()
    if self.GetInter.Interactable == false then
        PublicPopupCtrl.Instance:Center(Language.Society.NotGetInterTip)
        return
    end
    SocietyCtrl.Instance:SendMailFetchAttachment{mail_index = self.data.cur_sel_mail.mail_index}
end

function SocietyViewMailDetail:OnClickDelete()
    AudioMgr:PlayEffect(AudioType.UI, "bag_tidy")
    if self.data.cur_sel_mail then
        SocietyCtrl.Instance:SendMailDelete(self.data.cur_sel_mail.mail_index)
    end
end