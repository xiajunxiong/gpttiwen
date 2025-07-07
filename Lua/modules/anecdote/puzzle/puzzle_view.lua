PuzzleBoard = PuzzleBoard or DeclareMono("PuzzleBoard",AnecdoteBaseBoard)
function PuzzleBoard:SetData(view_type,anecdote_type,close_func,title_name)
    AnecdoteBaseBoard.SetData(self,view_type,anecdote_type,close_func)
    if title_name then
        UH.SpriteName(self.NameImg,title_name)
    end
end

PuzzleLogic = PuzzleLogic or DeclareMono("PuzzleLogic")
function PuzzleLogic:PuzzleLogic()
    self.puzzle_data = PuzzleData.Instance
    self.finish_func = nil
    self.cell_data = nil
    self.row = nil
    self.col = nil
end

function PuzzleLogic:SetData(list_data,finish_func,row,col)
    self.cell_data = list_data
    self.List:SetData(list_data)
    self:SetFinishFunc(finish_func)
    self.row = row or 3
    self.col = col or 3
    self:CheckPass()
end

function PuzzleLogic:SetFinishFunc(func)
    self.finish_func = func
end

function PuzzleLogic:OnClickItem(data,item_obj,icon_obj)
    local select_data = self.List:GetSelectedData()
    if self.start_time == nil then
        self.start_time = os.time()
    end
    if #select_data == 1 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
        self.item_num = data.num
        self.item_index = data.index
        self.ElementA.gameObject:SetLocalPosition(item_obj:GetLocalPosition())
        self.ElementA:SetData(data)
        if data.img_type ~= PuzzleData.PuzzleImgType.Sprite then
            self.a_icon_obj = self.List:GetItem(data.index).RawIcon.gameObject
        else
            self.a_icon_obj = self.List:GetItem(data.index).SpriteIcon.gameObject
        end
    else
        self.List:SelectAll(false)
        if not self.puzzle_data:IsAdjoin(self.item_index,data.index) then
            if self.item_index ~= data.index then
                PublicPopupCtrl.Instance:Center(Language.Anecdote.PuzzleSelectTip)
            else
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
            end
            return
        end
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        -- 交换数据
        local a_data = TableCopy(self.cell_data[data.index])
        a_data.index = self.item_index
        local b_data = TableCopy(self.cell_data[self.item_index])
        b_data.index = data.index
        self.cell_data[self.item_index] = a_data
        self.cell_data[data.index] = b_data
        self.ElementB.gameObject:SetLocalPosition(item_obj:GetLocalPosition())
        self.ElementB:SetData(data)
        if data.img_type ~= PuzzleData.PuzzleImgType.Sprite then
            self.b_icon_obj = self.List:GetItem(data.index).RawIcon.gameObject
        else
            self.b_icon_obj = self.List:GetItem(data.index).SpriteIcon.gameObject
        end
        self.MaskObj:SetActive(true)
        self.move_ht = PuzzleData:PlayPuzzleMoveAnima(self.a_icon_obj,self.b_icon_obj,self.ElementA.gameObject,self.ElementB.gameObject,function()
            self.MaskObj:SetActive(false)
            self.List:SelectAll(false)
            self.List:SetData(self.cell_data)
            self:CheckPass()
        end)
    end
end

-- 拼图检测
function PuzzleLogic:CheckPass()
    if self.puzzle_data:IsPass(self.cell_data) then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PetGain)
        self.MaskObj:SetActive(true)
        if self.finish_func then
            self.finish_func()
        end
    end
end

function PuzzleLogic:OnDestroy()
    TimeHelper:CancelTimer(self.move_ht)
end

----------------------------JigsawPuzzleItem----------------------------
PuzzleItem = PuzzleItem or DeclareMono("PuzzleItem", UIWidgetBaseItem)

function PuzzleItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.img_type and data.img_type == PuzzleData.PuzzleImgType.Sprite then
        self.SpriteIcon.gameObject:SetActive(true)
        self.RawIcon.gameObject:SetActive(false)
        self:SetSpriteIcon()
    else
        self.SpriteIcon.gameObject:SetActive(false)
        self.RawIcon.gameObject:SetActive(true)
        self:SetRawIcon()
    end
end

function PuzzleItem:SetSpriteIcon()
    if self.data.raw_img_name == nil then
        return
    end
    UH.SpriteName(self.SpriteIcon,self.data.raw_img_name .. self.data.num)
end

