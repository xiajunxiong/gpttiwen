
SocietyViewMailLeft = SocietyViewMailLeft or DeclareMono("SocietyViewMailLeft", UIWFlushPanelGroup)

function SocietyViewMailLeft:SocietyViewMailLeft()
    self.flush_events = {EventProtocol.ToEventKey(SCMailListAck), EventProtocol.ToEventKey(SCFetchAttachmentAck), EventProtocol.ToEventKey(SCRecvNewMail), EventProtocol.ToEventKey(SCMailDeleteAck)}
    self.data = SocietyCtrl.Instance:Data()
    self.data.cur_sel_mail = nil
end

function SocietyViewMailLeft:onFlush()
    local mail_list = self.data:GetMailList()
    local mail_num = #mail_list
    self.MailList:ClearSmData()
    self.MailList:SetData(mail_list)
    self.NoMailObj:SetActive(0 == mail_num)
    self.MailNum.text = string.format(Language.Society.MailNum, mail_num, SocietyConfig.MAIL_MAX_NUM)

    --是否可以领取邮件
    self.GetInter.Interactable = self.data.mail_red_point.val
    self.DeleteInter.Interactable = mail_num > 0
end

function SocietyViewMailLeft:OnClickGet()
    if self.GetInter.Interactable == false then
        PublicPopupCtrl.Instance:Center(Language.Society.NotGetInterTip)
        return
    end
    SocietyCtrl.Instance:SendMailOneKeyFetchAttachment()
end

function SocietyViewMailLeft:OnClickDelete()
    if self.DeleteInter.Interactable == false then
        PublicPopupCtrl.Instance:Center(Language.Society.NotDeleteInterTip)
        return
    end
    SocietyCtrl.Instance:SendMailClean()
end

SocietyViewMailLeftItem = DeclareMono("SocietyViewMailLeftItem", UIWidgetBaseItem)

function SocietyViewMailLeftItem:SocietyViewMailLeftItem()
    self.society_data = SocietyCtrl.Instance:Data()
    self.MailState = SocietyConfig.MailState
    self.MailTimeFormat = Language.Society.MailTimeFormat
end

function SocietyViewMailLeftItem:OnDestroy()
    self:uncareData()
end

function SocietyViewMailLeftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
    self.care_handles = {}
    self:careData(data.mail_red_point, BindTool.Bind(self.MailRedPoint, self))
    
    local info = data.info
    self.Name.text = RichTextHelper.ParseAll(info.mail_subject, false, RTParseType.SocietyMail)
    self.NameG.text = RichTextHelper.ParseAll(info.mail_subject, false, RTParseType.SocietyMail)
    local time = TimeHelper.GetServerTime() - info.recv_time
	local hour = math.floor(time / (60 * 60))
    local day = math.floor(hour / 24)
    local time_v = hour < 24 and (hour > 1 and string.format(self.MailTimeFormat[2], hour) or self.MailTimeFormat[1]) or string.format(self.MailTimeFormat[3], day)
    self.Time.text = time_v
    self.TimeG.text = time_v

    self:FlushRead()
end

function SocietyViewMailLeftItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyViewMailLeftItem:OnSelect(selected)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.society_data.cur_sel_mail = self.data
        self.data:ReadMail()
        self:FlushRead()
        if nil == self.data.detail then
            SocietyCtrl.Instance:SendMailRead(self.data.mail_index)
        else
            ViewMgr:FlushView(SocietyView, "SocietyViewMailDetail")
        end
    end
end

function SocietyViewMailLeftItem:FlushRead()
    local data = self.data
    self.State.SpriteName = data.is_read and self.MailState[1] or self.MailState[2]
    self.GreyObj:SetActive(data.is_read)
    self.Name.gameObject:SetActive(not data.is_read)
    self.Time.gameObject:SetActive(not data.is_read)
    self.NameG.gameObject:SetActive(data.is_read)
    self.TimeG.gameObject:SetActive(data.is_read)
end

function SocietyViewMailLeftItem:MailRedPoint()
    self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemObj, Vector3.New(185,25,0))
	self.red_point_item:SetNum(self.data.mail_red_point.val and 1 or 0)
end