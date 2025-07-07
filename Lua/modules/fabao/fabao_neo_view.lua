FabaoNeoView = FabaoNeoView or DeclareView("FabaoNeoView", "fabao/fabao_neo", Mod.Fabao.Main)
VIEW_DECLARE_LEVEL(FabaoNeoView, ViewLevel.L1)
----Board=======UIWBoard
----Tabbar======UIWidgetList
local sp = {
    [0] = "xingxingzi",
    [1] = "xingxinghuang",
    [2] = "xingxinghong"
}
function FabaoNeoView:FabaoNeoView()
    self.lang = Language.Fabao
    self.fabao_data = FabaoData.Instance
    self.start_guide = false
end

function FabaoNeoView:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Title)
    self.Currency:DefaultSet()
    --判断是否进行了指引
    local guide = GuideManager.Instance:Guide()
    if guide and guide.id == 43 then
        self.start_guide = true
    end
end 

function FabaoNeoView:OnFlush()
end

function FabaoNeoView:CloseCallback()
    if self.start_guide then
        --打某怪物组
        local param_t = {
            req_type = FABAO_REQ_TYPE.GUIDE_MONSTER
        }
        FabaoCtrl.Instance:SendFabaoReq(param_t)
    end
end

FabaoNeoPanel = FabaoNeoPanel or DeclareMono("FabaoNeoPanel", UIWFlushPanel)
function FabaoNeoPanel:FabaoNeoPanel()
    self.data = FabaoData.Instance
    self.data_cares = {
        {data = self.data:GetFabaoList(), func = self.FlushList},
        {data = BagData.Instance.item_grid, func = self.FlushCurPanel, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi(
        "FabaoCreateBtn",
        function()
            if self.FabaoList:SetupCompleted() then
                --先找一下对应法宝是否被激活，如果被激活就算了
                local id = GuideData.Instance:GetFabaoId()
                local info = self.data:FabaoListInfo(id)
                if info and info.fabao_index <= -1 then
                    self.FabaoList:ClickItem(id)
                    return self.FabaoCreateBtn
                else
                    GuideManager.Instance:ForceStop()
                end
            end
        end
    )
end 

function FabaoNeoPanel:onFlush()

end

function FabaoNeoPanel:FlushList()
    local data,index = self.data:FabaoList()
    self.FabaoList:SetData(data)
    --如果是指引那不执行
    --if GuideManager.Instance:IsGuide() == false then
        self.FabaoList:ClickItem(self.select_data and self.select_data.id or index)
    --end
end

function FabaoNeoPanel:OnClickItem(data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    self.select_data = data
    self.data:ViewData().select_data = data
    self.data:ViewData().flush = not self.data:ViewData().flush
    self:FlushCurPanel()

    self.ShengJiPanel:ClearEffectList()
end

function FabaoNeoPanel:FlushCurPanel()
    local status = 0
    local level = self.select_data.info.fabao_level
    local jie_level = self.select_data.info.fabao_jie

    local flag_lianhua = self.select_data.info.fabao_index < 0
    local flag_max = self.data:GetHadMax(self.select_data.id)
    local flag_is_advanced = self.data:GetNeedAdvanced(self.select_data.id)

    self.Lianhua:SetActive(flag_lianhua)
    self.Max:SetActive(flag_max)

    self.TopInfo:SetActive(not flag_lianhua)
    self.Shengji:SetActive(not flag_lianhua and not flag_max and not flag_is_advanced)
    self.Shengjie:SetActive(not flag_lianhua and not flag_max and flag_is_advanced)

    if flag_lianhua then self.LianHuaPanel:SetData(self.select_data) end 
    if flag_max then self.CompleteMax:SetData(self.select_data) end 
    if not flag_lianhua then self.TopInfoPanel:SetData(self.select_data) end 
    if not flag_lianhua and not flag_max and not flag_is_advanced then 
        self.ShengJiPanel:SetData(self.select_data)
    end 
    if not flag_lianhua and not flag_max and flag_is_advanced then
        self.ShengjiePanel:SetData(self.select_data)
    end 
end

function FabaoNeoPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[93].desc})
end 

-- 法宝置顶顶信息
FabaoTopInfo = FabaoTopInfo or DeclareMono("FabaoTopInfo")
function FabaoTopInfo:FabaoTopInfo() end 
function FabaoTopInfo:SetData(select_data)
    self.FabaoCell:SetData(select_data)
    UH.SetText(self.FabaoName,select_data.name)
    UH.SetText(self.FabaoLevel, string.format("Lv.%d", select_data.info.fabao_level))
    self.JieBar:SetData(select_data.info.fabao_jie)
end

-- 炼化
LianHuaNeoPanel = LianHuaNeoPanel or DeclareMono("LianHuaNeoPanel")
function LianHuaNeoPanel:LianHuaNeoPanel() end 
function LianHuaNeoPanel:SetData(select_data)
    self.fabao_data = FabaoData.Instance
    self.data = select_data
    local attr_info = self.fabao_data:FabaoAttrInfo(select_data.id)
    UH.SetText(self.LblName,select_data.name)
    UH.SetText(self.LblDesc,attr_info.desc)
    UH.SetText(self.LblEffect,attr_info.effect_desc)
    UH.SetText(self.LblTip, Config.language_cfg_auto.textdesc[3].desc)

    local cost_item = Item.Create({item_id = select_data.compose_item})
    UH.SetText(self.LblCost,string.format(
            Language.Common.Cstr,
            cost_item:Num() >= select_data.compose_item_num and COLORSTR.Green7 or COLORSTR.Red10,
            cost_item:Num(),select_data.compose_item_num
        )
    )

    self.FabaoCell:SetData(select_data)
    self.FabaoSPCell:SetData(cost_item)
end

function LianHuaNeoPanel:OnClickCreate()
    -- 弹出购买界面
    if Item.GetNum(self.data.compose_item) < self.data.compose_item_num  then-- self.data.is_not_enough_stuff then
        MainOtherCtrl.Instance:GetWayView({item =  Item.Create({item_id = self.data.compose_item})})
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    FabaoCtrl.SendActive(self.data.id)
end

-- 升级
ShengJiNeoPanel = ShengJiNeoPanel or DeclareMono("ShengJiNeoPanel",UIWFlushPanel)
function ShengJiNeoPanel:ShengJiNeoPanel() 
    self.fabao_data = FabaoData.Instance
    self.data_cares = {
        {data = self.fabao_data:GetFabaoList(), func = self.FlushView, init_call = false},
    }
    self.handle_effect = {}
end 

function ShengJiNeoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_effect)
    TimeHelper:CancelTimer(self.handle_effect2)
