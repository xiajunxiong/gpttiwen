LuckyMazeView = LuckyMazeView or DeclareView("LuckyMazeView","serveractivity/lucky_maze_view",Mod.Splendid.LuckyMaze)

function LuckyMazeView:LuckyMazeView()
    LuckyMazeView.SendReq(LuckyMazeData.CtrlType.PuzzleInfo)
end

function LuckyMazeView:OnBoxClick(box_id,box_obj)
    local box_data = LuckyMazeData.Instance:GetBoxData(box_id)
    local pos = UH.GetRootPosition(box_obj)
    pos.y = pos.y + 110
    ViewMgr:OpenView(ShowItemView,{item_list = box_data.item_list,pos = Vector3.New(pos.x,pos.y,0),pivot = 0})
end

function LuckyMazeView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[108].desc)
end

function LuckyMazeView:OnCloseClick()
    ViewMgr:CloseView(LuckyMazeView)
end

function LuckyMazeView.SendReq(ctrl_type,p1,p2,p3)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_LUCKY_MAZE,ctrl_type,p1,p2,p3)
end

function LuckyMazeView:CloseCallback()
    LuckyMazeData.Instance:SetNotPassId()
end

LuckyMazePanel = LuckyMazePanel or DeclareMono("LuckyMazePanel",UIWFlushPanel)
function LuckyMazePanel:LuckyMazePanel()
    self.data = LuckyMazeData.Instance
    self.data_cares = {
        {data = self.data:GetActData(),func = self.FlushPanel,init_call = false},
        {data = self.data.puzzle_data,func = self.FlushPanel,init_call = false,keys = {"puzzle_info"}},
    }
    self.AddCountBtnObj:SetActive(DEBUG_MODE)
    self.center_card = self.CardList[4]
    self.card_pos_report_list = {} -- 这个是拿来记录每张卡片对应的img_id,img_id 为key,作用就是方便拿到对应的card_item
    self.max_speed = 3      -- 这个值越小，最大速度愉快，指最大多少帧完成一个阶段
    self.drag_speed = 4     -- 拖拽的滑动速度
    --此界面刘海屏特殊处理
	if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        UH.LocalScale(self.PuzzleInfoTans,Vector3.New(0.95,0.95,1))
	end
    self.Animator:ListenEvent("cards_on_exit_event",BindTool.Bind(self.OnCardsOnExitEvent,self))
    self.Animator:ListenEvent("cards_off_exit_event",BindTool.Bind(self.OnCardsOffExitEvent,self))
    self:InitData()
end

function LuckyMazePanel:Awake()
    UIWFlushPanel.Awake(self)
    --self:InitData()
    self.ActTime:CloseCountDownTime()
    self.ActTime:SetTimeText(Language.LuckyMaze.ActTime)
	self.ActTime:CreateActTime(ACTIVITY_TYPE.RAND_LUCKY_MAZE,TimeType.Type_Special_4)
    self.Animator:Play("card_showing")
    UH.SetText(self.TitleText, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_LUCKY_MAZE))
    -- local jump_cfg = ServerActivityCtrl.Instance:ActGetWayCfgById(ACTIVITY_TYPE.RAND_LUCKY_MAZE)
    -- if jump_cfg ~= nil then
    --     UH.SetText(self.ItemText, Format(Language.Common.GetWayItemName, Item.GetName(jump_cfg.item_id)))
    --     UH.SetIcon(self.ItemIcon, Item.GetIconId(jump_cfg.item_id), ICON_TYPE.ITEM)
    -- end
end

function LuckyMazePanel:InitData()
    self.card_trans_data = {}
    self.all_cards = {}
    local init_func = function (card)
        local data = {}
        data.scale = card.CardTrans.localScale
        data.x = card.gameObject:GetLocalPosition().x
        data.child_index = card.gameObject.transform:GetSiblingIndex()
        data.pos = card.gameObject:GetLocalPosition()
        data.name = card.gameObject.name
        return data
    end
    table.insert(self.card_trans_data,init_func(self.VirItemL))
    for i = 1,self.CardList:Length() do
        table.insert(self.card_trans_data,init_func(self.CardList[i]))
    end
    table.insert(self.card_trans_data,init_func(self.VirItemR))
    self.max_step = #self.card_trans_data
    self.center_card.gameObject.transform:SetParent(self.CardsTopObj.transform)
