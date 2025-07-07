DeepDarkFantasyBoxRoll = DeepDarkFantasyBoxRoll or DeclareView("DeepDarkFantasyBoxRoll", "deep_dark_fantasy/deep_dark_fantasy_box_roll")
function DeepDarkFantasyBoxRoll:DeepDarkFantasyBoxRoll()
	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function DeepDarkFantasyBoxRoll:OnClickClose()
    ViewMgr:CloseView(DeepDarkFantasyBoxRoll)
end

function DeepDarkFantasyBoxRoll:LoadCallback()
    self.GoldSelect:Trigger()
end 

DeepDarkFantasyBoxRollF = DeepDarkFantasyBoxRollF or DeclareMono("DeepDarkFantasyBoxRollF", UIWFlushPanel)
function DeepDarkFantasyBoxRollF:DeepDarkFantasyBoxRollF()
    self.data = DeepDarkFantasyData.Instance
    self.ctrl = DeepDarkFantasyCtrl.Instance

    self.data_cares = {
        {data = self.data.miling_data, func = self.onFlush,init_call = false },
    }

    self.send_data = {
        chapter = self.data:GetCurMiLingChapter(false),
        type = 0,
        max_times = 0
    }
    self.list:SetCompleteCallBack(function()
        if self.show_eff then 
            for k,v in pairs(self.list.item_list) do 
                v:ShowEff()
            end 
            PublicPopupCtrl.Instance:Center(Language.DeepDarkFantasy.BoxFlushTips)
            self.show_eff = false
        end 
    end)
end

function DeepDarkFantasyBoxRollF:onFlush(param_t)
    local list_data = self.send_data.type == 3 and self.data:GetMiLingGoldReward() or self.data:GetMiLingSilverReward()

    local show_eff = false
    if self.init and param_t.is_select == nil then 
        show_eff = self.data:CheckIsAllNeo(list_data,self.send_data.type == 3)
    end 

    self.show_eff = show_eff
    self.list:SetData(list_data)

    self.send_data.chapter = self.data:GetCurMiLingChapter(self.send_data.type == 3)
    self.send_data.max_times = self.data:GetMiLingCurMaxRollNum(self.send_data.type == 3,self.send_data.chapter)

    local operate_icon = self.send_data.type == 3 and "_loc_gold_num" or "_loc_silver_num"
    UH.SpriteName(self.oneicon, operate_icon)
    UH.SpriteName(self.tenicon, operate_icon)
    UH.SetText(self.onecost, 1)
    UH.SetText(self.tencost, self.send_data.max_times)

    UH.SetText(self.mid_key, self.data.miling_data.mid_key)
    UH.SetText(self.high_key,self.data.miling_data.high_key)

    UH.SetText(self.chapter_text,string.format(Language.DeepDarkFantasy.BoxChpaterShow,
    DataHelper.GetDaXie(self.send_data.chapter)))

    UH.SetText(self.mulit_times,string.format(Language.DeepDarkFantasy.BoxRollOpenTime,DataHelper.GetDaXie(self.send_data.max_times)))

    -- for k,v in pairs(self.data.miling_data.gold_reward_flag) do 
    --     LogError("!?~~~",v)
    -- end 
end

function DeepDarkFantasyBoxRollF:OnClickBoxSelect(type)
    if self.send_data.type ~= type then 
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    end 
    
    self.send_data.type = type
    -- 金
    if type == 3 then 
		UH.SizeDelta(self.GoldBox, Vector2.New(175, 160))
        UH.SizeDelta(self.SilverBox, Vector2.New(140, 128))
        self.GoldSelect:SetActive(true)
        self.SilverSelect:SetActive(false)
    else
        UH.SizeDelta(self.GoldBox, Vector2.New(140, 128))
        UH.SizeDelta(self.SilverBox, Vector2.New(175, 160))
        self.GoldSelect:SetActive(false)
        self.SilverSelect:SetActive(true)
    end 

    self.send_data.max_times = self.data:GetMiLingCurMaxRollNum(type == 3,self.send_data.chapter)
    self:onFlush({is_select = true})

    self.init = true
end

function DeepDarkFantasyBoxRollF:OnClickOnes()
    self.ctrl:SendOperateReq(3,self.send_data.chapter,self.send_data.type,1)
    self.data:MiLingRewardMark({is_gold = self.send_data.type == 3})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 

function DeepDarkFantasyBoxRollF:OnClickTens()
    self.ctrl:SendOperateReq(3,self.send_data.chapter,self.send_data.type,self.send_data.max_times)
    self.data:MiLingRewardMark({is_gold = self.send_data.type == 3})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 

-- 携带伙伴的cell
DarkFantasyBoxItem = DarkFantasyBoxItem or DeclareMono("DarkFantasyBoxItem", UIWidgetBaseItem)
function DarkFantasyBoxItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self.Call:SetData(Item.Create(data.gold_reward or data.silver_reward))
    self.IsComplete:SetActive(DeepDarkFantasyData.Instance:CheckBoxRewardGot(data,self.parent.send_data.type == 3))


end 

function DarkFantasyBoxItem:ShowEff()
    if self.effect_handle ~= nil then
        self.EffectTool:Stop(self.effect_handle)
		self.effect_handle = nil
    end
	self.effect_handle = self.EffectTool:Play(3161114) 
end

