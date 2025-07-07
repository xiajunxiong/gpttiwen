
LunJianRankPanel = LunJianRankPanel or DeclareMono("LunJianRankPanel", UIWFlushPanel)
--MyScoreTxt========Text
--MyRankTxt=========Text
--RankList==========UIWidgetList[LunJianRankItem]
--GroupList=========UIWidgetList[LunJianRankGroupItem]
--GroupListObj======GameObject
--SeledGroupName====Text

function LunJianRankPanel:LunJianRankPanel()
    self.group_list_need_flush = true
    local my_group_id = LunJianData.Instance:MyGroupId()
    LunJianData.Instance:SetRankSeledGroup((my_group_id ~= nil and my_group_id > 0) and my_group_id or 1)
    UH.SetText(self.MyScoreTxt,tostring(LunJianData.Instance:MyScore()))
	self.data_cares = {
		{data = LunJianData.Instance:ViewData(), func = self.OnSeledGroupChange, keys = {"rank_seled_group"}},
        {data = LunJianData.Instance:ViewData(), func = self.FlushRankList, keys = {"rank_seled_group"},init_call = false},
        {data = LunJianData.Instance:RankData(), func = self.FlushRankList},
        {data = LunJianData.Instance:MyInfo(), func = self.FlushMyRank, keys = {"rank"}},
    }
end

function LunJianRankPanel:FlushMyRank()
    local rank = LunJianData.Instance:MyRank()
    UH.SetText(self.MyRankTxt,tostring(rank <= 0 and "--" or rank))
end

function LunJianRankPanel:OnClickGroupSel()
    local group_obj_act = self.GroupListObj.activeSelf
    if group_obj_act == false and self.group_list_need_flush == true then     --初始化组列表
        self.group_list_need_flush = false
        self.GroupList:SetData(LunJianData.Instance:AllGroupId())
    end
    self.GroupListObj:SetActive(not group_obj_act)
end

function LunJianRankPanel:OnGroupParClick()
    self.GroupListObj:SetActive(false)
end

function LunJianRankPanel:OnSeledGroupChange()
    self.group_list_need_flush = true
    self.GroupListObj:SetActive(false)
    UH.SetText(self.SeledGroupName,LunJianData.GroupIdToName(LunJianData.Instance:RankSeledGroup()))
end

function LunJianRankPanel:FlushRankList()
    local cur_seld_group = LunJianData.Instance:RankSeledGroup()
    local rank_data = LunJianData.Instance:GetGroupRankList(cur_seld_group)
    if rank_data == nil then
        LunJianCtrl.Instance:ReqRankList(cur_seld_group)
        return
    end
    self.RankList:SetData(rank_data)
end

--------------------------------------------------------------------------------
LunJianRankItem = LunJianRankItem or DeclareMono("LunJianRankItem", UIWidgetBaseItem)
--RankTxt=========Text
--Name============Text
--WinNum==========Text
--Score===========Text
--RankImg=========UIImageSpriteSet
function LunJianRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    -- LogError("LunJidanRankItem=====",data)
    local show_rank_img = data.total_num > 0 and data.rank <= 3
    self.RankTxt:SetObjActive(not show_rank_img)
    self.RankImg:SetObjActive(show_rank_img)

    if data.total_num <= 0 then
        UH.SetText(self.RankTxt,"--")
    else
        if show_rank_img then
            UH.SpriteName(self.RankImg,LunJianMainView.RankSpriteName(data.rank))
        else
            UH.SetText(self.RankTxt,tostring(data.rank))
        end
    end
    UH.SetText(self.Name,data.name)
    UH.SetText(self.WinNum,string.format(Language.LunJian.Rank.WinLoseNum,data.win_num,data.draw_num,data.lose_num))
    UH.SetText(self.Score,data.score)
end

function LunJianRankItem:OnClickCheck()
    if self.data.plat_type == 0 then    --机器人
        PublicPopupCtrl.Instance:Center(Language.ItemInfo.RoleInfoDesc.RobotInfoHint)
    else
        RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.uid)
        RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.uid)
    end
end


-----------------------------------------------------------------------------------
LunJianRankGroupItem = LunJianRankGroupItem or DeclareMono("LunJianRankGroupItem", UIWidgetBaseItem)
---Name=========Text
---Desc1========UIImageSpriteSet
---Desc2========UIImageSpriteSet

function LunJianRankGroupItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name,LunJianData.GroupIdToName(data))
    local is_self_seled = data == LunJianData.Instance:RankSeledGroup()
    UH.Color(self.Name,is_self_seled and COLORS.Yellow1 or COLORS.Yellow7)
    local desc_sp = is_self_seled and "XiaoZhuangShi2" or "XiaoZhuangShi1"
    UH.SpriteName(self.Desc1,desc_sp)
    UH.SpriteName(self.Desc2,desc_sp)
end

function LunJianRankGroupItem:OnClickGroupItem()
    LunJianData.Instance:SetRankSeledGroup(self.data)
end

