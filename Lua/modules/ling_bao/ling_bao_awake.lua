LingBaoAwakeView = LingBaoAwakeView or DeclareView("LingBaoAwakeView", "ling_bao/ling_bao_awake")
VIEW_DECLARE_MASK(LingBaoAwakeView, ViewMask.Block)
function LingBaoAwakeView:LingBaoAwakeView()
    self.Data = LingBaoData.Instance
end

function LingBaoAwakeView:LoadCallback(param_t)
    
end

function LingBaoAwakeView:CloseCallback()
    if self.Data:ShengHenUnlockFlag() ~= -1 then
        ViewMgr:OpenView(ShengHenUnlockView, {shenghen_type = self.Data:ShengHenUnlockFlag()})
        self.Data:ShengHenUnlockFlag(-1)
    end
end

function LingBaoAwakeView:OnCloseClick()
    ViewMgr:CloseView(LingBaoAwakeView)
end

----------------------------LingBaoAwakePanel----------------------------
LingBaoAwakePanel = LingBaoAwakePanel or DeclareMono("LingBaoAwakePanel", UIWFlushPanel)
function LingBaoAwakePanel:LingBaoAwakePanel()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:LingBaoRemindItemList(), BindTool.Bind(self.FlushAll, self))
    end
end

function LingBaoAwakePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.now_select_shenghen = self.Data:MainShengHenType()
    self:FlushAll()
end

function LingBaoAwakePanel:FlushAll()
    local str_lv = self.data[self.now_select_shenghen].level
    UH.SetText(self.TxtShengHen, Format(self.language.ShengHenNameLv, self.language.ShengHenName[self.now_select_shenghen], str_lv))
    self.ObjMaxShow:SetActive(str_lv == self.Data:ShengHenMaxLv())
    if str_lv == self.Data:ShengHenMaxLv() then
        local cfg = self.Data:ShengHenLvAttrCfg(self.now_select_shenghen, str_lv)
        local attr_list = self.Data:GetAttrList(cfg)
        self.MaxList:SetData(attr_list)
        UH.SetText(self.TxtLimit, "")
    else
        local now_cfg = self.Data:ShengHenLvAttrCfg(self.now_select_shenghen, str_lv == 0 and 1 or str_lv)
        local now_attr_list = self.Data:GetAttrList(now_cfg, str_lv == 0)
        self.NowList:SetData(now_attr_list)
        local next_lv = (str_lv + 1) > self.Data:ShengHenMaxLv() and self.Data:ShengHenMaxLv() or (str_lv + 1)
        local next_cfg = self.Data:ShengHenLvAttrCfg(self.now_select_shenghen, next_lv)
        local next_attr_list = self.Data:GetAttrList(next_cfg)
        self.NextList:SetData(next_attr_list)

        local next_info_cfg = self.Data:ShengHenLvUpSpendCfg(str_lv + 1)
        if not TableIsEmpty(next_info_cfg) then
            if str_lv == 0 then
                local equip_count = self.Data:EquipCount(self.now_select_shenghen)
                local color = equip_count >= LingBaoConfig.LingBaoMax and COLORSTR.Green4 or COLORSTR.Red8
                UH.SetText(self.TxtLimit, Format(self.language.NeedAllWareTip, LingBaoConfig.LingBaoMax, color, equip_count, LingBaoConfig.LingBaoMax))
            else
                local total_lingbao_lv = self.Data:GetLingBaoStrAll(self.now_select_shenghen)
                local color = total_lingbao_lv >= next_info_cfg.strengthen_limit and COLORSTR.Green4 or COLORSTR.Red8
                UH.SetText(self.TxtLimit, Format(self.language.ShengHenLimit, color, total_lingbao_lv, next_info_cfg.strengthen_limit))
            end

            local has_num = Item.GetNum(next_info_cfg.awaken_consume)
            local need_num = next_info_cfg.consume_num
            local item = Item.Init(next_info_cfg.awaken_consume, has_num, false)
            UH.SetText(self.TxtName, item:Name())
            local color2 = has_num >= need_num and COLORSTR.Green4 or COLORSTR.Red8
            UH.SetText(self.TxtNum, Format(Language.Common.Cstr, color2, has_num, need_num))
            self.Cell:SetData(item)
        end
    end
    local is_can = self.Data:ShengHenIsCanStr(self.now_select_shenghen)
    self.Remind:SetNum(is_can and 1 or 0)

    if not self.now_str_lv then
        self.now_str_lv = str_lv
    else
        if self.now_str_lv ~= str_lv then
            self:PlayEffect()
            self.now_str_lv = str_lv
        end
    end
end

function LingBaoAwakePanel:PlayEffect()
    self.ObjNode:SetActive(false)
    if self.handle2 then
        UH.StopEffect(self.EffectTool2, self.handle2)
    end
    self.handle2 = UH.PlayEffect(self.EffectTool2, LingBaoConfig.ShengHenAwake[self.now_select_shenghen])
end

function LingBaoAwakePanel:BigEffectEnd()
    self.ObjNode:SetActive(true)
    if self.handle2 then
        UH.StopEffect(self.EffectTool2, self.handle2)
    end
    if self.handle then
        UH.StopEffect(self.EffectTool, self.handle)
    end
    self.handle = UH.PlayEffect(self.EffectTool, "3165236")
end

function LingBaoAwakePanel:OnUpClick()
    local is_can, str, p1, p2, p3 = self.Data:ShengHenIsCanStr(self.now_select_shenghen)
    if is_can then
        LingBaoCtrl.Instance:SendReq(LingBaoReqType.ShengHenStrUp, self.now_select_shenghen)
    else
        PublicPopupCtrl.Instance:Center(str)
        if p1 == 5 and p2 and p3 then
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = p2, jump_need_num = p3})})
        end
    end
end

function LingBaoAwakePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

--------------LingBaoAttrItem--------------
LingBaoAttrItem = LingBaoAttrItem or DeclareMono("LingBaoAttrItem", UIWidgetBaseItem)
function LingBaoAttrItem:LingBaoAttrItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.attr_name)
    UH.SetText(self.TxtValue, data.attr_percent_value)
end