end

function LuckyMazePanel:OnDestroy()
    self.ActTime:CloseCountDownTime()
    LuckyMazeData.Instance:SetChangeIndex(nil,0)
    if self.run_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_timer)
    end
    self.data.remind_data[RoleData.Instance:GetRoleId()] = self.data.act_data.rand_count
    UIWFlushPanel.OnDestroy(self)
end

function LuckyMazePanel:FlushPanel()
    self:FlushCard()
    self:FlushInfo()
end

function LuckyMazePanel:FlushInfo()
    self.TaskList:SetData(self.data:GetTaskData())
    local child_is_full = self.data:GetPuzzleChildFull(self.puzzle_img_id)
    local pass_flag = PuzzleData.Instance:IsPass(self.data:GetPuzzleOneData(self.puzzle_img_id).child_data)
    local count_str = ColorStr(tostring(self.data.act_data.rand_count),COLORSTR.Green4)
    UH.SetText(self.RemainText,string.format(Language.LuckyMaze.Remain,count_str))
    if self.data.act_data.can_enter_fb == 1 or pass_flag then
        UH.SetText(self.PlayBtnText,Language.LuckyMaze.BtnEnterScene)
    else
        UH.SetText(self.PlayBtnText,Language.LuckyMaze.BtnGetCard)
    end
    self.PlayBtnIntera.Interactable = (not pass_flag and child_is_full == 0) or self.data.act_data.can_enter_fb == 1
    self.RedPoint:SetNum(self.data.act_data.can_enter_fb)
end

function LuckyMazePanel:FlushCard()
    self.puzzle_img_id = self.data:GetPuzzleIndex() or 0
    if not self.first_fluch_card then
        local img_sort_list = self.data:GetImgSortList(self.puzzle_img_id)
        for i = 1,self.CardList:Length() do
            self.CardList[i]:SetData(self.data:GetPuzzleOneData(img_sort_list[i]))
            self.card_pos_report_list[img_sort_list[i]] = i
        end
        self.first_fluch_card = true
    end
    self.center_card:SetData(self.data:GetPuzzleOneData(self.puzzle_img_id))
end

function LuckyMazePanel:OnPlayClick()
    local child_is_full = self.data:GetPuzzleChildFull(self.puzzle_img_id)
    if not child_is_full then
        return
    end
    if self.data.act_data.can_enter_fb > 0 then
        TeamData.Instance:CheckSingle(function ()
            self.center_card.FinishEffectObj:SetActive(false)
            LuckyMazeView.SendReq(LuckyMazeData.CtrlType.EnterScene)
            ViewMgr:CloseView(LuckyMazeView)
            ViewMgr:CloseView(PlanActivityView)    
        end)
        -- if TeamData.Instance:InTeam() then
		-- 	PublicPopupCtrl.Instance:Error(Language.LuckyMaze.TeamTip)
		-- 	return
		-- end
    else
        if PuzzleData.Instance:IsPass(self.data:GetPuzzleOneData(self.puzzle_img_id).child_data) then
            PublicPopupCtrl.Instance:Error(Language.LuckyMaze.AccTip)
            return
        end
        if self.data.act_data.rand_count > 0 then
            if child_is_full == 1 then
                PublicPopupCtrl.Instance:Error(Language.LuckyMaze.GetTip)
                return
            end
            LuckyMazeView.SendReq(LuckyMazeData.CtrlType.GetCard,self.puzzle_img_id)
        else
            PublicPopupCtrl.Instance:Error(Language.LuckyMaze.CountTip)
        end
    end
end

function LuckyMazePanel:OnCloseClick()
    self.Animator.enabled = false
end

function LuckyMazePanel:OnCardsOnExitEvent()
    self.anima_ing = nil
    if not self.on_touch then
        self.center_card.gameObject.transform:SetParent(self.CardsTopObj.transform)
    end
end

function LuckyMazePanel:OnCardsOffExitEvent()
    
end

function LuckyMazePanel:OnTouchClick()
    self.on_touch = nil
    self:HandleCardsAnimation(0)
end

