LandsBetweenAltarInfoView = LandsBetweenAltarInfoView or DeclareView("LandsBetweenAltarInfoView", "lands_between/lands_between_altar_info",Mod.LandsBetween.AlterInfo)
function LandsBetweenAltarInfoView:LandsBetweenAltarInfoView()

end

function LandsBetweenAltarInfoView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenAltarInfoView)
end

function LandsBetweenAltarInfoView:LoadCallback(param_t)
    self.param_t = param_t
end

function LandsBetweenAltarInfoView:OpenCallback()
    --  LogError("?fisj!",self.param_t)
    if self.param_t.click_type == 1 then 
        self.Panel.InfoClick:Trigger()
        self.Panel.InfoClickSelect:SetActive(true)
    else 
        LandsBetweenCtrl.Instance:ReqCheckEssenceInfo()
        self.Panel.RewardClick:Trigger()
        self.Panel.RewardClickSelect:SetActive(true)
    end  
end 

LandsBetweenAltarInfoPanel = LandsBetweenAltarInfoPanel or DeclareMono("LandsBetweenAltarInfoPanel", UIWFlushPanel)
function LandsBetweenAltarInfoPanel:LandsBetweenAltarInfoPanel()
    self.data = LandsBetweenData.Instance

    self.data_cares = {
        {data = self.data.base_info, func = self.FlushPanel},
        {data = self.data.brief_info, func = self.FlushRewardInfo,init_call = false},
        {data = self.data.net_alter_info, func = self.FlushAlterInfo,init_call = false},
    }

    self.main_list = {}

    self.init = false
    self.select_type = -1
end 

function LandsBetweenAltarInfoPanel:Awake()
    UIWFlushPanel.Awake(self)
    
end 

function LandsBetweenAltarInfoPanel:FlushPanel()
    -- self.info_click:Trigger()

    self:FlushAlterInfo()
    self:FlushRewardInfo()
end

function LandsBetweenAltarInfoPanel:FlushAlterInfo()
    self.main_list = self.data:GetAlterInfoList()

    if self.select_type == -1 then 
        self.alter_list:SetData(self.main_list)
    else 
        local fix_list = {}
        for k,v in pairs(self.main_list) do
            if v.level == self.select_type then 
                table.insert(fix_list,v)
            end
        end 

        self.alter_list:SetData(fix_list)
        
    end 

    self.AltarEmpty:SetActive(#self.main_list == 0)

    self.btn_list:SetData(LandsBetweenAlterTag)
    TimeHelper:AddDelayFrameTimer( 
        function ()
            if self.InfoPart ~= nil and self.InfoPart.activeSelf then 
                if not self.init then 
                    self.btn_list:ClickItem(-1)
                    self.init = true
                end         
            end 
        end,10)
    
end

function LandsBetweenAltarInfoPanel:OnClickCancelAlter(data)
    local role_id = RoleData.Instance:GetRoleId()
    if data.net_info.occupy_uid ~= role_id then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.AlterInfoCancelTips)    
        return 
    end 

    local info = {
		content =Language.LandsBetween.Desc.CancelAltar, 
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = function()
                local fix_link = self.data.FixLink(data.link_mark)
                LandsBetweenCtrl:OperateCancelAltar({param_1 = fix_link[1],param_2 = fix_link[2]})
                PublicPopupCtrl.Instance:CloseDialogTips()
            end,
		}
	}

	PublicPopupCtrl.Instance:DialogTips(info)
end 

function LandsBetweenAltarInfoPanel:FlushRewardInfo()
    
    local reward_list = self.data:GetAlterInfoRewardList()
    self.reward_list:SetData(reward_list)

    self.RewardEmpty:SetActive(#reward_list == 0)
end

function LandsBetweenAltarInfoPanel:OnClickAllReward()
    -- PublicPopupCtrl.Instance:Center("全部领取")

    LandsBetweenCtrl.Instance:ReqFetchEssenceReward()
end 

function LandsBetweenAltarInfoPanel:OnClickReward()
    UH.SetText(self.main_title,Language.LandsBetween.Title.AlterReward)
    LandsBetweenCtrl.Instance:ReqCheckEssenceInfo()
end

function LandsBetweenAltarInfoPanel:OnClickInfo()
    UH.SetText(self.main_title,Language.LandsBetween.Title.AlterInfo)

end

function LandsBetweenAltarInfoPanel:ClickAlterTag(data)
    self.select_type = data.index

    self:FlushAlterInfo()
    -- PublicPopupCtrl.Instance:Center(data.index)
end

LandsBetweenAltarRewardCell = LandsBetweenAltarRewardCell or DeclareMono("LandsBetweenAltarRewardCell", UIWidgetBaseItem)
function LandsBetweenAltarRewardCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    local reward = Item.Create(data)
    self.calls:SetData(reward)
    UH.SetText(self.name,reward:Name())
end

LandsBetweenAltarTagCell = LandsBetweenAltarTagCell or DeclareMono("LandsBetweenAltarTagCell", UIWidgetBaseItem)
function LandsBetweenAltarTagCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    if data.index == -1 then 
        UH.SetText(self.name,Language.LandsBetween.Desc.TagAll)
    else
        UH.SetText(self.name, DataHelper.GetDaXie(data.index))
    end 
end

function LandsBetweenAltarTagCell:Click()
    self.ItemClick:Trigger()
end

LandsBetweenAltarInfoCell = LandsBetweenAltarInfoCell or DeclareMono("LandsBetweenAltarInfoCell", UIWidgetBaseItem)
function LandsBetweenAltarInfoCell:LandsBetweenAltarInfoCell()
    self.time:SetCallBack(function()self:TimerComplete() end)
end 
function LandsBetweenAltarInfoCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    -- 名字
    local grid_name = string.format(Language.LandsBetween.Desc.Alter,self.data.level) 

    UH.SpriteName(self.icon, "ShenTan"..self.data.level)
    UH.SetText(self.name,grid_name)

    RichTextHelper.Bind(self.pos, BindTool.Bind(self.OnClickPos, self))

    local fix_link = LandsBetweenData.FixLink(data.link_mark)
	local str = "("..fix_link[1]..","..fix_link[2]..")" 
    str = RichTextHelper.LinkStr(ColorStr(str,COLORSTR.Green9), "_ul", COLORSTR.Green9)
    self.ready_pos = {x = tonumber(fix_link[1]),y = tonumber(fix_link[2])}
    UH.SetText(self.pos,str)
 
    local is_me = LandsBetweenData.Instance:IsMyRole(data.net_info.prayer_uid,data.net_info.prayer_plat_type)
    local is_irony =  data.net_info.pray_end_timestamp ~= nil and data.net_info.pray_end_timestamp > 0

    self.mask:SetActive(is_irony)
    self.irony:SetActive(not is_me and is_irony)
    self.my_pos:SetActive(is_me and is_irony)

    local mask_time = data.net_info.pray_end_timestamp or 0
    if mask_time > 0 then
        self.time:StampTime(mask_time,TimeType.Type_Special_11,Language.LandsBetween.Desc.AlterInfoTimer)
    end 
end

function LandsBetweenAltarInfoCell:TimerComplete()
    if not ViewMgr:IsOpen(LandsBetweenAltarInfoView) then
        return 
    end 
    self.mask:SetActive(false)
end 

function LandsBetweenAltarInfoCell:OnClickPos()
    LandsBetweenData.Instance:SetMapClick({x = self.ready_pos.x,y = self.ready_pos.y})
                
    self.closetimer = TimeHelper:AddDelayTimer(function()
        ViewMgr:CloseView(LandsBetweenAltarInfoView)
    end,0.3)
end