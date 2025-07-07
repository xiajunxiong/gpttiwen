ColorfulIllustrateView = ColorfulIllustrateView or DeclareView("ColorfulIllustrateView","serveractivity/colorful_illustrate_view",Mod.Splendid.ColorfulIllustrate)

function ColorfulIllustrateView:ColorfulIllustrateView()
    self.Currency:DefaultSet()
    ActivityRandData.SetRemind(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE,ColorfulIllustrateData.Instance:GetColorNum())
    ColorfulIllustrateData.Instance:CheckRedPiontData()
end

function ColorfulIllustrateView:CloseCallback()
    ColorfulIllustrateData.Instance:Initialization()
end

function ColorfulIllustrateView:OnClickClose()
    ViewMgr:CloseView(ColorfulIllustrateView)
end

---------------------------------------ColorfulIllustratePanel-------------------------------------
ColorfulIllustratePanel = ColorfulIllustratePanel or DeclareMono("ColorfulIllustratePanel", UIWFlushPanel)

function ColorfulIllustratePanel:ColorfulIllustratePanel()
    self.data = ColorfulIllustrateData.Instance
    self.data_cares = {
        {data = self.data.select_data,func = self.OnFlush,init_call = false},
        {data = self.data.info_data,func = self.FlushColorView, init_call = true},
        {data = self.data.info_data,func = self.FlushPreinstallView,init_call = true,keys = {"is_fetch_make_flag"}},
    }
    self.make_page = 1
    self.consume_id = 24545
end

function ColorfulIllustratePanel:Awake()
    UIWFlushPanel.Awake(self)
    for i=1,self.CallList:Length() do
        self.CallList[i]:BindEvent(BindTool.Bind(self.FlushImageView, self))
    end
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE,TimeType.Type_Special_4)
    self.make_page = self:GetPageIndex() or 1
    self.RewardItem:SetData(self.data:GetRewardData())
    UH.SetText(self.Desc,Config.language_cfg_auto.textdesc[89].desc)
    UH.SetText(self.TitleText, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE))

    self:FlushPageList()
    self:FlushPageView()
end

--选中颜色
function ColorfulIllustratePanel:OnFlush()
    for k,item in pairs(self.ColorList.item_list or {}) do
        if item ~= nil then
            item:Cancel()
        end
    end
end

function ColorfulIllustratePanel:GetPageIndex()
    local _,num_list = self.data:GetSaveData()
    for i,v in ipairs(num_list) do
        if v ~= COLORFUL_ILLUSTRATE_IMAGE_NUM then
            return i
        end
    end
end

function ColorfulIllustratePanel:FlushColorView()
    self.ColorList:SetData(self.data:GetColorList())
    self.ColorList:SetCompleteCallBack(function()
        if self.IsColorAnim == nil then
            self.ColorAnim:Play()
            self.IsColorAnim = true
        end
    end)
    local save_data = self.data:GetSaveData()
    for i=1,self.CallList:Length() do
        self.CallList[i]:SetData(save_data[i])
    end
end

--刷新当前页数
function ColorfulIllustratePanel:FlushPageView()
    UH.SetText(self.Page,Format(Language.ColorfulIllustrate.PageNum,self.make_page,self.CallList:Length()))
    self.CallList[self.make_page]:OnChangeEvent()
end

function ColorfulIllustratePanel:FlushPageList()
    self.BtnLeft:SetActive(self.make_page > 1)
    self.BtnRight:SetActive(self.make_page < self.CallList:Length())
    for i=1, self.PageList:Length() do
        self.PageList[i]:SetActive(i == self.make_page)
    end
end

--刷新当前涂色图片的数量
function ColorfulIllustratePanel:FlushImageView(data)
    UH.SetText(self.ImageNum,Format("%s/%s",data.num,COLORFUL_ILLUSTRATE_IMAGE_NUM))
    UH.SetText(self.Score,data.score_num or 0)
    if self.data:IsPreinstall()and self.data.info_data.is_fetch_make_flag == 1 then
        UH.SetText(self.Score,self:GetSaveOldStringNum())
        UH.SetText(self.ImageNum,Format("%s/%s",COLORFUL_ILLUSTRATE_IMAGE_NUM,COLORFUL_ILLUSTRATE_IMAGE_NUM))
    end
    if data.save then
        local num = self.CallList[self.make_page]:GetColorNum(data.save.sort)
        self.ColorList.item_list[data.save.seq + 1]:SetNum(data.save.quality,num)
    end
end

