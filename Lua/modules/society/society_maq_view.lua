
SocietyMaqView = SocietyMaqView or DeclareView("SocietyMaqView", SocietyConfig.ResPath .. "society_maq")

function SocietyMaqView:OnClickClose()
    ViewMgr:CloseView(SocietyMaqView)
    SocietyCtrl.Instance:SendShiTuSeekingPreference()
end

function SocietyMaqView:CloseCallback()
    ViewMgr:OpenView(SocietyShiTuGameView)
end

SocietyMaqViewF = SocietyMaqViewF or DeclareMono("SocietyMaqViewF", UIWFlushPanel)

function SocietyMaqViewF:SocietyMaqViewF()
    self.data = SocietyCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data.maq_options, func = self.FlushPage, keys = {"cur_page"}},
    }
end

function SocietyMaqViewF:FlushPage()
    local cur_page = self.data.maq_options.cur_page
    local info = self.data.maq_list[cur_page]
    if info then
        self.Quest.text = info.question
        self.Progress.text = string.format("%s/%s", cur_page, #self.data.maq_list)
        self.OptionList:SetData(info.buttons)
    else
        self.QuestObj:SetActive(false)
        self.LastObj:SetActive(true)
    end
end

SocietyMaqViewItem = DeclareMono("SocietyMaqViewItem", UIWidgetBaseItem)
function SocietyMaqViewItem:SocietyMaqViewItem()
    self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyMaqViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Name.text = data.option
end

function SocietyMaqViewItem:OnClickItem()
    self.society_data.maq_options.cur_page = self.society_data.maq_options.cur_page + 1
    table.insert(self.society_data.maq_options.jobs, self.data.recommend_job)
end


SocietyShiTuGameView = SocietyShiTuGameView or DeclareView("SocietyShiTuGameView", SocietyConfig.ResPath .. "shitu/society_shitu_game")

function SocietyShiTuGameView:OnClickClose()
    ViewMgr:CloseView(SocietyShiTuGameView)
end
