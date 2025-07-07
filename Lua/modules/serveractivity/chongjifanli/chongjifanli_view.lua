ChongJiFanLiView = ChongJiFanLiView or DeclareMono("ChongJiFanLiView", UIWFlushPanel)

function ChongJiFanLiView:ChongJiFanLiView()
    
end

ChongJiFanLiPanel = ChongJiFanLiPanel or DeclareMono("ChongJiFanLiPanel", UIWFlushPanel)

function ChongJiFanLiPanel:ChongJiFanLiPanel()
    self.data = ChongJiFanLiData.Instance
    self.data_cares = {
        {data = self.data.chongjifanli_smart, func = self.FlushPanel,init_call = false},
        {data = ActivityData.Instance.activity_status_event, func = self.CheckOpen, init_call = false }
    }

end

function ChongJiFanLiPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.data.first_remind = true
    self.flush_jump = true
    -- 刷红点
    self.data:RemindNotice()
    self:FlushPanel()
end

function ChongJiFanLiPanel:FlushPanel()
    if self:CheckOpen() then
        return true
    end
    self:FlushInfo()
    self:FlushList()
end

function ChongJiFanLiPanel:CheckOpen()
    local is_open = self.data:IsOpen()
    if not is_open then
        ViewMgr:CloseView(RewardView)
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_CHONGJIFANLI) then
            PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenEndTip)
        else
            PublicPopupCtrl.Instance:Center(Language.ChongJiFanLi.Tip4)
        end 
        return true
    end
end

function ChongJiFanLiPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    -- 取消动效
    -- TimeHelper:CancelTimer(self.time_ht)
    -- self.time_ht = Invoke(function ()
    --     self.Anim:Play()
    -- end,0.1)
end

function ChongJiFanLiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    -- 刷红点
    self.data:RemindNotice()
    --TimeHelper:CancelTimer(self.time_ht)
end


function ChongJiFanLiPanel:FlushInfo()
    local day = self.data:GetMeetDay()
    local mark_cfg,need_day = self.data:GetNextMarkReward()
    self.BigPriceItemCell:SetData(Item.Create(mark_cfg.reward1_item))

    local tip_str
    local day_str1 = ColorStr(day .. Language.ChongJiFanLi.day, COLORSTR.Green2)
    local day_str2 = ColorStr(need_day .. Language.ChongJiFanLi.day, COLORSTR.Green2)
    if need_day == 0 then
        tip_str = string.format(Language.ChongJiFanLi.Tip1,day_str1)
        local flag = self.data:GetRewardFlag(mark_cfg)
        if flag == -1 then
            self.RewardTip:SetObjActive(false)
            return
        end
    else
        tip_str = string.format(Language.ChongJiFanLi.Tip2,day_str1,day_str2)
    end
    self.RewardTip:SetObjActive(true)
    UH.SetText(self.RewardTip, tip_str)
end



function ChongJiFanLiPanel:FlushList()
    local chongjifanli_item_list = self.data:GetChongJiFanLiItemList()
    if chongjifanli_item_list then
        self.ChongJiFanLiItemList:SetData(chongjifanli_item_list)
        local offset = self.data:GetOffset() or 0
        if offset < 1 then
            offset = 1
        end
        if self.flush_jump then
            self.ChongJiFanLiItemList:JumpHorizontalIndex(offset,-218)
            self.flush_jump = false
        end
    end
end


function ChongJiFanLiPanel:OnClickPreview()
    ViewMgr:OpenView(ChongJiFanLiPreview)
end



ChongJiFanLiItem = ChongJiFanLiItem or DeclareMono("ChongJiFanLiItem", UIWidgetBaseItem)

function ChongJiFanLiItem:ChongJiFanLiItem()
    self.data_instance = ChongJiFanLiData.Instance

end

function ChongJiFanLiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.day = data.day
    self.gameObject.name = "ChongJiFanLiItem"..self.day
    UH.SetText(self.LeiJiText, string.format(Language.ChongJiFanLi.Tip3,self.day))

    local flag = ChongJiFanLiData.Instance:GetRewardFlag(data)
    self.flag = flag
    self.GetRedPoint:SetNum(flag > 0 and 1 or 0)

    local is_cz = ChongJiFanLiData.Instance:IsDayMeetFlag()
    local meet_day = ChongJiFanLiData.Instance:GetMeetDay()

    local is_go_up_show = false
    if not is_cz then
        is_go_up_show = meet_day + 1 == data.day
    end


    self.IsShowMark = (data.mark == 1)
    self.NotReach:SetActive(flag == 0 and not is_go_up_show)
    self.GoTopUpButton:SetActive(is_go_up_show)
    self.GetButton:SetActive(flag > 0)
    self.HaveGet:SetActive(flag < 0)
    self.BigPriceLogo:SetActive(data.mark == 1)

    -- 奖励展示
    self.reward_item = {}
    self.reward1_item = data.reward1_item
    self.reward2_item = data.reward2_item

    if not TableIsEmpty(self.reward1_item) then
        table.insert(self.reward_item ,self.reward1_item)
    end
    if not TableIsEmpty(self.reward2_item) then
        for key, value in pairs(self.reward2_item) do
            table.insert(self.reward_item ,value)
        end
    end

    -- LogError("这是第",self.day,"天","self.reward_item",self.reward_item)
    self:FlushRewardItemList(self.reward_item)
end



function ChongJiFanLiItem:FlushRewardItemList(reward_item)
    local list = {}
    for i,v in pairs(reward_item) do
        local data = {}
        data.flag = self.flag
        data.item = Item.Create(v)
        table.insert(list, data)
    end

    self.RewardItemList:SetData(list)
    local pos = #list == 1 and Vector3.New(-1.3,-37,0) or Vector3.New(-1.3,7,0)
    self.RewardItemList.gameObject:SetLocalPosition(pos)
end

function ChongJiFanLiItem:OnClickGet()
    ChongJiFanLiCtrl.Instance:SendChongJiFanLiReq(ChongJiFanLiCtrl.REQ_TYPE.REQ_GET_REWARD,self.data.seq)
end

function ChongJiFanLiItem:OnClickGoTopUp()
    -- LogError("前往充值")
    ViewMgr:OpenViewByKey(Mod.Store.Recharge)
end

ChongJiFanLiRewardItem = ChongJiFanLiRewardItem or DeclareMono("ChongJiFanLiRewardItem",UIWidgetBaseItem)
function ChongJiFanLiRewardItem:RewardItem()
end

function ChongJiFanLiRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data.item)
    self.Effect:SetActive(data.flag == 1)
end



--------预览界面-----------------------
ChongJiFanLiPreview = ChongJiFanLiPreview or DeclareView("ChongJiFanLiPreview","serveractivity/chongjifanli/chongjifanli_reward_preview")
VIEW_DECLARE_MASK(ChongJiFanLiPreview,ViewMask.BgBlock)
function ChongJiFanLiPreview:ChongJiFanLiPreview()
    self.Board:SetData(self:GetType(),Language.ChongJiFanLi.Title1,Vector2.New(386, 552))
    self.data = ChongJiFanLiData.Instance
end

function ChongJiFanLiPreview:LoadCallback()
    local list_data = self.data:GetPreviewItemList() or {}
    self.ItemList:SetData(list_data)
    UH.SetText(self.Tip, Language.ChongJiFanLi.Tip)
end

function ChongJiFanLiPreview:OnClickClose()
    ViewMgr:CloseView(ChongJiFanLiPreview)
end

---------预览界面item------------------
ChongJiFanLiPreviewItem = ChongJiFanLiPreviewItem or DeclareMono("ChongJiFanLiPreviewItem",UIWidgetBaseItem)
function ChongJiFanLiPreviewItem:ChongJiFanLiPreviewItem()
end

function ChongJiFanLiPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- LogError("预览界面item",data)
    local item = Item.Create({item_id = data.item_id, num = data.num, is_bind = 1})
    self.ItemCell:SetData(item)
    self.JiaoBiao:SetActive(data.mark == 1)
end
























