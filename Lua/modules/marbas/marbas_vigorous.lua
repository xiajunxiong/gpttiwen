-- 造化装备·罡气
MarbasVigorous = MarbasVigorous or DeclareMono("MarbasVigorous", UIWFlushPanel)
function MarbasVigorous:MarbasVigorous()
    self.data = MarbasData.Instance
    self.ctrl = MarbasCtrl.Instance

    self.data_cares = {
        {data = self.data.vigorous_info,func = self.FlushListShow},
        {data = self.data.equip_list,func = self.SuitFlush, init_call = false},
        {data = self.data.vigorous_view_data,func = self.SuitFlush}
    }
end 

function MarbasVigorous:Awake() 
    UIWFlushPanel.Awake(self)
    self:SuitFlush()
end 

function MarbasVigorous:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self, "ui_obj")

    self.data:SetVigorousSelect()
end 

function MarbasVigorous:SuitFlush()
    local select_data = self.data:GetVigorousSelect()
    local flag_select = select_data ~= nil

    if not self.ui_obj then
        self.ui_obj = UIChDrawer.CopyMainRoleData()
        self.mod_show:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
            }
        )
    else 
        -- zaohua_app_id
        local Appearance = RoleData.Instance:GetApearance()
        if flag_select then 
            Appearance.zaohua_app_id = select_data.seq
        else 
            Appearance.zaohua_app_id = 0
        end 
        self.ui_obj:SetData(Appearance)
    end
    
    self.mod_name_obj:SetActive(flag_select)
    self.mod_tips_obj:SetActive(flag_select)

    if flag_select then 
        
        UH.SetText(self.mod_name,select_data.vigorous_name)
        self.mod_tips_obj:SetActive(not self.data:GetVigorousUnlock(select_data.seq))
        local level_show = MarbasConfig.LevelShow[select_data.level]
        local level_check = self.data:GetVigorousLevelCheck(level_show)
        local str = DataHelper.ConsumeNum(level_check, 8,true)
        UH.SetText(self.mod_tips,string.format(Language.Marbas.VigorousSuitTips,level_show,str))
    end 
end 

function MarbasVigorous:FlushListShow() 

    local list = self.data:GetVigorousShowList()
    self.ill_list:SetData(list)    
end

MarbasVigorousGridItems = DeclareMono("MarbasVigorousGridItems", UIWidgetBaseItem)
function MarbasVigorousGridItems:SetData(data)
	self.SuitList:SetData(data)
end

MarbasVigorousListItem = DeclareMono("MarbasVigorousListItem", UIWidgetBaseItem)
function MarbasVigorousListItem:MarbasVigorousListItem() 
    self.mb_data = MarbasData.Instance 
end

function MarbasVigorousListItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    Delete(self, "ui_obj")
end

function MarbasVigorousListItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    
    UH.SetText(self.Name, data.vigorous_name)

    self.MaskObj:SetActive(false)
    
    self.InfoObj:SetActive(true)
    self.EmptyObj:SetActive(false)

    --
    -- self.ShowImageSetter

    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.mod_show:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
            }
        )
    end
    --
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(MarbasConfig.ModLevelShow[self.data.level]))
    self:FlushEquip()
end

function MarbasVigorousListItem:FlushEquip()
    local flag_unlock = self.mb_data:GetVigorousUnlock(self.data.seq)
    local flag_weaked = self.mb_data:GetVigorousWeared(self.data.seq)
    local flag_can_unlock =  self.mb_data:GetVigorousCanUnlock(self.data.seq)

    -- LogError("my level",self.data.level,self.data.seq,flag_unlock,flag_weaked,flag_can_unlock)
    self.BtnActObj:SetActive(not flag_unlock and flag_can_unlock)

    self.BtnOffObj:SetActive(flag_unlock and flag_weaked)
    self.BtnEquipObj:SetActive(flag_unlock and not flag_weaked)

    self.TipsObj:SetActive(not flag_unlock)
    self.EquipTipsObj:SetActive(flag_unlock and flag_weaked)

    self.MaskObj:SetActive(not flag_unlock)
end

function MarbasVigorousListItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function MarbasVigorousListItem:OnClickItem(selected)
    if selected then
		UIWidgetBaseItem.SetSelected(self, true)
        self.mb_data:SetVigorousSelect(self.data)
	else
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        self.mb_data:SetVigorousCancelSelect(self.data)
	end
end

function MarbasVigorousListItem:OnClickAct()
    MarbasCtrl.Instance:SendEquipOper(MarbasReqType.ActiveApp,self.data.seq)


    UIWidgetBaseItem.SetSelected(self, true)
    self.mb_data:SetVigorousSelect(self.data)
    self.mb_data:WaitVigorousActMark(self.data.seq)
end 

function MarbasVigorousListItem:OnClickEquip()

    UIWidgetBaseItem.SetSelected(self, true)
    self.mb_data:SetVigorousSelect(self.data)
    MarbasCtrl.Instance:SendEquipOper(MarbasReqType.Wear,self.data.seq)
end 

function MarbasVigorousListItem:OnClickTakeOff()

    UIWidgetBaseItem.SetSelected(self, false)
    self.mb_data:SetVigorousCancelSelect(self.data)
    MarbasCtrl.Instance:SendEquipOper(MarbasReqType.WearOff)
end 