-- type == 0 则检查on的 == 1 则检查 off 的
function LuckyMazePanel:HandleCardsAnimation(type)
    if self.data.act_data.can_enter_fb > 0 then
        return
    end
    -- 不做self.anima_ing判断，操作的流程一点
    if not self.cards_anima_on and type == 0 then
        self.Animator:Play("cards_on")
        self.cards_anima_on = true
        self.anima_ing = true
    elseif self.cards_anima_on and type == 1 then
        -- 注释掉，取消卡牌回去动画
        -- self.Animator:Play("cards_off")
        -- self.cards_anima_on = nil
        -- self.anima_ing = true
    end
end

-- 防止有时候协议下发的慢，就触发了这个，
function LuckyMazePanel:CheckData()
    if not self.center_card.ChildList:GetData() then
        return false
    else
        return true
    end
end

function LuckyMazePanel:OnTouchDown()
    if not self:CheckData() then
        return
    end
    if self.data.act_data.can_enter_fb > 0 then
        PublicPopupCtrl.Instance:Error(Language.LuckyMaze.TouchTip)
        return
    end
    -- 表示触摸滑动中
    self.on_touch = true
    self.center_card.gameObject.transform:SetParent(self.CardsObj.transform)
    self.center_card.ChildList:SelectAll(false)
    self:HandleCardsAnimation(0)
end

function LuckyMazePanel:OnTouchUpClick()
    self.center_card.gameObject.transform:SetParent(self.CardsTopObj.transform)
end

function LuckyMazePanel:OnTouchDrag()
    if not self:CheckData() then
        return
    end
    if self.data.act_data.can_enter_fb > 0 then
        return
    end
    if not self.cards_anima_on then
        return
    end
    
    local dir,speed = self:GetMoveDV()
    if dir == 0 or speed >= 100 then
        return
    end
   self:MoveItem(dir,speed)
end

function LuckyMazePanel:OnTouchEndDrag()
    if not self:CheckData() then
        return
    end
    if self.data.act_data.can_enter_fb > 0 then
        return
    end
    self.last_move_x = nil
    if self.run_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_timer)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MapOpen)
    self.run_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.MoveItem,self,self.end_dir,self.max_speed,true,function()
        LuckyMazeView.SendReq(LuckyMazeData.CtrlType.SetPuzzleIndex,self.puzzle_img_id)
        self.on_touch = nil
        self:FlushInfo()
        self:ResetCards()
        self:HandleCardsAnimation(1)
    end))
end

function LuckyMazePanel:ResetCards()
    local card_sort = self.data:GetImgSortList(self.puzzle_img_id) -- 从左到右以位置x的排序
    for k = 1,3 do
        local data = self.data:GetPuzzleOneData(card_sort[k])
        self:SetObjStepState(self.CardList[k],k + 1,true,data)
    end
    for k = #card_sort,4,-1 do
        local data = self.data:GetPuzzleOneData(card_sort[k])
        self:SetObjStepState(self.CardList[k],k + 1,true,data)
    end
    self:SetObjStepState(self.VirItemR,self.max_step)
    self:SetObjStepState(self.VirItemL,1)
end

function LuckyMazePanel:MoveItem(dir,speed,is_ret,callback_func)
    if dir == nil then
        return true
    end
    local vir_can_move = true
    local is_back = false
    for i = 1,self.CardList:Length() do
        local pos = self.CardList[i].gameObject:GetLocalPosition()
        local step,next_step = self:GetStep(pos.x,dir)
        if step == self.max_step and next_step == 1 then   
            step = 1 + 1
            next_step = 1 + 2
            pos.x = -pos.x
            vir_can_move = true
        elseif step == 1 and next_step == self.max_step then
            step = self.max_step - 1
            next_step = self.max_step - 2
            pos.x = -pos.x
            vir_can_move = true
        end
        
        if step == 2 and next_step == 1 and vir_can_move == true then
            if self.cur_ctrl_i ~= i then
                self.cur_ctrl_i = i
                self:SetObjStepState(self.VirItemR,self.max_step)
            end
            self.VirItemR:SetData(self.CardList[i]:GetData())
            self:Move(self.VirItemR,speed,dir,self.max_step,self.max_step - 1,nil,true)
            vir_can_move = false
        elseif step == 1 and next_step == 2 and vir_can_move == true then
            if self.cur_ctrl_i ~= i then
                self.cur_ctrl_i = i
                self:SetObjStepState(self.VirItemR,self.max_step - 1)
            end
            self:Move(self.VirItemR,speed,dir,self.max_step - 1,self.max_step,nil,true)
            vir_can_move = false
        elseif step == self.max_step -1 and next_step == self.max_step and vir_can_move == true then
            if self.cur_ctrl_i ~= i then
                self.cur_ctrl_i = i
                self:SetObjStepState(self.VirItemL,1)
            end
            self.VirItemL:SetData(self.CardList[i]:GetData())
            self:Move(self.VirItemL,speed,dir,1,2,nil,true)
            vir_can_move = false
        elseif step == self.max_step and next_step == self.max_step - 1 and vir_can_move == true then
            if self.cur_ctrl_i ~= i then
                self.cur_ctrl_i = i
                self:SetObjStepState(self.VirItemL,2)
            end
            self:Move(self.VirItemL,speed,dir,2,1,nil,true)
            vir_can_move = false
        end

        if self:Move(self.CardList[i],speed,dir,step,next_step,pos) == true then
            if is_ret == true then
                is_back = true
            end
        end
    end

    if is_back == true then
        if callback_func then
            callback_func()
        end
        return true
    end
