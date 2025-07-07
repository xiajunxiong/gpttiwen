LandsBetweenSingleAltarView = LandsBetweenSingleAltarView or DeclareView("LandsBetweenSingleAltarView", "lands_between/lands_between_single_altar")
function LandsBetweenSingleAltarView:LandsBetweenSingleAltarView()

end

function LandsBetweenSingleAltarView:LoadCallback(param_t)
    if param_t == nil then 
        return 
    end 
    
    UH.SetText(self.city_name,param_t.city_name)
    self.panel_irony:SetObjActive(param_t.holded)
    self.panel_attack:SetObjActive(not param_t.holded)
end

function LandsBetweenSingleAltarView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenSingleAltarView)
end

function LandsBetweenSingleAltarView:CloseCallback()
    LandsBetweenData.Instance:ClearMapClick()
end

LandsBetweenSingleAltarPanel = LandsBetweenSingleAltarPanel or DeclareMono("LandsBetweenSingleAltarPanel", UIWFlushPanel)
function LandsBetweenSingleAltarPanel:LandsBetweenSingleAltarPanel()

end 

LandsBetweenSingleAltarAttackPage = LandsBetweenSingleAltarAttackPage or DeclareMono("LandsBetweenSingleAltarAttackPage", UIWFlushPanel)
function LandsBetweenSingleAltarAttackPage:LandsBetweenSingleAltarAttackPage()
    self.data = LandsBetweenData.Instance
    self.data_cares = {
        {data = self.data.base_info, func = self.FlushPanel},
    }

    -- self.mod_show	 
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.mod_show:SetShowData({ui_obj = self.ui_obj,view = self})
    end

    if self.ui_role == nil then
        self.ui_role = UIChDrawer.New()
        self.ModChShow:SetShowData({ui_obj = self.ui_role,view = self})
    end

    self.select_p = -1

    ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE)
end 

function LandsBetweenSingleAltarAttackPage:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil

    if self.ui_role ~= nil then
		self.ui_role:DeleteMe()
	end
	self.ui_role = nil
end

function LandsBetweenSingleAltarAttackPage:FlushPanel()
    local param = self.data:GetAlterAttackInfo()

    self.mod_show_obj:SetActive(param.is_monster)
    self.ModChShowObj:SetActive(not param.is_monster)
    if param.is_monster then 
        self.ui_obj:SetData(param.obj_path)--DrawerHelper.GetNpcPath())
    else 
        self.ui_role:SetData(param.role_app)
    end 

    -- UH.SetIcon(self.partner_show, param.icon_sp, ICON_TYPE.ITEM)
    self.progress:SetProgress(param.progress_rate) -- 服务器要追补守军血量！放弃 不 做 了
    self.progress:SetText(param.progress_num) -- 服务器要追补守军血量！放弃 不 做 了
    UH.SetText(self.single_point,Language.LandsBetween.Desc.SinglePoint..param.single_point) --精华获取算法！
    UH.SetText(self.group_point,Language.LandsBetween.Desc.GroupPoint..param.group_point) --精华获取算法！
    UH.SetText(self.diffcult,param.diffcult) 
    UH.SetText(self.emeny_score,param.emeny_score) 
    self.reward_list:SetData(param.reward_list) 
    self.irony_type = param.irony_type

    local qua_str = ColorStr(Language.Partner.ColorLevel[param.qua_tips],PartnerQualityColor[param.qua_tips])
    if self.irony_type == 2 then 
        qua_str = ColorStr(Language.Pet.ColorLevelT[param.qua_tips],PartnerQualityColor[param.qua_tips])
    end 

    UH.SetText(self.reward_tips,string.format(Language.LandsBetween.Desc.IronyRewardTips,qua_str,
        Language.LandsBetween.Desc.IronyP[param.irony_type]))

    UH.SetIcon(self.irony_show,param.show_icon)
    self.IronyPetQua.gameObject:SetActive(self.irony_type == 2)
    self.IronyQua.gameObject:SetActive(self.irony_type == 1)
    if self.irony_type == 2 then 
        UH.SpriteName(self.IronyPetQua, "pet_qua_"..(param.qua_tips))
    else 

        PartnerInfo.SetQuality(self.IronyQua, param.qua_tips)
    end 
    
    -- UH.SetIcon(self.IronyQua,)
    UH.SetIcon(self.RewardIcon, Item.GetIconId(CommonItemId.GodSpaceEssence))
end

function LandsBetweenSingleAltarAttackPage:OnClickShare()
    -- PublicPopupCtrl.Instance:Center("进行分享")

    local str = self.data:GetShareStr()
    ChatCtrl.Instance:SendChannelChatText(str,nil, ChatConfig.ChannelType.landsbetween)
end

function LandsBetweenSingleAltarAttackPage:OnClickAttack()
    -- PublicPopupCtrl.Instance:Center("开始进攻")
    local map_pic = self.data:GetMapPicClick(true)
    if #self.data:GetMyNetAlter() >= GodPosData.Instance:GodPosCfg().occupy then
        ViewMgr:CloseView(LandsBetweenSingleAltarView)
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.limit_tip)
    elseif (self.data:GetCurMapType() == 1 and map_pic.linking_city ) or (map_pic.adjoining_grid) then 
        -- local fix_link = self.data.FixLink(map_pic.static.link_mark)
        -- LandsBetweenCtrl.Instance:OperateOccupy({param_1 = fix_link[1],param_2 = fix_link[2],param_3 = 1})
        -- ViewMgr:CloseView(LandsBetweenSingleAltarView)
        -- ViewMgr:CloseView(LandsBetweenMainView)

        self:OnClickBuZhen()
    else 
        if not map_pic.adjoining_grid then
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.AdjoinError)
        else 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.LinkError)
        end
    end 
