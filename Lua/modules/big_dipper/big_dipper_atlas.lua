BigDipperAtlasView = BigDipperAtlasView or DeclareView("BigDipperAtlasView", "big_dipper/big_dipper_atlas")
function BigDipperAtlasView:BigDipperAtlasView()
	self.AudioOpen = CommonAudio.ViewOpenL2
    self.AudioClose = CommonAudio.ViewCloseL1
end

function BigDipperAtlasView:OnClickClose()
    ViewMgr:CloseView(BigDipperAtlasView)
end

function BigDipperAtlasView:LoadCallback()
end

BigDipperAtlasPanel = BigDipperAtlasPanel or DeclareMono("BigDipperAtlasPanel", UIWFlushPanel)
function BigDipperAtlasPanel:BigDipperAtlasPanel()
	self.data = BigDipperData.Instance
	self.data_cares = {
		{data = self.data.boss_list, func = self.FlushShowPanel, init_call = true}
    }
end

function BigDipperAtlasPanel:FlushShowPanel()
	local boss_list = self.data:GetAtlasBossList()
	for i = 1, 8 do
        local vo = boss_list[i]
        self.ButtomLists[i]:SetData(vo)
    end 
end

----------------------------BigDipperAtlasItem----------------------------
BigDipperAtlasItem = BigDipperAtlasItem or DeclareMono("BigDipperAtlasItem", UIWidgetBaseItem)

function BigDipperAtlasItem:SetData(data)
	self.UnlockObj:SetActive(not data.is_lock)
	self.LockObj:SetActive(data.is_lock)
	self.Handler.Interactable = not data.is_lock
	if not data.is_lock then
		UH.SetText(self.Name,BigDipperData.Instance:GetBossNameBySeq(data.info.npc_seq))
	end
    UIWidgetBaseItem.SetData(self, data)
end

function BigDipperAtlasItem:OnClickItem()
	if not self.data.is_lock then
		BigDipperData.Instance.boss_info:Set(self.data.info)
		ViewMgr:OpenView(BigDipperBossInfoView)
	end
end
