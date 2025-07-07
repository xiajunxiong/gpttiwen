BigDipperRankView = BigDipperRankView or DeclareView("BigDipperRankView", "big_dipper/big_dipper_rank")
function BigDipperRankView:BigDipperRankView()
	self.AudioOpen = CommonAudio.ViewOpenL2
    self.AudioClose = CommonAudio.ViewCloseL1
end

function BigDipperRankView:OnClickClose()
    ViewMgr:CloseView(BigDipperRankView)
end

function BigDipperRankView:LoadCallback()
	self:SetTabName()
    SocietyCtrl.Instance:SendCrossGetPersonAllRankList(24)
end

function BigDipperRankView:SetTabName()
	local list = BigDipperData.Instance:GetRankBossNameList()	
	for k, v in pairs(list) do
		UH.SetText(self.BossNames[k],v)
	end
end

function BigDipperRankView:OnClickType(type)
    SocietyCtrl.Instance:SendCrossGetPersonAllRankList(24 + type)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
end

BigDipperRankPanel = BigDipperRankPanel or DeclareMono("BigDipperRankPanel", UIWFlushPanel)
function BigDipperRankPanel:BigDipperRankPanel()
    self.data = BigDipperData.Instance
    self.data_cares = {
        {data = self.data.rank_data, func = self.onFlush ,keys = {"flush_num"},init_call = false},
    }

end

function BigDipperRankPanel:onFlush()
    local list = self.data.rank_data.rank_infos
	if #list > 0 then
		list = self:SortRankList(list)
	end
    self.List:SetData(list)
    self.NotHasTip:SetActive(#list == 0 and true or false)
end

function BigDipperRankPanel:SortRankList(list)
	local rank_value_begin = -1
	local value_list = {}
	for k, v in pairs(list) do
		if v.rank_value ~= rank_value_begin then
			table.insert(value_list,v.rank_value)
			rank_value_begin = v.rank_value
		end
	end
	table.sort(value_list,function(a,b)return a < b end)
	for k, v in pairs(value_list) do
		for k1, v1 in pairs(list) do
			if v1.rank_value == v then
				v1.rank = k
			end
		end
	end
	table.sort(list,function(a,b)return a.rank_value < b.rank_value end)
	return list
end

----------------------------BigDipperRankItem----------------------------
BigDipperRankItem = BigDipperRankItem or DeclareMono("BigDipperRankItem", UIWidgetBaseItem)

function BigDipperRankItem:SetData(data)
	local rank_top = true
    if data.rank > 3 then
        UH.SetText(self.RankNum,data.rank)
		rank_top = false
    else
        UH.SpriteName(self.ImgNum,"RankNum"..data.rank)
    end
	
	self.RankTop:SetActive(rank_top)
    self.RankOther:SetActive(not rank_top)
    UH.SetText(self.Name,data.role.name)
    UH.SetText(self.Round,data.rank_value)
	
	local role_id = RoleData.Instance:GetRoleId()
	if data.role.uid == role_id then
		UH.SpriteName(self.RankBg,"ranktiao2")
	else
		UH.SpriteName(self.RankBg,"ranktiao1")
	end
    UIWidgetBaseItem.SetData(self, data)
end

function BigDipperRankItem:OnClickItem()
    RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.role.uid)
    RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.role.uid)
end