end

-- 这个按钮点击无效果，留存接口防止背刺
function LandsBetweenSingleAltarAttackPage:OnClickTips()
    -- PublicPopupCtrl.Instance:Center("提示")
end

function LandsBetweenSingleAltarAttackPage:OnClickBuZhen()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE)
end

LandsBetweenSingleAltarRewardItem = LandsBetweenSingleAltarRewardItem or DeclareMono("LandsBetweenSingleAltarRewardItem",UIWidgetBaseItem)
function LandsBetweenSingleAltarRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.calls:SetData(data)
end 

LandsBetweenSingleAltarIronyPage = LandsBetweenSingleAltarIronyPage or DeclareMono("LandsBetweenSingleAltarIronyPage", UIWFlushPanel)
function LandsBetweenSingleAltarIronyPage:LandsBetweenSingleAltarIronyPage()
    self.data = LandsBetweenData.Instance
    self.data_cares = {
        {data = self.data.my_info, func = self.FlushPanel},
        {data = self.data.net_alter_info, func = self.FlushPanel},
    }

end 

function LandsBetweenSingleAltarIronyPage:FlushPanel()
    local param = self.data:GetAlterIronyInfo() 

    self.irony_list:SetData(param.irony_list)

    local item_str = Language.LandsBetween.Desc.RateNum
    for k,v in pairs(param.reward_list) do 
        item_str = item_str .. v:QuaName(true).. (k == #param.reward_list and "" or ",")
    end 
    UH.SetText(self.rate_produce,item_str)
    UH.SetText(self.ponit_produce,Language.LandsBetween.Desc.PointNum..param.single_point..Language.LandsBetween.Desc.PointFromSingle..","..param.group_point..Language.LandsBetween.Desc.PointFromGroup)
    UH.SetText(self.cost_tips,string.format(Language.LandsBetween.Desc.MovingCost,param.cost_num))

    self.cost_tips.gameObject:SetActive(param.should_start)
    self.BtnStart:SetActive(param.should_start)
    self.BtnStop:SetActive(not param.should_start)
    self.irony_type = param.irony_type

    local qua_str = ColorStr(Language.Partner.ColorLevel[param.qua_tips],PartnerQualityDeepColor[param.qua_tips])
    if self.irony_type == 2 then 
        qua_str = ColorStr(Language.Pet.ColorLevelT[param.qua_tips],PartnerQualityDeepColor[param.qua_tips])
    end 

    self.irony_tips_index = param.irony_tips
    UH.SetText(self.plus_tips,string.format(Language.LandsBetween.Desc.IronyRewardTipsDeep,qua_str,
        Language.LandsBetween.Desc.IronyP[self.irony_type]))

    self.select_p = self.irony_type == 2 and -1 or 0
end

function LandsBetweenSingleAltarIronyPage:OnClickIrony()
    -- PublicPopupCtrl.Instance:Center("进行祈祷")
    -- if self.data.my_info.action_point < Config.divine_domain_system_auto.action_force[1].pray_ap_consume then 
    --     PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.MovingIsLack)
    --     ViewMgr:OpenView(LandsBetweenMovingView)         
    --     return
    -- end 

    local map_pic = self.data:GetMapPicClick(true)

    if (self.data:GetCurMapType() == 1 and map_pic.linking_city ) or (map_pic.adjoining_grid) then 
        local fix_link = self.data.FixLink(map_pic.static.link_mark)

        if self.irony_type == 1 then 
            LandsBetweenCtrl:OperatePray({param_1 = fix_link[1],param_2 = fix_link[2],param_3 = self.select_p})
        else 
            LandsBetweenCtrl:OperatePray({param_1 = fix_link[1],param_2 = fix_link[2],param_4 = self.select_p})
        end
    else 
        if not map_pic.adjoining_grid then
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.AdjoinError)
        else 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.LinkError)
        end
    end 
end

function LandsBetweenSingleAltarIronyPage:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[self.irony_tips_index].desc})
end 

function LandsBetweenSingleAltarIronyPage:OnClickCancelAlter()
    -- PublicPopupCtrl.Instance:Center("停止祈祷")
    local map_pic = self.data:GetMapPicClick()
    local fix_link = self.data.FixLink(map_pic.static.link_mark)
    
    LandsBetweenCtrl:OperateStopPray({param_1 = fix_link[1],param_2 = fix_link[2]})
end 

function LandsBetweenSingleAltarIronyPage:OnClickEffCell(data)
    -- PublicPopupCtrl.Instance:Center("已选中伙伴 ".. data.info.name)

    if self.irony_type == 1 then
        self.select_p = data.info.partner_id
    else 
        self.select_p = data.info.index
    end
    -- LogError("?fdickdsk",)
    -- 
end

LandsBetweenSingleAltarEffItem = LandsBetweenSingleAltarEffItem or DeclareMono("LandsBetweenSingleAltarEffItem",UIWidgetBaseItem)
function LandsBetweenSingleAltarEffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.qua_frame.gameObject:SetActive( self.panel.irony_type == 1)
    self.icon.gameObject:SetActive( self.panel.irony_type == 1)
    self.pet_head.gameObject:SetActive( self.panel.irony_type == 2)

    if self.panel.irony_type == 1 then 
        local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
        PartnerInfo.SetQuality(self.qua_frame, info_quality.quality)
        UH.SetText(self.name,ColorStr(data.info.name,PartnerQualityDeepColor[info_quality.quality]))
        UH.SetIcon(self.icon, PartnerData.IconId(data), ICON_TYPE.ITEM)
    else
        self.pet_head:SetDataByPet(data)
        UH.SetText(self.name,data:QuaName(true))
    end 
    
end 
