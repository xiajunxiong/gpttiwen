CowGoingLuckView = CowGoingLuckView or DeclareView("CowGoingLuckView","serveractivity/cow_going_luck_view",Mod.SpringFestival.CowGoingLuck)
function CowGoingLuckView:CowGoingLuckView()end
function CowGoingLuckView:OpenCallback()end
function CowGoingLuckView:LoadCallback()end
function CowGoingLuckView:CloseCallback()end

CowGoingLuckPanel = CowGoingLuckPanel or DeclareMono("CowGoingLuckPanel", UIWFlushPanel)
function CowGoingLuckPanel:CowGoingLuckPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
		{data = self.data.cow_going_lock_data, func = self.FlushPanel,init_call = false},
    }
    self.show_speed = 0.05

    self.config = self.data:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_COW_GOING_LUCK,Config.customs_gift_auto.gift_configure)
    self.introduce_bank = KeyList(Config.customs_gift_auto.customs_introduce_bank,"customs_introduce_id")
end 

function CowGoingLuckPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushShow()
end

function CowGoingLuckPanel:CatchIntroduce()
    for k,v in pairs(self.config) do 
        if v.type == self.data.cow_going_lock_data.id then 
            return self.introduce_bank[v.customs_introduce_id].customs_introduce
        end 
    end 
end

function CowGoingLuckPanel:CatchItemList()
    for k,v in pairs(self.config) do 
        if v.type == self.data.cow_going_lock_data.id then 
            return DataHelper.FormatItemList(v.reward_item)
        end 
    end 
end

function CowGoingLuckPanel:FlushShow()
    local customs_introduce = self:CatchIntroduce()
    if self.data.Instance.cow_going_lock_data.twice or self.data.cow_going_lock_data.today_reward == 1 then 
        UH.SetText(self.Texter,customs_introduce)
        self.Texter.gameObject:SetActive(true)
        self:OnEndTyper()
    else 
        self.Typer.gameObject:SetActive(true)
        self.Typer:ShowContent(customs_introduce, self.show_speed)
    end 
end

function CowGoingLuckPanel:FlushPanel()
    self.BtnOper:SetActive(self.data.cow_going_lock_data.today_reward == 0)
    self.Done:SetActive(self.data.cow_going_lock_data.today_reward == 1)
end

function CowGoingLuckPanel:OnEndTyper()
    self:FlushPanel()
    self.ItemList:SetData(self:CatchItemList())

    self.data:SetCowGoingLuckTwice()
end

function CowGoingLuckPanel:OnClickOpearte()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_GOING_LUCK,1)
end

function CowGoingLuckPanel:OnClickClose()
    ViewMgr:CloseView(CowGoingLuckView)
end