function PuzzleItem:SetRawIcon()
    if self.data.raw_img_name and not self.seted_raw then
        if self.RawImage ~= nil then
            local path = string.format("anecdote_new/puzzle_game/%s.png",self.data.raw_img_name)
            self.RawImage:SetByPath(path)
        else
            UH.SetIconToRawImage(self.RawIcon,self.data.raw_img_name, ICON_TYPE.ANECDOTE)
        end
        self.seted_raw = true
    end
    self.RawIcon.uvRect = UnityEngine.Rect.New(self.data.x, self.data.y, self.data.w, self.data.h)
end

----------------------------- PuzzlePanel ---------------------------
PuzzlePanel = PuzzlePanel or DeclareMono("PuzzlePanel",UIWFlushPanel)
function PuzzlePanel:PuzzlePanel()
    self.puzzle_data = PuzzleData.Instance
    self.anecdote_type = AnecdoteType.Puzzle
    self.row = 3
    self.col = 3
    self.is_ok = false
    self.puzzle_name = "JiuGong"
    self.close_func = nil
    self.is_send = true --是否请求完成任务
end

-- anecdote_type 奇闻类型 -- 必须要有
-- close_func 关闭事件  -- 必须要有
-- puzzle_name 是精灵图，必须要有，纹理图可留空，留空就用预制体上的
-- img_type  == PuzzleData.PuzzleImgType
-- is_send 是否发生任务完成请求 -- 默认请求
-- row 拼图的行数 默认 3
-- col 拼图的列数 默认 3
function PuzzlePanel:SetData(view_type,anecdote_type,close_func,puzzle_name,img_type,is_send,row,col)
    self.anecdote_type = anecdote_type
    self.puzzle_name = puzzle_name
    self.close_func = close_func
    self.is_send = is_send == nil and true or is_send
    self.task_info = AnecdoteData.Instance:GetTaskInfoByType(self.anecdote_type)
    self.Board:SetData(view_type,self.anecdote_type,BindTool.Bind(self.OnCloseClick,self))
    if row then
        self.row = row
    end
    if col then
        self.col = col
    end
   
    self.mask_close = self.CloseShow
    if self.ResultRImg and puzzle_name and img_type ~= PuzzleData.PuzzleImgType.Sprite then
        UH.SetIconToRawImage(self.ResultRImg,puzzle_name,ICON_TYPE.ANECDOTE)
    end
    self.is_need_random = self.task_info and self.puzzle_data:IsNeedRandom(self.task_info.task_id)
    self.cell_data = self.puzzle_data:GetListData(self.cow,self.col,self.puzzle_name,self.is_need_random,img_type)
    self.Logic:SetData(self.cell_data,BindTool.Bind(self.FinishFunc,self),self.row,self.col)
end

function PuzzlePanel:Awake()
    UIWFlushPanel.Awake(self)
    if self.is_need_random == true then
        self.Animator:Play("puzzle_view_showing")
    end
end

function PuzzlePanel:OnDestroy()
    TimeHelper:CancelTimer(self.time_handle)
    UIWFlushPanel.OnDestroy(self)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function PuzzlePanel:FinishFunc()
    self.Animator:Play(self.acc_anima_name or "puzzle_view_disapear_801")
    TimeHelper:CancelTimer(self.time_handle)
    self.time_handle = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnAnimaExitClick,self),3)
    self.is_ok = true
    if self.is_send == true then
        self:SendRequest()
    end
end

function PuzzlePanel:SendRequest()
    AnecdoteCtrl.Instance:SendTaskAction(self.anecdote_type)
end

function PuzzlePanel:OnAnimaExitClick()
    if self.mask_close then
        self.mask_close:SetActive(self.is_ok)
    end
end

function PuzzlePanel:OnCloseClick()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if self.close_func then
        self.close_func(self.is_ok)
    end
end

function PuzzlePanel:SetAccAnimation(anima_name)
    self.acc_anima_name = anima_name
end

------------- 九宫拼图 ------------
PuzzleView = PuzzleView or DeclareView("PuzzleView", "anecdote/puzzle/puzzle_view",Mod.Anecdote.Puzzle)

VIEW_DECLARE_MASK(PuzzleView,ViewMask.BgBlock)

function PuzzleView:PuzzleView()
    self.anecdote_type = AnecdoteType.Puzzle
    self.Panel:SetData(self:GetType(),self.anecdote_type,BindTool.Bind(self.OnCloseFunc,self))
    self.config = Config.jigsaw_puzzle_auto.other[1]
end

function PuzzleView:OnCloseFunc(is_ok)
    if is_ok == true then
        SceneLogic.Instance:AutoToNpcs(self.config.npc_id)
        ViewMgr:CloseView(PuzzleView)
    else
        PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.PuzzleExitTip,function ()
            NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config.fail_talk})
            ViewMgr:CloseView(PuzzleView)
        end)
    end
