PeakAchieveView = PeakAchieveView or
                      DeclareView("PeakAchieveView",
                                  "peak_road/peak_achieve_view")

function PeakAchieveView:PeakAchieveView() end

function PeakAchieveView:LoadCallback() end

function PeakAchieveView:OnClickClose() ViewMgr:CloseView(PeakAchieveView) end

PeakAchievePanel = PeakAchievePanel or
                       DeclareMono("PeakAchievePanel", UIWFlushPanel)

function PeakAchievePanel:PeakAchievePanel()
    self.data = PeakRoadData.Instance
    self.data_cares = {
        {
            data = self.data:PeakData(),
            func = self.FlushShow,
            keys = {"achieve_flag","achieve_refresh"}
        }
    }
end

function PeakAchievePanel:FlushShow()
    self.RewardList:SetData(self.data:GetAchieveCfg())
end

PeakAchieveViewItem = DeclareMono("PeakAchieveViewItem", UIWidgetBaseItem)

function PeakAchieveViewItem:PeakAchieveViewItem()
    self.peak_data = PeakRoadData.Instance
end

function PeakAchieveViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.data.param ~= 1 then
        UH.SetText(self.Num, string.format(Language.Common.Levels, self.data.param))
    else
        UH.SetText(self.Num, "")
    end
    UH.SetText(self.Desc, self.data.achieve_des)
    self.RewardGrid:SetData(data.rewards)
    self.BtnGetInter.Interactable = self.peak_data:AchiCanGet(self.data.seq + 1, self.data.param)
	self.BtnGetInter:SetObjActive(not self.peak_data:AchiHadGet(self.data.seq + 1))
	self.GetedObj:SetActive(self.peak_data:AchiHadGet(self.data.seq + 1))
end

function PeakAchieveViewItem:OnClickGet()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    PeakRoadCtrl.CSRoleTopLevelReq(2, self.data.seq)
end