end

function LuckyMazePanel:Move(card,speed,dir,step,next_step,card_pos,is_vir)
    local step_data = self.card_trans_data[step]
    local next_step_data = self.card_trans_data[next_step]
    local dv = math.abs(step_data.x - next_step_data.x) / speed * dir
    card_pos = card_pos or card.gameObject:GetLocalPosition()
    if next_step_data.x > step_data.x then
        card_pos.x = card_pos.x < step_data.x and step_data.x or card_pos.x
        if (card_pos.x + dv) >= next_step_data.x then
            if not is_vir then
                card.gameObject.transform:SetSiblingIndex(next_step_data.child_index)
            else
                card.gameObject.transform:SetSiblingIndex(0)
            end
            card_pos.x = next_step_data.x
        else
            card_pos.x = card_pos.x + dv
        end
    else
        card_pos.x = card_pos.x > step_data.x and step_data.x or card_pos.x
        if (card_pos.x + dv) <= next_step_data.x then
            if not is_vir then
                card.gameObject.transform:SetSiblingIndex(next_step_data.child_index)
            else
                card.gameObject.transform:SetSiblingIndex(0)
            end
            card_pos.x = next_step_data.x
        else
            card_pos.x = card_pos.x + dv
        end
    end
    card.gameObject:SetLocalPosition(card_pos)

    -- 中心点判断 -- 这个111是卡牌宽度的一半
    --card:CheckGrayMask()  -- 放这里原因是让他下一帧检查中心点
    local center_x = self.card_trans_data[(self.max_step + 1)/2].x
    if math.abs(card_pos.x - center_x) < 111 then
        self.puzzle_img_id = card:GetData().img_id
        LuckyMazeData.Instance.puzzle_index = self.puzzle_img_id
        self.end_dir = card_pos.x - center_x > 0 and -1 or 1 -- 最后放手时，item回弹移动的方向
    end
    self:CheckAllGaryMask()
    
    -- 控制比例
    local s_dir = next_step_data.scale.x - step_data.scale.x > 0 and 1 or -1
    local s_dv = math.abs(next_step_data.scale.x - step_data.scale.x) / speed * s_dir
    local card_scale = card.CardTrans.localScale
    if next_step_data.scale.x > step_data.scale.x then
        card_scale.x = card_scale.x < step_data.scale.x and step_data.scale.x or card_scale.x
        if (card_scale.x + s_dv) > next_step_data.scale.x then
            card_scale.x = next_step_data.scale.x
        else
            card_scale.x = card_scale.x + s_dv
        end
    else
        card_scale.x = card_scale.x > step_data.scale.x and step_data.scale.x or card_scale.x
        if (card_scale.x + s_dv) < next_step_data.scale.x then
            card_scale.x = next_step_data.scale.x
        else
            card_scale.x = card_scale.x + s_dv
        end
    end
    card.CardTrans.localScale = Vector3.New(card_scale.x,card_scale.x,1)

    -- 中间那张显示的牌是否到中心
    if card_pos.x == center_x then
        return true
    else
        return false
    end
end

function LuckyMazePanel:SetObjStepState(card,step,is_all_handle,data)
    card.gameObject:SetLocalPosition(self.card_trans_data[step].pos)
    UH.LocalScale(card.CardTrans,self.card_trans_data[step].scale)
    if is_all_handle then
        card.gameObject.transform:SetAsLastSibling()
    end
    if data then
        card:SetData(data)
    end
