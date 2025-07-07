--===========================ExtraBonusView===========================
ExtraBonusView = ExtraBonusView or DeclareMono("ExtraBonusView", UIWFlushPanel)

function ExtraBonusView:ExtraBonusView()
    self.data_cares = {
        {data = ServerActivityData.Instance.bonus_data,func = self.FlushBonusView},
    }
end

function ExtraBonusView:FlushBonusView()
    local info = ServerActivityData.Instance:GetBonusInfo(self.act_type,self.type)
    if info == nil then self.gameObject:SetActive(false) end
    self.Panel:SetActive(info.type_item == 1)
    local ProgressTips = Language.ExtraBonus.ProgressTips
    if self.type == nil then
        UH.SetText(self.Progress,Format(ProgressTips[1],info.curr_num,info.need_num))
    else
        UH.SetText(self.Progress,ProgressTips[2])
    end
    info.item = Item.New(info.reward_item)
    info.item.is_grey = info.flag == 1
    self.Call:SetData(info.item)
    self.Mark:SetActive(info.flag == 1)
    self.Effect:SetActive(info.flag == 0 and info.curr_num >= info.need_num)
    self.sned_func = function()ServerActivityCtrl.Instance:SendBonusOperaReq(info.activity_id,1,info.seq)end
end

function ExtraBonusView:OnClickItem()
    if self.sned_func then
        self.sned_func()
    end
end