function ColorfulIllustratePanel:OnClickPage(num)
    if self.make_page + num <= 0 then
        return
    end
    self.make_page = self.make_page + num
    self:FlushPageList()
    self:FlushPageView()
    self.PanelAnim:Stop()
    self.PanelAnim:Play()
end

--确定填入
function ColorfulIllustratePanel:OnClickConfirm()
    if #self.CallList[self.make_page]:GetColorList() == 0 then
        PublicPopupCtrl.Instance:Center(Language.ColorfulIllustrate.NotFullTip)
        return
    end
    PublicPopupCtrl.Instance:AlertTip(Language.ColorfulIllustrate.ConfirmTip,function()
        local num = COLORFUL_ILLUSTRATE_IMAGE_NUM - self.CallList[self.make_page]:GetFillImageNum()
        ServerActivityCtrl.Instance:SendColorfulIllustrateSeq(1,self.make_page-1,
        self.CallList[self.make_page]:GetNumList(),self:GetSaveString())
        self.CallList[self.make_page]:ClearColorList()
        if num > 0 then
            PublicPopupCtrl.Instance:Center(Format(Language.ColorfulIllustrate.FillImageNumTip,num))
        end
    end)
end

--撤销
function ColorfulIllustratePanel:OnClickErase()
    if #self.CallList[self.make_page]:GetColorList() == 0 then
        return
    end
    self.CallList[self.make_page]:RemveColorList()
    PublicPopupCtrl.Instance:Center(Language.ColorfulIllustrate.ReCallTip)
end

function ColorfulIllustratePanel:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[55].desc)
end

function ColorfulIllustratePanel:GetSaveString()
    local str = ""
    for i=1, self.CallList:Length() do
        if i == self.make_page then
            local save_string = self.CallList[i]:GetString()
            str = str .. save_string
            self.data:SetSaveStringData(i,save_string)
        else
            str = str .. self.data:GetSaveStringData(i)
        end
    end
    return str
end

function ColorfulIllustratePanel:OnClickReward()
    self.RewardItem:ActiveSelf()
end

function ColorfulIllustratePanel:FlushPreinstallView()
    if self.data:IsPreinstall()and self.data.info_data.is_fetch_make_flag == 1 then
        if self.make_page ~= 1 then
            self.make_page = 1
            self:OnClickPage(0)
        end
        UH.SetText(self.Score,self:GetSaveOldStringNum())
        UH.SetText(self.ImageNum,Format("%s/%s",COLORFUL_ILLUSTRATE_IMAGE_NUM,COLORFUL_ILLUSTRATE_IMAGE_NUM))
    end
    self.ConfirmInter.Interactable = self.data.info_data.is_fetch_make_flag == 0
    self.Preinstall:SetActive(self.data:IsPreinstall()and self.data.info_data.is_fetch_make_flag == 0 and self.data:GetCompletionNum() ~= 0)
    if self.data.info_data.is_fetch_make_flag == 1 then
        local list = self.data:GetPreinstall()
        for i=1,self.CallList:Length() do
            self.CallList[i]:SetPreinstall(list[i])
        end
    end
end

function ColorfulIllustratePanel:OnClickPreinstall()
    if self.data:GetColorNum() > 0 or Item.GetNum(self.consume_id) > 0 then
        PublicPopupCtrl.Instance:AlertTip(Language.ColorfulIllustrate.ContentTips,function()
            self:OnPreinstall()
        end)
        return
    end
    self:OnPreinstall()
end

function ColorfulIllustratePanel:OnPreinstall()
    local config = self.data:GetOtherConfig()[1]
    local num = self.data:GetCompletionNum()
    ViewMgr:OpenView(ActDialogTipsBackView,{
        reward_list = DataHelper.FormatItemList({config.seven_color_collection_reward}),
        content = Format(Language.ColorfulIllustrate.BackContent,num),
        cancel_name = RichTextHelper.SpriteStrAtlas(CommonItemId.Coin, 40, 40) .. config.coin_num * num, 
        confirm_name = RichTextHelper.SpriteStrAtlas(CommonItemId.Gold, 40, 40) .. config.gold_num * num, 
        cancel_func = function()
            if MallCtrl.IsNotCurrency(CurrencyType.CoinBind,config.coin_num * num) then
                ServerActivityCtrl.Instance:SendMakeUpReq(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE,0)
                self.data:SaveOldStringData()
            end
        end,
        confirm_func = function()
            if MallCtrl.IsNotCurrency(CurrencyType.Gold, config.gold_num * num) then
                ServerActivityCtrl.Instance:SendMakeUpReq(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE,1)
                self.data:SaveOldStringData()
            end
        end,
    })
