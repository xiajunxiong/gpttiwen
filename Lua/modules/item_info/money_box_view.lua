MoneyBoxView = MoneyBoxView or DeclareView("MoneyBoxView", "item_info/money_box")
VIEW_DECLARE_MASK(MoneyBoxView, ViewMask.BlockClose)

function MoneyBoxView:LoadCallback(param_t)
    MoneyBoxView.UseNum = param_t.num
end

function MoneyBoxView:OnClickClose()
    ViewMgr:CloseView(MoneyBoxView)
end

MoneyBoxViewF = MoneyBoxViewF or DeclareMono("MoneyBoxViewF", UIWFlushPanel)

function MoneyBoxViewF:MoneyBoxViewF()
    self.data = ItemInfoCtrl.Instance:Data()
    self.item = self.data:GetItemData()

    self.data_cares = {
        {data = self.data.money_box_info, func = self.FlushShow, init_call = false}
    }
end

function MoneyBoxViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushRbIcon()
end

function MoneyBoxViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.fly_effect_cache)
end

function MoneyBoxViewF:OnClickOpen()
    BagCtrl.Instance:SendUseItem({item_id = self.item.item_id, column = self.item.column_type, index = self.item.index, num = MoneyBoxView.UseNum or 1})
    self.EffectBg:SetActive(false)
    self.EffectTool:Play(3161034)
    self.ClickEffect:SetActive(true)
end

function MoneyBoxViewF:FlushRbIcon()
    local value = ShowType.MoneyBox1 == self.item:ShowType() and 1 or 2

    for i = 1, self.RbIcons:Length() do
        self.RbIcons[i]:SetActive(i == value)
    end
end

function MoneyBoxViewF:FlushShow()
    local info = self.data.money_box_info
    if info.init then
        return
    end
    self.Lingqu:SetActive(true)
    self.ShowAnim:SetTrigger(APH("GetLingYu"))
    self.fly_effect_cache = TimeHelper:AddDelayTimer(BindTool.Bind(self.ShowCreateEffect, self), 0.8)
    -- UH.SetText(self.GetTex, string.format(Language.Guild.HongBaoGetText, info.param2 or 0))
    self.NameList:SetData(info.item_list or {})
end

function MoneyBoxViewF:ShowCreateEffect()
    self:PlayCurrEffect(Vector3.New(0, 24, 0), Vector3.New(648, -319, 0), 7)
end

local crossPos1 = Vector3.New(-300, 100, 0)
local crossPos2 = Vector3.New(-62, 240, 0)

function MoneyBoxViewF:PlayCurrEffect(startPos, endPos, count)
    if not self.LingYuObjs then
        return
    end
    local effect_lsit = {}
    for i = 1, count, 1 do
        effect_lsit[i] = {}
        effect_lsit[i].index = i
        effect_lsit[i].shoot_curve_list = Game.CurveParabola(startPos, endPos, i % 2 == 0 and crossPos1 or crossPos2, GameMath.Ramdon())
        effect_lsit[i].ling_yu = self.LingYuObjs[i]
        effect_lsit[i].count = count
    end
    self.EffectList:SetDataList(effect_lsit)
end

-------------玩家领取到的红包消息--------------------
MoneyBoxViewItem =MoneyBoxViewItem or DeclareMono("MoneyBoxViewItem", UIWidgetBaseItem)
function MoneyBoxViewItem:MoneyBoxViewItem()
    
end
function MoneyBoxViewItem:SetData(data)
    -- UH.SetText(self.Name,data.name)
    UH.SetText(self.Val,data.num)
end
------