end

------------- 神秘宝藏拼图 ------------
PuzzleStorehouseView = PuzzleStorehouseView or DeclareView("PuzzleStorehouseView","anecdote/puzzle/puzzle_storehouse_view",Mod.Anecdote.PuzzleStorehouse)
function PuzzleStorehouseView:PuzzleStorehouseView()
    self.anecdote_type = AnecdoteType.PuzzleStorehouse
    self.config = Config.treasure_fever_auto.other[1]
    self.Panel:SetData(self:GetType(),self.anecdote_type,BindTool.Bind(self.OnCloseFunc,self))
end

function PuzzleStorehouseView:OnCloseFunc(is_ok)
    if is_ok == true then
        SceneLogic.Instance:AutoToNpcs(self.config.npc_id)
        ViewMgr:CloseView(PuzzleStorehouseView)
    else
        PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.PuzzleStorehouseExitTip,function ()
            NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config.fail_talk})
            ViewMgr:CloseView(PuzzleStorehouseView)
         end)
    end
end

function PuzzleStorehouseView:OnEnterFunc()
   
end

-- 神秘宝藏地图
PuzzleStorehouseMap = PuzzleStorehouseMap or DeclareView("PuzzleStorehouseMap","anecdote/puzzle/puzzle_storehouse_map",Mod.Anecdote.PuzzleStorehouseMap)
function PuzzleStorehouseMap:PuzzleStorehouseMap()
    self.anecdote_type = AnecdoteType.PuzzleStorehouse
    ViewMgr:CloseView(BagView)
    MainViewData.Instance.rb_toggle.isOn = true
end

function PuzzleStorehouseMap:CloseCallback()
    local scene_cfg = AnecdoteLogic.Instance:GetSceneConfig(self.anecdote_type)
    SceneLogic.Instance:AutoToPos(scene_cfg.scene_id,nil,nil,nil,true)
    ViewMgr:CloseView(PuzzleStorehouseView)
end

function PuzzleStorehouseMap:OnCloseClick()
    ViewMgr:CloseView(PuzzleStorehouseMap)
end


------------- 机关巧解 ------------
PuzzleMechanicalView = PuzzleMechanicalView or DeclareView("PuzzleMechanicalView", "anecdote/puzzle/puzzle_mechanical_view",Mod.Anecdote.PuzzleMechanical)

VIEW_DECLARE_MASK(PuzzleMechanicalView,ViewMask.BgBlock)

function PuzzleMechanicalView:PuzzleMechanicalView()
    self.anecdote_type = AnecdoteType.PuzzleMechanical
    self.Panel:SetData(self:GetType(),self.anecdote_type,BindTool.Bind(self.OnCloseFunc,self),"PinTu",PuzzleData.PuzzleImgType.Sprite)
    self.Panel:SetAccAnimation("puzzle_view_disapear_802")
    self.config = Config.mechanical_puzzle_auto.other[1]
end

function PuzzleMechanicalView:OnCloseFunc(is_ok)
    if is_ok == true then
        SceneLogic.Instance:AutoToNpcs(self.config.npc_id)
        ViewMgr:CloseView(PuzzleMechanicalView)
    else
        PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.PuzzleMechanicalExitTip,function ()
            NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config.fail_talk})
            ViewMgr:CloseView(PuzzleMechanicalView)
        end)
    end
end


------------ 魂归故里 ------------------
PuzzleSoulBackView = PuzzleSoulBackView or DeclareView("PuzzleSoulBackView", "anecdote/puzzle/puzzle_soul_back_view", Mod.Anecdote.SoulBack)
function PuzzleSoulBackView:PuzzleSoulBackView()
    self.anecdote_type = AnecdoteType.SoulBack
    self.Panel:SetData(self:GetType(),self.anecdote_type,BindTool.Bind(self.OnCloseFunc,self),"PinTu",PuzzleData.PuzzleImgType.Sprite)
    self.config = Config.jigsaw_puzzle_auto.other[1]
end

function PuzzleSoulBackView:OnCloseFunc(is_ok)
    if is_ok == true then
        SceneLogic.Instance:AutoToNpcs(self.config.npc_id)
        ViewMgr:CloseView(PuzzleSoulBackView)
    else
        local npc_cfg = Cfg.NpcById(self.config.npc_id) or {}
        local npc_name = npc_cfg.name or ""
        local tip = string.format(Language.Anecdote.PuzzleComTip,npc_name)
        PublicPopupCtrl.Instance:AlertTip(tip,function ()
            NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config.fail_talk})
            ViewMgr:CloseView(PuzzleSoulBackView)
        end)
    end