end

function ShengJiNeoPanel:FlushView(old)
    old = old == nil and {} or old
    local old_fabao = old[self.select_item.id]
    local new_fabao = self.fabao_data:FabaoListInfo(self.select_item.id)
    if old_fabao and new_fabao then
        if new_fabao.fabao_level > old_fabao.fabao_level then
            UH.SetText(self.CurLevelE, string.format(Language.Common.Levels, old_fabao.fabao_level))
            UH.SetText(self.NextLevelE, string.format(Language.Common.Levels, new_fabao.fabao_level))
            self.SJEffect:SetActive(true)
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie2)
            self.handle_effect2 = TimeHelper:AddDelayTimer(function ()
                self.SJEffect:SetActive(false)
            end, 1.5)
        end
    end
end

function ShengJiNeoPanel:SetData(select_data)
    self.fabao_data = FabaoData.Instance
    self.lang = Language.Fabao
    self.data = select_data
    local exp_info = self.fabao_data:UplevelInfo( select_data.info.fabao_level )

    -- fabao_info
    UH.SetText(self.CurLevel, string.format("Lv.%d",  select_data.info.fabao_level))
    UH.SetText(self.NextLevel, string.format("Lv.%d", select_data.info.fabao_level + 1))
    UH.SetText(self.ExpValue, string.format("%d/%d", select_data.info.cur_exp, exp_info.exp))
    UH.FillAmount(self.Prog, select_data.info.cur_exp / exp_info.exp)
    UH.SetIcon(self.FabaoIcon, select_data.icon)

    -- Desc
    local str_cur,str_next = self.fabao_data:GetFabaoUpDesc(select_data,false)
    UH.SetText(self.LblCur, string.format("Lv.%d", select_data.info.fabao_level ))
    UH.SetText(self.LblNext, string.format("Lv.%d", select_data.info.fabao_level  + 1))
    UH.SetText(self.CurDesc,str_cur)
    UH.SetText(self.NextDesc,str_next)

    -- button desc
    local next_level = self.fabao_data:GetNextAdvanceLevel(select_data.info.fabao_level,select_data.info.fabao_jie)
    UH.SetText(self.NextLevelTip, string.format(self.lang.NextLevelTip,next_level))
    UH.SetText(self.OneCost, select_data.consume_coin)
    UH.SetText(self.TotalCost,(exp_info.exp - select_data.info.cur_exp) * select_data.consume_coin)

    self.select_item = select_data

