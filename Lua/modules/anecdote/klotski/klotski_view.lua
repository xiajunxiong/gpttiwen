KlotskiView = KlotskiView or DeclareView("KlotskiView", "anecdote/klotski_view", Mod.Anecdote.Klotski)

VIEW_DECLARE_MASK(KlotskiView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(KlotskiView, ViewLevel.L1)

function KlotskiView:KlotskiView()
    self.config = Config.klotski_auto

    self.data = KlotskiData.Instance
    local tab = {
        row = self.config.initial[1].row,
        col = self.config.initial[1].col,
        space_pos = self.config.initial[1].space_position
    }

    local pos_tab = {}
    for i, v in ipairs(self.config.position_initial) do
        pos_tab[i] = v.content + 1
    end

    self.klotski_data = self.data:GetStruct(Mod.Anecdote.Klotski, true, tab, pos_tab)

    -- 设置图片原本大小
    self.ImgBack:SetNativeSize()
    local size = self.RectImageBack.sizeDelta

    -- 根据行列设置格子大小和每列个数
    self.cell_size = Vector2.New(size.x / self.klotski_data.col, size.y / self.klotski_data.row)
    self.ListGrid.cellSize = self.cell_size
    self.ListGrid.constraintCount = self.klotski_data.col

    -- 设置用于交换的两个物体的格子大小
    self.RectExchangeA.sizeDelta = self.cell_size
    self.RectExchangeB.sizeDelta = self.cell_size

    -- 用于在关闭时显示的对话id
    self.need_show_talk_id = -1

    -- 是否完成
    self.is_finish = false

    -- 拼图名字，用于加载图片
    self.icon_name = "_LocKlotski_1"

    self.data:SetIsCanClick(false)
    
    -- 设置用于交换的道具A、B和其他相关的参数
    self.data:SetExchangeItem(self.RImgExchangeA, self.RImgExchangeB, self.RectExchangeA, self.RectExchangeB, self.icon_name)

    self.data:SetClickCallback(BindTool.Bind(self.ClickCallback, self))
    self.data:SetFinishCallback(BindTool.Bind(self.FinishCallback, self))

    -- 这里需要创建完成才能点击
    self.List:SetCompleteCallBack(function()
        self.data:SetIsCanClick(true)
    end)
    self.List:SetData(self.klotski_data.list)
end

-- 点击关闭
function KlotskiView:OnCloseClick()
    PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.KlotskiExitTip, function()
        self.need_show_talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.Klotski)
        ViewMgr:CloseView(KlotskiView)
    end)
end

function KlotskiView:CloseCallback()
    self.data:Clear()
    if self.need_show_talk_id ~= -1 then
		NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.need_show_talk_id})
    else
        if self.is_finish then
            SceneLogic.Instance:AutoToNpcs(self.config.other[1].npc_id)
        end
    end
end

-- 点击道具回调
function KlotskiView:ClickCallback()
    self.data:SetSpaceMono(self.List:GetItem(self.klotski_data.space_pos + 1))
end

-- 交换后成功匹配回调
function KlotskiView:FinishCallback()
    self.is_finish = true
    self.ObjFinishMask:SetActive(true)
    AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.Klotski)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
end

-- 点击全匹配遮罩
function KlotskiView:OnFinishClick()
    ViewMgr:CloseView(KlotskiView)
end

----------------------------KlotskiItem----------------------------
KlotskiItem = KlotskiItem or DeclareMono("KlotskiItem", UIWidgetBaseItem)
function KlotskiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- 根据图片名字给交换的道具设置图片
    UH.SetIconToRawImage(self.RImage, KlotskiData.Instance:GetIconName(), ICON_TYPE.ANECDOTE)
    self.RImage.uvRect = KlotskiData.Instance:GetRect()
    self:UpdateImage()
end

-- 更新图片显示
function KlotskiItem:UpdateImage()
    self.RImage.enabled = true --self.data.now_pos ~= KlotskiData.Instance:GetSpacePos()
    local rect_r, rect_c = KlotskiData.Instance:PosToRC(self.data.right_pos)
    rect_r = rect_r / KlotskiData.Instance:GetRow()
    rect_c = rect_c / KlotskiData.Instance:GetCol()
    self.RImage.uvRect = UnityEngine.Rect.New(rect_c, rect_r, self.RImage.uvRect.width, self.RImage.uvRect.height)
end

-- 点击事件
function KlotskiItem:OnClick()
    KlotskiData.Instance:OnItemClick(self)
end