end

function ColorfulIllustratePanel:GetSaveOldStringNum()
    local save_string = UnityPlayerPrefs.GetString(self.data:GetKey())
    if save_string ~= "" then
        local list,num_list = self.data:Interpret(save_string)
        return ColorfulIllustrateData.Instance:GetScoreNum(list[self.make_page] or {})
    end
    return 0
end

function ColorfulIllustratePanel:OnClickGetItem()
    if PlanActivityCtrl.Instance:ActGetWayViewById(ACTIVITY_TYPE.RAND_COLORFUL_ILLUSTRATE) then
        ViewMgr:CloseView(ColorfulIllustrateView)
    end
end

--颜色列表
----------------------------ColorfulIllustrateItem----------------------------
ColorfulIllustrateItem = ColorfulIllustrateItem or DeclareMono("ColorfulIllustrateItem", UIWidgetBaseItem)

function ColorfulIllustrateItem:SetData(data)
    UH.SpriteName(self.Label,"Label".. data.seq)
    for i=1,self.CellList:Length() do
        self.CellList[i]:SetData(data.colors[i])
    end
    self.gameObject.name = "Item" .. data.seq + 1
    UIWidgetBaseItem.SetData(self, data)
end

function ColorfulIllustrateItem:Cancel()
    if ColorfulIllustrateData.Instance:GetSelectData().seq ~= self.data.seq then
        self.ToggleGroup:SetAllTogglesOff()
    end
end

function ColorfulIllustrateItem:SetNum(quality,num)
    if self.CellList[quality + 1] then
        self.CellList[quality + 1]:SetNum(num)
    end
end

--颜色cell
----------------------------ColorfulIllustrateCell----------------------------
ColorfulIllustrateCell = ColorfulIllustrateCell or DeclareMono("ColorfulIllustrateCell", UIWidgetBaseItem)

function ColorfulIllustrateCell:SetData(data)
    UH.SpriteName(self.Icon,Language.ColorfulIllustrate.ColorKey[data.seq] .. data.quality)
    local pigment_num = ColorfulIllustrateData.Instance:GetPigmentNum(data.seq,data.quality)
    UH.SetText(self.Num,pigment_num)
    UIWidgetBaseItem.SetData(self, data)
end

