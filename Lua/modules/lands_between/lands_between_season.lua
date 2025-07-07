LandsBetweenSeasonView = LandsBetweenSeasonView or DeclareView("LandsBetweenSeasonView", "lands_between/lands_between_season",Mod.LandsBetween.Season)
function LandsBetweenSeasonView:LandsBetweenSeasonView()

end

function LandsBetweenSeasonView:LoadCallback()
    local oper_index = LandsBetweenData.Instance:GetCurMapType() == 0 and 1 or 2
    self.Panel.Triggers[oper_index]:Trigger()
end

function LandsBetweenSeasonView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenSeasonView)
end

LandsBetweenSeasonPanel = LandsBetweenSeasonPanel or DeclareMono("LandsBetweenSeasonPanel", UIWFlushPanel)
function LandsBetweenSeasonPanel:LandsBetweenSeasonPanel()
    self.data = LandsBetweenData.Instance
    self.data_cares = {
        {data = self.data.phasetask_info, func = self.FlushPanel},
    }
    self.select = 1
    self.param = nil

    -- 这个是静态的
end 

function LandsBetweenSeasonPanel:FlushPanel()
    if self.param == nil then 
       self.param = self.data:GetSeasonParam()
       self.select = self.param.cur_status
    end 

    UH.SetText(self.Status, self.param.cur_status == self.select and Language.LandsBetween.StatusType[1] 
        or (self.param.cur_status > self.select and Language.LandsBetween.StatusType[2] or Language.LandsBetween.StatusType[3]) )

    UH.SetText(self.Time,self.param.time_show)

    local t_p = self.param.season_cfg[self.select]
    for i = 1,2 do 
        self.TaskCells[i]:SetData(t_p.task_list[i])
    end 
    self.Rewards:SetData(t_p.reward_list)

    UH.SpriteName(self.Area, self.select == 1 and "_Loc_ShenYu4_0001_WaiYu" or "_Loc_ShenYu4_0002_NeiYu")

    self.BtnCatchInter.Interactable = (t_p.task_list[1].task_flag == 1 and t_p.task_list[2].task_flag == 1) and 
    t_p.reward_flag == 0
end

function LandsBetweenSeasonPanel:OnClickCatch()
    local t_p = self.param.season_cfg[self.select]
    if t_p.task_list[1].task_flag == 0 or t_p.task_list[2].task_flag == 0 then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.FetchShouldCompleteTask)
        return 
    end 

    if t_p.reward_flag == 1 then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.FetchedError)
        return 
    end 

    LandsBetweenCtrl.Instance:ReqFetchPhaseReward({index = self.select - 1})
end

function LandsBetweenSeasonPanel:OnClickTime(index)
    self.select = index

    self:FlushPanel()
end

LandsBetweenSeasonTaskCell = LandsBetweenSeasonTaskCell or DeclareMono("LandsBetweenSeasonTaskCell", UIWidgetBaseItem)
function LandsBetweenSeasonTaskCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.TaskDesc,data.task_desc..string.format("(%s/%s)",data.task_progress,data.task_total))
    self.UnDone:SetActive(data.task_flag == 0 and data.open == "")
    self.Done:SetActive(data.task_flag == 1)
    self.BtnGo:SetActive(data.task_flag == 0 and data.open ~= "")
end

function LandsBetweenSeasonTaskCell:OnClickGoTo()
    ViewMgr:OpenViewByKey(self.data.open)
end