end

function ShengJiNeoPanel:ClearEffectList()
    for k,v in pairs(self.handle_effect) do
        self.EffectTool:Stop(v)
        table.remove(self.handle_effect, k)
    end
end

function ShengJiNeoPanel:OnClickOne() 
    -- 判断数量
    local num = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    if num >= self.select_item.consume_coin then
        for k,v in pairs(self.handle_effect) do
            self.EffectTool:Stop(v)
            table.remove(self.handle_effect, k)
        end
        local handle = self.EffectTool:Play(10010275,self.EffPos)
        table.insert(self.handle_effect, handle)
        FabaoCtrl.SendUpLevel(self.select_item.id)
        AudioMgr:PlayEffect(AudioType.UI, "tongyongjinjie01")
    else
        PublicPopupCtrl.Instance:Error(ErrorText[94])
    end
end 

function ShengJiNeoPanel:OnClickTen() 
    local num = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local exp_info = self.fabao_data:UplevelInfo( self.select_item.info.fabao_level )
    if num >= (exp_info.exp - self.select_item.info.cur_exp) * self.select_item.consume_coin then
        local handle = self.EffectTool:Play(10010275,self.EffPos)
        table.insert(self.handle_effect, handle)
        FabaoCtrl.SendUpLevel(self.select_item.id, 1)
    else
        PublicPopupCtrl.Instance:Error(ErrorText[94])
    end
end 

-- 升阶
ShengJieNeoPanel = ShengJieNeoPanel or DeclareMono("ShengJieNeoPanel",UIWFlushPanel)
function ShengJieNeoPanel:ShengJieNeoPanel() 
    self.fabao_data = FabaoData.Instance
    self.data_cares = {
        {data = self.fabao_data:GetFabaoList(), func = self.FlushView, init_call = false},
    }
end 

function ShengJieNeoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_effect)
end


function ShengJieNeoPanel:FlushView(old)
    old = old == nil and {} or old
    local old_fabao = old[self.select_item.id]
    local new_fabao = self.fabao_data:FabaoListInfo(self.select_item.id)
    if old_fabao and new_fabao then
        if new_fabao.fabao_jie > old_fabao.fabao_jie then
            self.JJEffect:SetActive(true)
            self.handle_effect = TimeHelper:AddDelayTimer(function ()
                self.JJEffect:SetActive(false)
            end, 1)
        end
    end
end

