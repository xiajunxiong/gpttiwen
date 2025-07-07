GoldrMinecRareSelectView = GoldrMinecRareSelectView or DeclareView("GoldrMinecRareSelectView", "serveractivity/goldr_minec/goldr_minec_select")
-- VIEW_DECLARE_LEVEL(GoldrMinecRareSelectView, ViewLevel.L0)
function GoldrMinecRareSelectView:GoldrMinecRareSelectView()
	-- self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function GoldrMinecRareSelectView:LoadCallback(param_t)
    -- GoldRMinecCtrl.Instance:SendInfo()
    self.data = GoldRMinecData.Instance

    self.list:SetData(self.data:GetSelectRewardList())
    self.send_info = {}

    self:FlushText()
end

function GoldrMinecRareSelectView:CloseCallback()
    
end

function GoldrMinecRareSelectView:OnClickClose()
    ViewMgr:CloseView(GoldrMinecRareSelectView)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function GoldrMinecRareSelectView:OnSelect()
    local data = self.list:GetSelectedData()
    self.send_info = {}
    for k,v in pairs(data) do
        table.insert(self.send_info,v)
    end 

    self:FlushText()
end

function GoldrMinecRareSelectView:FlushText()
    UH.SetText( self.select_num, string.format(Language.GoldRMinec.SelectTips,#self.send_info))
end

function GoldrMinecRareSelectView:OnClickSure()
    if #self.send_info < 4 then 
        PublicPopupCtrl.Instance:Center(Language.GoldRMinec.SelectErrorLack)
        return 
    end 

    for i = 1,4 do 
        GoldRMinecCtrl.Instance:SendSetReward(i-1,self.send_info[i].seq)
    end 

    self:OnClickClose()

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end


GoldrMinecRareSelectCell = GoldrMinecRareSelectCell or DeclareMono("GoldrMinecRareSelectCell", UIWidgetBaseItem)
function GoldrMinecRareSelectCell:GoldrMinecRareSelectCell()
    self.gm_data = GoldRMinecData.Instance
    self.is_long_click = false
end
function GoldrMinecRareSelectCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.call:SetData(Item.Create(data.reward_item))
end 

function GoldrMinecRareSelectCell:ShowSelect(flag)
    self.select:SetActive(flag)
end

function GoldrMinecRareSelectCell:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end 

function GoldrMinecRareSelectCell:OnPressDown()
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 1 then
                self.is_long_click = true
                CellClicks.BagGridNoButton(self.call)
            end
        end,
        0.2
    )
end 

function GoldrMinecRareSelectCell:OnSelect(selected)
    if not self.is_long_click then
        local select_data = self.master.list:GetSelectedData()
        if #select_data > 4 then 
            PublicPopupCtrl.Instance:Center(Language.GoldRMinec.SelectLimit)
            self:SetSelected(not selected)
            return 
        end 

        self:SetSelected(selected)
        -- self:ShowSelect(selected)
    else
        
        self:SetSelected(not selected)
        -- self:ShowSelect(not selected)
    end 
end 