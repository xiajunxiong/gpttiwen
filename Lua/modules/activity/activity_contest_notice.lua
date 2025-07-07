-- ================ 天下第一比武大会预告 =================
ContestMeetingNotice = ContestMeetingNotice or DeclareView("ContestMeetingNotice", "arena_contest/contest_meeting_notice")
function ContestMeetingNotice:ContestMeetingNotice()
    self:InitData()
end

function ContestMeetingNotice:LoadCallback()
    UH.SetText(self.Desc, Config.language_cfg_auto.textdesc[209].desc)
end

function ContestMeetingNotice:OnToggleClick(desc_type)
    self:OpenPanel(desc_type)
end

function ContestMeetingNotice:OpenPanel(desc_type)
    self.Content:SetActive(true)
    local list_data = self.desc_cfg[desc_type] or {}
    if desc_type == 1 then
        for i = 1, self.TipList:Length() do
            local data = list_data[i]
            self.TipList[i]:SetObjActive(data ~= nil)
            if data then
                self.TipList[i]:SetData(data)
            end
        end
    else
        for i = 1, self.TipList:Length() do
            self.TipList[i]:SetObjActive(false)
        end
        local data = list_data[1]
        if data then
            self.TipList[2]:SetObjActive(true)
            self.TipList[2]:SetData(data)
        end
    end
    local title = Language.ArenaContestNotice.DescTitle[desc_type] or ""
    UH.SetText(self.Title, title)
end

function ContestMeetingNotice:ClosePanel()
    self.Content:SetActive(false)
end

function ContestMeetingNotice:InitData()
    local desc_cfg = {}
    local cfg = Config.daily_activity_auto.match_notice
    for i,v in pairs(cfg) do
        if v.seq == 1 then
            if not desc_cfg[v.type] then
                desc_cfg[v.type] = {}
            end
            table.insert(desc_cfg[v.type], v)
        end
    end
    self.desc_cfg = desc_cfg
end

function ContestMeetingNotice:OnCloseClick()
    ViewMgr:CloseView(ContestMeetingNotice)
end



-- ================ 英雄会武 =================
ContestHeroNotice = ContestHeroNotice or DeclareView("ContestHeroNotice", "arena_contest/contest_hero_notice")
function ContestHeroNotice:ContestHeroNotice()
    self:InitData()
end

function ContestHeroNotice:LoadCallback()
    UH.SetText(self.Desc, Config.language_cfg_auto.textdesc[208].desc)
end

function ContestHeroNotice:InitData()
    local desc_cfg = {}
    local cfg = Config.daily_activity_auto.match_notice
    for i,v in pairs(cfg) do
        if v.seq == 0 then
            if not desc_cfg[v.type] then
                desc_cfg[v.type] = {}
            end
            table.insert(desc_cfg[v.type], v)
        end
    end
    self.desc_cfg = desc_cfg
end

function ContestHeroNotice:OnToggleClick(desc_type)
    self:OpenPanel(desc_type)
end

function ContestHeroNotice:OpenPanel(desc_type)
    self.Content:SetActive(true)
    local list_data = self.desc_cfg[desc_type] or {}
    for i = 1, self.TipList:Length() do
        local data = list_data[i]
        self.TipList[i]:SetObjActive(data ~= nil)
        if data then
            self.TipList[i]:SetData(data)
        end
    end

    local title = Language.ArenaContestNotice.DescTitle[desc_type] or ""
    UH.SetText(self.Title, title)
end

function ContestHeroNotice:ClosePanel()
    self.Content:SetActive(false)
end

function ContestHeroNotice:OnCloseClick()
    ViewMgr:CloseView(ContestHeroNotice)
end



ContestMeetingNoticeTipItem = ContestMeetingNoticeTipItem or DeclareMono("ContestMeetingNoticeTipItem", UIWidgetBaseItem)
function ContestMeetingNoticeTipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, data.name)
    local desc_list = Split(tostring(data.language_seq), "|") or {}
    UH.SetText(self.Desc, Config.language_cfg_auto.textdesc[tonumber(desc_list[1])].desc)
    if self.Desc2 and desc_list[2] then
        UH.SetText(self.Desc2, Config.language_cfg_auto.textdesc[tonumber(desc_list[2])].desc)
    end
    local h = self.SelfH + self.Desc.preferredHeight
    self.Tansform:SetSizeWithCurrentAnchors(1,h)
end