function ShengJieNeoPanel:SetData(select_data)
    self.fabao_data = FabaoData.Instance
    self.lang = Language.Fabao
    self.data = select_data

    -- icon
    UH.SetIcon(self.FabaoIcon, select_data.icon)
    self.IconStar:SetData(select_data.info.fabao_jie)

    -- top star
    self.CurStar:SetData(select_data.info.fabao_jie)
    self.NextStar:SetData(select_data.info.fabao_jie + 1)

    -- Desc
    local str_cur,str_next = self.fabao_data:GetFabaoUpDesc(select_data,false)
    UH.SetText(self.CurDesc,str_cur)
    UH.SetText(self.NextDesc,str_next)

    -- StuffCell
    local cost_item = Item.Create({item_id = select_data.param})
    self.StuffCell:SetData(cost_item)
    local color =  cost_item:Num()>= select_data["param_num_" .. (select_data.info.fabao_jie + 1)] and COLORSTR.Black1 or COLORSTR.Red7 
    UH.SetText(self.StufffNum, string.format("<color=#%s>%d</color>/%d",color,cost_item:Num(), select_data["param_num_" .. (select_data.info.fabao_jie + 1)]))
    UH.SetText(self.StufffName, cost_item:Name())

    self.select_item = select_data

    self.LevelPanel:ClearEffectList()
end

function ShengJieNeoPanel:OnClickJinJie()
    if Item.GetNum(self.select_item.compose_item) < self.select_item.compose_item_num 
        or Item.GetNum(self.select_item.param) < self.select_item["param_num_" .. (self.select_item.info.fabao_jie + 1)] then
        local operate = Item.Create({item_id = self.select_item.compose_item})
        MainOtherCtrl.Instance:GetWayView({item = operate})

        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    else
        FabaoCtrl.SendAdvanced(self.select_item.id)
    end
end 

-- 满级
CompleteMaxNeoPanel = CompleteMaxNeoPanel or DeclareMono("CompleteMaxNeoPanel")
function CompleteMaxNeoPanel:CompleteMaxNeoPanel() end
function CompleteMaxNeoPanel:SetData(select_data)
    self.fabao_data = FabaoData.Instance
    self.FabaoCell:SetData(select_data)

    local attr_info = self.fabao_data:FabaoAttrInfo(select_data.id)
    UH.SetText(self.LevelDesc, attr_info.max_level_desc)    
    UH.SetText(self.JieDesc, attr_info.max_addition_desc)
    self.LevelDesc.gameObject:SetActive( attr_info.max_level_desc ~= "")
    self.JieDesc.gameObject:SetActive( attr_info.max_addition_desc ~= "")
    self.LevelTitle:SetActive( attr_info.max_level_desc ~= "")
    self.JieTitle:SetActive( attr_info.max_addition_desc ~= "")
end

-- 法宝阶星显示 
-- type == 0 可以显示0星
-- type == 1 显示全部星星
FabaoStarLight = FabaoStarLight or DeclareMono("FabaoStarLight", UIWidgetBaseItem)
function FabaoStarLight:FabaoStarLight() 
    self.sp_name = {
        [0] = "xingxingzi",
        [1] = "xingxinghuang",
        [2] = "xingxinghong",
    }
end 
function FabaoStarLight:SetData(Jie_Level)
    local level = math.floor(Jie_Level / 4)
    local num = Jie_Level % 4
    if self.Type == 0 then 
        for i = 1,4 do self.stars[i]:SetActive(false) end 
        if num == 0 and level == 0 then 
            self.Zero:SetActive(true)
        else 
            self.Zero:SetActive(false)
            for i = 1,self.stars:Length() do
                self.stars[i]:SetActive(i <= Jie_Level)
                UH.SpriteName(self.lights[i], i <= num and self.sp_name[level] or self.sp_name[level-1])
            end 
        end 
    else 
        for i = 1,self.stars:Length() do

            self.lights[i].gameObject:SetActive(i <= Jie_Level)
            UH.SpriteName(self.lights[i], i <= num and self.sp_name[level] or self.sp_name[level-1])
        end 
    end 
end