function ColorfulIllustrateCell:OnClickType(type)
    ColorfulIllustrateData.Instance:SetSelectData(self.data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

--改变数量
function ColorfulIllustrateCell:SetNum(num)
    local pigment_num = ColorfulIllustrateData.Instance:GetPigmentNum(self.data.seq,self.data.quality)
    UH.SetText(self.Num,pigment_num - num)
end

--图鉴call
----------------------------ColorfulIllustrateCall----------------------------
ColorfulIllustrateCall = ColorfulIllustrateCall or DeclareMono("ColorfulIllustrateCall", UIWidgetBaseItem)

function ColorfulIllustrateCall:ColorfulIllustrateCall()
    self.color_list = {}
    self.color_config = ActivityConfig.ColorfulIllustrateColor
end

--加载保存本地图片颜色
function ColorfulIllustrateCall:SetData(data)
    for i=1,self.ImageList:Length() do 
        if data[i].sort then
            self.ImageList[i].color = self.color_config[data[i].seq][data[i].quality]
        end
    end
    UIWidgetBaseItem.SetData(self, data)
end

function ColorfulIllustrateCall:SetPreinstall(data)
    for i=1,self.ImageList:Length() do 
        if self.data[i].sort == nil then
            self.ImageList[i].color = self.color_config[data[i].seq][data[i].quality]
        end
    end
    UIWidgetBaseItem.SetData(self, data)
end

--点击图片
function ColorfulIllustrateCall:OnClickChange(index)
    self:SetImageColor(index,ColorfulIllustrateData.Instance:GetSelectData())
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

--监听改变
function ColorfulIllustrateCall:OnChangeEvent(data)
    if self.change_event_func then
        self.change_event_func({num = self:GetFillImageNum(),score_num = self:GetScoreNum(),save = data})
    end
end

function ColorfulIllustrateCall:BindEvent(func)
    self.change_event_func = func
end

--设置点击的颜色
function ColorfulIllustrateCall:SetImageColor(index,data)
    if data.sort then
        if self.data[index].sort and self.data[index].index == nil then
            PublicPopupCtrl.Instance:Center(Language.ColorfulIllustrate.FillColorTip)
            return
        end
        if self.data[index].sort == data.sort then
            return
        end
        if ColorfulIllustrateData.Instance:GetPigmentNum(data.seq,data.quality) - self:GetColorNum(data.sort) <= 0 then--是否有染料
            PublicPopupCtrl.Instance:Center(Language.ColorfulIllustrate.PigmentNumTip)
            return
        end
        self:ChangeImage(index,data)
    end
end

function ColorfulIllustrateCall:ChangeImage(index,data)
    if self.data[index].sort then
        self:ReplaceList(index)
        self:OnChangeEvent(self.data[index])
    end
    self.ImageList[index].color = self.color_config[data.seq][data.quality]
    self:AddColorList(index,data)
    self:OnChangeEvent(data)
end

function ColorfulIllustrateCall:ColorList(index,data)
    return {
        index = index,
        seq = data.seq,
        sort = data.sort,
        quality = data.quality
    }
end

--添加到队列
function ColorfulIllustrateCall:AddColorList(index,data)
    self.color_list[#self.color_list + 1] = self:ColorList(index,data)
    self.data[index] = self:ColorList(index,data)
end

--移除出队列
function ColorfulIllustrateCall:RemveColorList()
    local data = self.color_list[#self.color_list]
    if data and data.index then
        self.data[data.index] = {}
        self.ImageList[data.index].color = COLORS.White
        self.color_list[#self.color_list] = nil
        self:OnChangeEvent(data)
    end
end

--替换颜色
function ColorfulIllustrateCall:ReplaceList(index)
    for k,v in pairs(self.color_list) do
        if v.index == index then
            table.remove(self.color_list,k)
        end
    end
end

--获取填充数量
function ColorfulIllustrateCall:GetFillImageNum()
    local num = 0
    for k,v in pairs(self.data) do
        if v.sort then
            num = num + 1
        end
    end
    return num
end

--计算当前选中颜色的数量
function ColorfulIllustrateCall:GetColorNum(sort)
    local num = 0
    for k,v in pairs(self.color_list) do
        if v.sort == sort then
            num = num + 1
        end
    end
    return num
end

--获取序列化字符串
function ColorfulIllustrateCall:GetString()
    local str = ""
    for k,v in pairs(self.data) do
        str = str .. (v.sort and string.char(v.sort + 96) or " ")
    end
    return str
end

function ColorfulIllustrateCall:GetScoreNum()
    return ColorfulIllustrateData.Instance:GetScoreNum(self.data)
end

--获取保存的列表
function ColorfulIllustrateCall:GetColorList()
    return self.color_list or {}
end

--清理缓存
function ColorfulIllustrateCall:ClearColorList()
    self.color_list = {}
end

function ColorfulIllustrateCall:GetNumList()
    local list = {}
    for i=0,6 do
        list[i] = {}
        for j=0,2 do
            list[i][j] = self:GetSeqQualityNum(i,j)
        end
    end
    return list
end

function ColorfulIllustrateCall:GetSeqQualityNum(seq,quality)
    local num = 0
    for k,v in pairs(self.color_list) do
        if v.seq == seq and v.quality == quality then
            num = num + 1
        end
    end
    return num
end
----------------------------ColorfulIllustrateBlock----------------------------
ColorfulIllustrateBlock = ColorfulIllustrateBlock or DeclareMono("ColorfulIllustrateBlock", UIWidgetBaseItem)

function ColorfulIllustrateBlock:ColorfulIllustrateBlock()
    self.Image.alphaHitTestMinimumThreshold = 0.5
end

----------------------------ColorfulIllustrateReward----------------------------
ColorfulIllustrateReward = ColorfulIllustrateReward or DeclareMono("ColorfulIllustrateReward", UIWidgetBaseItem)

function ColorfulIllustrateReward:SetData(data)
    local other_onfig = ColorfulIllustrateData.Instance:GetOtherConfig()[1]
    local item = Item.New(other_onfig.seven_color_collection_reward)
    self.Call:SetData(item)
    UH.SetText(self.Name,item:Name())
    self.RewardList:SetData(data)
end

function ColorfulIllustrateReward:ActiveSelf()
    if not self.Parent.activeSelf then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    end
    self.Parent:SetActive(not self.Parent.activeSelf)
end

----------------------------ColorfulIllustrateRewardItem----------------------------
ColorfulIllustrateRewardItem = ColorfulIllustrateRewardItem or DeclareMono("ColorfulIllustrateRewardItem", UIWidgetBaseItem)

function ColorfulIllustrateRewardItem:SetData(data)
    data.item = Item.New(data.reward_show)
    UH.SetText(self.Name,data.item:Name())
    self.Call:SetData(data.item)
end