end

function LuckyMazePanel:CheckAllGaryMask()
    for i = 1,self.CardList:Length() do
        self.CardList[i]:CheckGrayMask()
    end
end

function LuckyMazePanel:GetMoveDV()
    local mouse_pos_x = UnityEngine.Input.mousePosition.x
    if self.last_move_x == nil then
        self.last_move_x = mouse_pos_x
        return 0,0
    else
        local dv = self.last_move_x - mouse_pos_x
        self.last_move_x = mouse_pos_x
        dv = -dv * self.drag_speed
        local speed = 100 - math.floor(math.abs(dv))
        if not self.anima_ing then
            speed = speed < self.max_speed and self.max_speed or speed
        else
            speed = speed < 40 and 40 or speed
        end
        local dir = dv > 0 and 1 or -1
        return dir,speed
    end
end

-- 通过位置判断对象移动到哪个阶段了
function LuckyMazePanel:GetStep(pos_x,dv)
    if dv == nil or dv == 0 then
        return
    end
    local dir = dv > 0 and 1 or -1
    local i = dv > 0 and 1 or #self.card_trans_data
    local len = dv > 0 and #self.card_trans_data or 1
    for i = i,len,dir do
        if i > 1 and i <= #self.card_trans_data - 1 then
            if dir > 0 then
                if pos_x >= self.card_trans_data[i].x and pos_x < self.card_trans_data[i + dir].x then
                    return i,i + dir
                end
            else
                if pos_x <= self.card_trans_data[i].x and pos_x > self.card_trans_data[i + dir].x then
                    return i,i + dir
                end
            end
        elseif i == #self.card_trans_data then
            if dir > 0 then
                if pos_x >= self.card_trans_data[i].x and -pos_x <= self.card_trans_data[1].x then--pos_x < self.card_trans_data[i].x + math.abs(self.card_trans_data[1].x) - 480 then
                    return i,1
                end
            else
                if pos_x <= self.card_trans_data[i].x and pos_x > self.card_trans_data[i - 1].x then
                    return i,i - 1
                end
            end
        elseif i == 1 then
            if dir > 0 then
                if pos_x >= self.card_trans_data[i].x and pos_x < self.card_trans_data[i + 1].x then
                    return i,i + 1
                end
            else
                if pos_x <= self.card_trans_data[i].x and pos_x <= -self.card_trans_data[self.max_step].x then
                    return i,self.max_step
                end
            end
        end
    end
end

function LuckyMazePanel:OnAddCountClick()
    GMCmdCtrl.Instance:SendGMCommand("ramaze",1)
end

function LuckyMazePanel:OnClickGetItem()
    if PlanActivityCtrl.Instance:ActGetWayViewById(ACTIVITY_TYPE.RAND_LUCKY_MAZE) then
        ViewMgr:CloseView(LuckyMazeView)
    end
end

LuckyMazeTaskItem = LuckyMazeTaskItem or DeclareMono("LuckyMazeTaskItem",UIWidgetBaseItem)
function LuckyMazeTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.gameObject.name = string.format("TaskItem%s",data.seq1 + 1)
    local desc_str = data.describe
    local count_str = string.format("(%s/%s)",data.count,data.parameter)
    if data.flag == 1 then
        desc_str = ColorStr(desc_str,COLORSTR.Green4)
        count_str = ColorStr(count_str,COLORSTR.Green4)
    else
        desc_str = ColorStr(desc_str,COLORSTR.Yellow8)
        count_str = ColorStr(count_str,COLORSTR.Yellow8)
    end
    UH.SetText(self.Desc,desc_str)
    UH.SetText(self.Count,count_str)
end

LuckyMazeCardItem = LuckyMazeCardItem or DeclareMono("LuckyMazeCardItem",UIWidgetBaseItem)
function LuckyMazeCardItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.cell_data = data.child_data
    self:CheckPass(true)
    self.ChildList:SetData(data.child_data)
    self:CheckGrayMask()
end

function LuckyMazeCardItem:CheckGrayMask()
    self.GrayMaskObj:SetActive(self.data.img_id ~= LuckyMazeData.Instance.puzzle_index) --and not LuckyMazeData.Instance:IsAllFinish())
    if self.data.img_id == LuckyMazeData.Instance.puzzle_index then
        return self
    end
