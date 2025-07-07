AnnouncementView = AnnouncementView or DeclareView("AnnouncementView", "announcement/announcement")
VIEW_DECLARE_LEVEL(AnnouncementView, ViewLevel.L1)
VIEW_DECLARE_MASK(AnnouncementView, ViewMask.BgBlock)
function AnnouncementView:AnnouncementView()
    AnnouncementData.Instance.red_remind.flag = false
    AnnouncementData.Instance.is_show_data:Notify()
end

function AnnouncementView:OnCloseClick()
    ViewMgr:CloseView(AnnouncementView)
end


----------------------------AnnouncementPanel----------------------------
AnnouncementPanel = AnnouncementPanel or DeclareMono("AnnouncementPanel", UIWFlushPanel)
function AnnouncementPanel:AnnouncementPanel()
    self.Data = AnnouncementData.Instance
    self.data_cares = {
        {data = self.Data.is_show_data, func = self.FlushData, init_call = true},
    }
    self.now_page = 1
    self.max_page = 1
    AnnouncementCtrl.Instance:RequestAnnouncement()
end

function AnnouncementPanel:OnPageChange(param)
    self.now_page = self.now_page + param
    self.now_page = self.now_page < 1 and 1 or self.now_page
    self.now_page = self.now_page > self.max_page and self.max_page or self.now_page
    self:FlushShow()
end

function AnnouncementPanel:FlushData()
    self.config = self.Data:GetAnnouncementInfo()
    if TableIsEmpty(self.config) then
        ViewMgr:CloseView(AnnouncementView)
        return
    end
    self.now_page = 1
    self.max_page = table.nums(self.config)
    self:FlushShow()
end

function AnnouncementPanel:FlushShow()
    local now_show = self.config[self.now_page]
    if now_show then
        UH.SetText(self.TxtTitle, now_show.title)
        self.BigContentList:SetData(now_show.content)
    end
    self:FlushLRArrow()
end

function AnnouncementPanel:FlushLRArrow()
    self.ObjLArrow:SetActive(self.now_page ~= 1)
    self.ObjRArrow:SetActive(self.now_page ~= self.max_page)
end


----------------------------AnnouncementItem----------------------------
AnnouncementItem = AnnouncementItem or DeclareMono("AnnouncementItem", UIWidgetBaseItem)
function AnnouncementItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtTitle, data.title)
    UH.SetText(self.TxtContent, data.content)
end


----------------------------AnnouncementItem2----------------------------
AnnouncementItem2 = AnnouncementItem2 or DeclareMono("AnnouncementItem2", UIWidgetBaseItem)
function AnnouncementItem2:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtContent, data.content)
end