end

------------ 帝王画像 ------------------
PuzzleEmperorPortraitView = PuzzleEmperorPortraitView or DeclareView("PuzzleEmperorPortraitView", "anecdote/puzzle/puzzle_emperor_portrait_view", Mod.Anecdote.EmperorPortrait)
function PuzzleEmperorPortraitView:PuzzleEmperorPortraitView()
    self.anecdote_type = AnecdoteType.EmperorPortrait
    self.Panel:SetData(self:GetType(),self.anecdote_type,BindTool.Bind(self.OnCloseFunc,self),"PinTu",PuzzleData.PuzzleImgType.Sprite)
    self.config = Config.emperor_portrait_auto.other[1]
end

function PuzzleEmperorPortraitView:OnCloseFunc(is_ok)
    if is_ok == true then
        SceneLogic.Instance:AutoToNpcs(self.config.npc_id)
        ViewMgr:CloseView(PuzzleEmperorPortraitView)
    else
        local npc_cfg = Cfg.NpcById(self.config.npc_id) or {}
        local npc_name = npc_cfg.name or ""
        local tip = string.format(Language.Anecdote.PuzzleComTip,npc_name)
        PublicPopupCtrl.Instance:AlertTip(tip,function ()
            NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config.fail_talk})
            ViewMgr:CloseView(PuzzleEmperorPortraitView)
        end)
    end
end

------------ 心心相印 ------------------
PuzzleWeddingView = PuzzleWeddingView or DeclareView("PuzzleWeddingView", "anecdote/puzzle/puzzle_wedding_view", Mod.Love.Puzzle)
function PuzzleWeddingView:PuzzleWeddingView()
    self.anecdote_type = AnecdoteType.EmperorPortrait
    self.Panel:SetData(self:GetType(),self.anecdote_type,BindTool.Bind(self.OnCloseFunc,self),"",PuzzleData.PuzzleImgType.Sprite)
    self.time = 0
    if self.time > 0 then
        self.time_ht = Invoke(function ()
            self:OnCloseFunc(false)
            PublicPopupCtrl.Instance:Center(Language.LuckyMaze.PuzzleTimeTip)
        end, self.time)
    end

    local puzzle_logic = self.Panel.Logic
    puzzle_logic:SetFinishFunc(function ()
        self.EffectTool:Play("7161013")
        self.Panel:FinishFunc()
    end)
end

function PuzzleWeddingView:CloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
end

-- is_ok == true 说明是拼图完成，== flase则是手动关闭
function PuzzleWeddingView:OnCloseFunc(is_ok)
    if is_ok == true then
        local param_t = {
            req_type = MarryConfig.FBReqType.sub_act,
            param1 = MarryConfig.FBSubActType.xin_xin_xiang_yin,
        }
        MarryCtrl.Instance:SendWeddingFBReq(param_t)
        ViewMgr:CloseView(PuzzleWeddingView)
        MarryCtrl.Instance:PlayPuzzle(2)
    else
        ViewMgr:CloseView(PuzzleWeddingView)
    end
end
------------- 仙图重铸 ------------
PuzzleFairyView = PuzzleFairyView or DeclareView("PuzzleFairyView", "anecdote/puzzle/puzzle_fairy_view")

VIEW_DECLARE_MASK(PuzzleFairyView,ViewMask.BgBlock)

function PuzzleFairyView:PuzzleFairyView()
    local type = FairyWhereData.Instance:GetTurnType()
    self.puzzle_name = tostring(type) .. [[_]]
    self.anecdote_type = AnecdoteType.Puzzle
    self.img_type = PuzzleData.PuzzleImgType.Sprite
    self.config = Config.jigsaw_puzzle_auto.other[1]

    self.Panel:SetData(self:GetType(),self.anecdote_type,
    BindTool.Bind(self.OnCloseFunc,self),self.puzzle_name,self.img_type)
    self.ResultRImg:SetByPath(Format(PeriodActivityConfig.PuzzleFairyPath,type))
end

function PuzzleFairyView:OnCloseFunc(is_ok)
    if is_ok == true then
        ViewMgr:CloseView(PuzzleFairyView)
        FairyWhereCtrl.Instance:OnFairyWhereFinish(1)
    else
        PublicPopupCtrl.Instance:AlertTip(Language.FairyWhere.PuzzleFairyTip,function ()
            ViewMgr:CloseView(PuzzleFairyView)
            FairyWhereCtrl.Instance:OnFairyWhereFail(1)
        end)
    end
end