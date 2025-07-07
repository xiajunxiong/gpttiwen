SignHappyView = SignHappyView or DeclareView("SignHappyView","serveractivity/sign_happy_view", Mod.Splendid.SignHappy)

function SignHappyView:OnClickClose()
    ViewMgr:CloseView(SignHappyView)
end

SignHappyPanel = SignHappyPanel or DeclareMono("SignHappyPanel", UIWFlushPanel)

function SignHappyPanel:SignHappyPanel()
    self.data = ServerActivityCtrl.Instance:Data()
    self.data_cares = {
        {data = self.data.sign_happy_data,func = self.FlushUpList,init_call = true},
    }
    self.data:GetSignHappyUpReward()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SIGN_HAPPY, 
    TimeType.Type_Special_4, Language.SignHappy.Time)
end
function SignHappyPanel:FlushUpList()
    self.GiftList:SetData(self.data:GetSignHappyUpReward())
    self.DayList:SetData(self.data:GetSignHappyConfig())
    self.BtnShow:SetActive(self.data.sign_happy_data.gain < 2)
    self.BtnInter.Interactable = self.data.sign_happy_data.gain > 0
    UH.SetText(self.BtnText,Language.SignHappy.GetState[self.data.sign_happy_data.gain > 0 and 1 or 2])
end
function SignHappyPanel:OnClickBtn()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SIGN_HAPPY,2) --请求一键领取
end
function SignHappyPanel:OnClickDesc()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Language.SignHappy.Desc})
  end

SignHappyViewItem = DeclareMono("SignHappyViewItem", UIWidgetBaseItem)

function SignHappyViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.LocalPos(self.ItemPos,ServeractivityConfig.SignHappy.UpItemPos[data.index])
    if data.index == 1 or data.index == 3 or data.index == 6 or data.index == 10 then
        self:SetDotLine(self.DotObjs2)
        self.LeftDot:SetActive(data.index == 1)
    else
       self:SetDotLine(self.DotObjs1)
    end
    data.item = Item.New(data)
    UH.SetIcon(self.Icon, Item.GetIconId(data.item_id))
    if self.effect then 
        self.EffectTool:Stop(self.effect) 
        self.effect = nil 
    end
    if data.state == 4 or data.state == 8 then self.effect = self.EffectTool:Play(4161007) end
    if data.gain then
        self.Hide:SetActive(false)
        UH.SetInter(self.Inter,false)
    else
        self.Hide:SetActive(data.state == 1)
    end
end
function SignHappyViewItem:SetDotLine(obj_arr)
    for i = 1, obj_arr:Length() do
        --3显示半实虚线 2实线 1虚线 6显示下一天领取情况 2当前领取情况 4未领取   
        local state = self.data.state
        --if state == 6 then state = 4 end ---6-->>4
        if state == 8 then state = 3 end ---8-->>3
        obj_arr[i]:SetActive(state == i)
    end
end
function SignHappyViewItem:OnClickItem()
    CellClicks.BagGridNoButton({GetData = function()
        return self.data.item
    end})
end


SignHappyDayItem = DeclareMono("SignHappyDayItem", UIWidgetBaseItem)

function SignHappyDayItem:SetData(data)
    if self.CellList then
        for i = 0,#data.reward_item do
            data.reward_item[i].reward_flag = data.reward_flag
            data.reward_item[i].seq1 = data.seq1
        end
        self.CellList:SetData(data.reward_item)
        UH.SetText(self.StateText,string.format(Language.SignHappy.GainState[data.reward_flag ~= 1 and 0 or data.reward_flag],DataHelper.GetDaXie(data.seq1 + 1)))
        local state = data.reward_flag
        state = data.reward_flag == 3 and 2 or data.reward_flag
        for i = 1,self.DotList:Length() do --点
            self.DotList[i]:SetActive(state == i - 1)  --0未领取  1领取 2未达到
        end
        state = data.reward_flag == 3 and 0 or data.reward_flag
        for i = 1,self.LineObjs:Length() do
            self.LineObjs[i]:SetActive(state == i - 1)
        end
        self.Hide:SetActive(data.reward_flag == 2)
        self.BtnInter.Interactable = state == 0
    else
        data.item = Item.New(data)
        self.Cell:SetData(data.item)
        self.Cell:MakeGrey(data.reward_flag == 1) --0未领取  1领取 2未达到
        if self.effect then 
            self.EffectTool:Stop(self.effect) 
            self.effect = nil 
        end
        if data.reward_flag == 0 then  self.effect = self.EffectTool:Play(4161008) end
    end
    UIWidgetBaseItem.SetData(self, data)
end
function SignHappyDayItem:OnClickItem()
    self:OnClickBtn()
    CellClicks.BagGridNoButton({GetData = function()
        return self.data.item
    end})
end

function SignHappyDayItem:OnClickBtn()
    if self.data.reward_flag ~= 0 then return end
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SIGN_HAPPY,1,self.data.seq1) --请求领取
end
