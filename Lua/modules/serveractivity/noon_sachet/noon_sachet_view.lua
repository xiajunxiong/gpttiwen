NoonSachetView = NoonSachetView or DeclareView("NoonSachetView", "serveractivity/noon_sachet_view", Mod.DragonBoat.NoonSachet)
VIEW_DECLARE_LEVEL(NoonSachetView, ViewLevel.L0)
function NoonSachetView:NoonSachetView()
	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function NoonSachetView:LoadCallback(param_t)
    NoonSachetCtrl.Instance:SendInfo()
end

function NoonSachetView:OnClickClose()
    ViewMgr:CloseView(NoonSachetView)
end

--===========================NoonSachetPanel===========================
NoonSachetPanel = NoonSachetPanel or DeclareMono("NoonSachetPanel", UIWFlushPanel)

function NoonSachetPanel:NoonSachetPanel()
    self.data = NoonSachetData.Instance
   
    self.data_cares = {
        {data = self.data.act_info, func = self.MainFlush, keys = {"flush"},init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushBagChange},
    }

    self.act_timer:CreateActTime(ACTIVITY_TYPE.RAND_DUAN_YANG_XIANG_NANG,TimeType.Type_Special_4,Language.FlowerTouch.ActTime)

end

function NoonSachetPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)

end

function NoonSachetPanel:OnFlush()
    
end

function NoonSachetPanel:OnClickTips()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[329].desc})
end

function NoonSachetPanel:MainFlush()
    
    local list = self.data:GetMissionList()
    self.list:SetData(list)
end

function NoonSachetPanel:FlushBagChange()
    
    local stuff_data = self.data:GetStuffInfo()
    -- LogError("aa",stuff_data)
    local flag_eff = self.data:CheckIsEnough()
    for i = 1,4 do 
        local flag =  stuff_data[i] ~= nil
        self.stuff_list[i].gameObject:SetActive(flag)
        if flag then 
            self.stuff_list[i]:SetData(stuff_data[i])
        end 
    end 
    

    self.btn_eff:SetActive(flag_eff)
end

function NoonSachetPanel:OnClickItem(data)
    if not self.data:CheckTaskFinish(data) then 
        PublicPopupCtrl.Instance:Center(Language.NoonSachet.TaskNoCompleteTips)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    end 

    NoonSachetCtrl.Instance:SendReward(data.seq)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function NoonSachetPanel:OnClickSachet()
    if not self.data:CheckIsEnough() then 
        PublicPopupCtrl.Instance:Center(Language.NoonSachet.StuffLack)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return 
    end 
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)

    self.btn_eff:SetActive(false)
    self.eff_show:Play(6165174)
    self.anim:Play()
    
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.FaBaoShengJie)
end

function NoonSachetPanel:OnEffComplete()
    NoonSachetCtrl.Instance:SendSachet()
end

function NoonSachetPanel:OnClickPreView()
    self.pop_list:SetData(self.data:GetNoonPreList())
    self.pop_part:SetActive(true)
end 

function NoonSachetPanel:OnClickClosePreView() 
    self.pop_part:SetActive(false)
end 

NoonSachetMission = NoonSachetMission or DeclareMono("NoonSachetMission",UIWidgetBaseItem)
function NoonSachetMission:SetData(data)
    UIWidgetBaseItem.SetData(self, data) 

    self.reward_list:SetData(BagData.Instance:ItemListBuild(data.reward_item))
    UH.SetText(self.task_desc ,data.task_desc)
    UH.SetText(self.task_progress ,Language.NoonSachet.TaskProgress..ColorStr(data.progress.."/"..data.parameter,COLORSTR.Green9))
    
    local flag_no_jump = NoonSachetData.Instance:CheckTaskJump(data)
    local flag_complete = NoonSachetData.Instance:CheckTaskFinish(data)
    local flag_receive = NoonSachetData.Instance:CheckTaskReceive(data)

    self.btn_catch:SetActive((flag_complete or (flag_no_jump and not flag_complete)) and not flag_receive)
    self.got_mark:SetActive(flag_receive)
    UH.SetText(self.btn_text,flag_complete and Language.NoonSachet.WaitReward or Language.NoonSachet.NoComplete)
    
    self.btn_inter.Interactable = flag_complete
    
    self.btn_goto:SetActive(not flag_complete and not flag_no_jump)

    if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.red_pos)
	end

    self.red_point:SetNum((flag_complete and not flag_receive) and 1 or 0)
    self.eff_pos:SetActive(flag_complete)
end

function NoonSachetMission:OnClickJump()
    ViewMgr:CloseView(NoonSachetView)
    ViewMgr:CloseView(ActDragonBoatMainView)
    ViewMgr:OpenViewByKey(self.data.jump_interface)
end

SachetStuffCell = SachetStuffCell or DeclareMono("SachetStuffCell",UIWidgetBaseItem)
function SachetStuffCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data) 

    UH.SetIcon(self.item_icon,Item.GetIconId(data.submit_item_id))
    UH.SetText(self.item_name ,Item.GetName(data.submit_item_id))
    local num = Item.GetNum(data.submit_item_id)
    local color = num >= data.submit_num and COLORSTR.Green4 or COLORSTR.Red19
    UH.SetText(self.item_num,"("..ColorStr(Item.GetNum(data.submit_item_id),color).."/"..data.submit_num..")")
    
end

function SachetStuffCell:OnClickItem() 
    local item = Item.Init(self.data.submit_item_id)
    local info = {
		item_list = {item}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end 