end

function LuckyMazeCardItem:OnDestroy()
    TimeHelper:CancelTimer(self.move_ht)
end

function LuckyMazeCardItem:OnChildClick(data,item_obj)
    if LuckyMazeData.Instance:GetPuzzleChildFull(self.data.img_id) == 0 then
        self.ChildList:SelectAll(false)
        PublicPopupCtrl.Instance:Error(Language.LuckyMaze.PuzzleCtrlTip)
        return
    end
    if data.num == 0 then
        self.ChildList:SelectAll(false)
        return
    end
    local select_data = self.ChildList:GetSelectedData()
    if #select_data == 1 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
        self.item_num = data.num
        self.item_index = data.pos_index
        self.ElementA.gameObject:SetLocalPosition(item_obj:GetLocalPosition())
        self.ElementA:SetData(data)
        self.a_icon_obj = self.ChildList:GetItem(data.pos_index).Icon.gameObject
    else
        self.ChildList:SelectAll(false)
        if not PuzzleData.Instance:IsAdjoin(self.item_index,data.pos_index) then
            if self.item_index ~= data.index then
                PublicPopupCtrl.Instance:Error(Language.Anecdote.PuzzleSelectTip)
            else
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
            end
            return
        end
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        -- 交换数据
        local a_data = TableCopy(self.cell_data[data.pos_index])
        a_data.pos_index = self.item_index
        local b_data = TableCopy(self.cell_data[self.item_index])
        b_data.pos_index = data.pos_index
        self.cell_data[self.item_index] = a_data
        self.cell_data[data.pos_index] = b_data
        self.ElementB.gameObject:SetLocalPosition(item_obj:GetLocalPosition())
        self.ElementB:SetData(data)
        self.b_icon_obj = self.ChildList:GetItem(data.pos_index).Icon.gameObject
        self.MaskObj:SetActive(true)
        self.move_ht = PuzzleData.Instance:PlayPuzzleMoveAnima(self.a_icon_obj,self.b_icon_obj,self.ElementA.gameObject,self.ElementB.gameObject,function()
            self.MaskObj:SetActive(false)
            self.ChildList:SelectAll(false)
            self.ChildList:SetData(self.cell_data)
            LuckyMazeView.SendReq(LuckyMazeData.CtrlType.Play,self.data.img_id,a_data.pos_index - 1,b_data.pos_index - 1)
            self:CheckPass(nil,true)
        end)
    end
end
-- 拼图检测
function LuckyMazeCardItem:CheckPass(is_remove_line,is_finish_effect)
    if PuzzleData.Instance:IsPass(self.cell_data) then
        self.MaskObj:SetActive(true)
        if is_remove_line then
            self.Line2:SetActive(false)
        end
        if is_finish_effect then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PetGain)
            self.FinishEffectObj:SetActive(true)
        end
    else
        self.FinishEffectObj:SetActive(false)
        self.MaskObj:SetActive(false)
        self.Line2:SetActive(true)
    end
end

LuckyMazeCardChildItem = LuckyMazeCardChildItem or DeclareMono("LuckyMazeCardChildItem",UIWidgetBaseItem)
-- data = {num,pos_index,img_name,img_id}
function LuckyMazeCardChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.Icon:SetObjActive(data.num > 0)
    local cur_puzzle_id = LuckyMazeData.Instance:GetPuzzleIndex()
    local change_index = LuckyMazeData.Instance:GetChangeIndex()
    self.IconBack:SetObjActive(data.num <= 0 or (data.img_id == cur_puzzle_id and change_index == data.pos_index))
    if data.num > 0 then
        UH.SpriteName(self.Icon,string.format("%s%s",data.img_name,data.num))
    end
    if not self.first_set_card_back then
        self.first_set_card_back = true
        UH.SpriteName(self.IconBack,string.format("KaBei_0%s",data.pos_index))
    end
    if data.img_id == cur_puzzle_id and change_index == data.pos_index then
        self:PlayAnima()
    end
end

-- 拼图碎片翻转动画
function LuckyMazeCardChildItem:PlayAnima()
    self.Anima:Play()
    LuckyMazeData.Instance:SetChangeIndex(nil,0)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
end

