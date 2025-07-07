-- 配饰 天命 
AccrssoryDestinyView = AccrssoryDestinyView or DeclareMono("AccrssoryDestinyView", UIWFlushPanel)
function AccrssoryDestinyView:AccrssoryDestinyView()
    self.data = AccrssoryData.Instance
    self.language = Language.Accrssory
    self.data_cares = {
        {data = self.data:GetTianMingInfo(),func = self.FlushDestiny},
        -- {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushDestiny, init_call = false},
    }

    self.pre_view_list.StepCount = 40
    self.pre_view_pos = nil

    self.pre_view_list:SetCompleteCallBack(function()
        if self.pre_view_pos ~= nil then 
            local index = self.pre_view_pos.tianming_level  > 3 and self.pre_view_pos.tianming_level - 3 or self.pre_view_pos.tianming_level
            self.pre_view_list:AutoSlide(self.pre_view_list:GetItem(index):GetData(),false)
        end 
    end)
    -- ObjBlock

    self.attr_str_list:SetCompleteCallBack(function()
        if self.cur_index > 0 then 
            local red_index = self.cur_index > 4 and 5 or 1
            self.attr_str_list:AutoSlide(self.attr_str_list:GetItem(red_index):GetData(),false)
        end 
    end)

    self.cur_index = 0
end

function AccrssoryDestinyView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function AccrssoryDestinyView:onFlush()
    self:FlushDestiny()
end

function AccrssoryDestinyView:FlushDestiny()

    self:FlushInfo()
    self:FlushLightLink()
end

function AccrssoryDestinyView:FlushLightLink()
    local data = self.data:GetTianMingInfo()
    local cur_cfg,next_cfg = self.data:GetTianMingLevelCfg()
    local pic_index = 1
    local index = 1 
    local max_exp = 0
    
    if data.level == 0 then 
        pic_index = next_cfg.tuji
        index = 0
        max_exp = next_cfg.exp
    elseif self.data:IsMaxTianMing() then 
        pic_index = cur_cfg.tuji
        index = 8
        max_exp = cur_cfg.exp
    else 
        pic_index = cur_cfg.tuji
        index = data.level % 8 
        index = (index == 0 and data.level > 0) and 8 or index
        max_exp = next_cfg.exp
    end 

    for i = 1,5 do 
        self.linelinks[i].gameObject:SetActive(pic_index == i)
    end 

    for i = 1,8 do 
        self:SetPointPos(self.level_points[i],pic_index,i)
        -- self.level_points[i].light:SetActive(index >= i)
        -- self.level_points[i].light_light:SetActive(index >= i)
        self.level_points[i]:ActLight(pic_index,index >= i)
        self.level_points[i]:ActLightReady(pic_index,index + 1 >= i)
    end 

    for i = 1, 8 do 
        -- self.level_points[i].line_light:SetActive((self.level_points[i].next_point~= nil and self.level_points[i].next_point.light.activeSelf))
        self.level_points[i]:AutoLinkLinks()
    end 

    self.progress:SetProgress((self.data:IsMaxTianMing() and max_exp or data.exp)/max_exp)
    self.progress:SetText(tostring((self.data:IsMaxTianMing() and max_exp or data.exp)).." / "..tostring(max_exp))
    self.Rend.material:SetFloat("_range", (self.data:IsMaxTianMing() and max_exp or data.exp)/max_exp);
    self.name_obj:SetActive(data.level > 0)
    UH.SetText(self.name,data.level > 0 and cur_cfg.tianming_name or "")
end

function AccrssoryDestinyView:SetPointPos(item,pic_index,index)
    item.gameObject:SetParent(self.linelinks[pic_index].star_list[index].lock_pos,true)
    UH.AnchoredPosition(item.gameObject:GetComponent(typeof(UnityEngine.RectTransform)),Vector2.zero)
    item.gameObject:SetParent(self.point_parent,true)
end

function AccrssoryDestinyView:FlushInfo()
    local data = self.data:GetTianMingInfo()
    local info = self.data:GetCurAttrInfo()

    local str_list = {}
    for i = 1,8 do 
        local str_per = string.format(Language.Accrssory.DestinyAttr[i],math.floor((info.attr_per_list[i] == nil and 0 or info.attr_per_list[i])/100))
        local next_per = (info.next_cfg ~=nil and info.next_cfg.att_type == i) and string.format(Language.Accrssory.DestinyPlusAttr,math.floor(info.next_cfg.att_per_tol/100)) or "" 

        -- UH.SetText(self.add_list[i], str_per..next_per)
        if (info.next_cfg ~=nil and info.next_cfg.att_type == i) then 
            self.cur_index = i
        end 
        local vo = {
            str_per = str_per,
            next_per = next_per,
            index = i,
        }
        table.insert(str_list , vo)
    end 

    self.attr_str_list:SetData(str_list)
end

function AccrssoryDestinyView:OnResolveClick()
    ViewMgr:OpenView(AccrssoryResolveView)

    self.data:MarkResolve()
end

function AccrssoryDestinyView:OnClickOpenPre()
    self.preview:SetActive(true)
    local list,cur_data = self.data:GetTianMingPreViewData()
    self.pre_view_pos = cur_data
    self.pre_view_list:SetData(list)
end

function AccrssoryDestinyView:OnClickClosePre()
    self.preview:SetActive(false)
    self.pre_view_pos = nil
end

function AccrssoryDestinyView:OnClickTurnTianMing(index)
    if self.pre_view_pos == nil then return end 

    local list,cur_data = self.data:GetTianMingPreViewData()
    -- 直接增加一定数量的上下变量
    local pos = self.pre_view_list.ContentPosition.localPosition
    local c_pos = self.pre_view_list.ItemPosition.sizeDelta.y
    local plus = pos.y

    if index > 0 then 
        plus = math.floor(plus)  >= (#list-8) * c_pos and plus or plus + 8 * c_pos
    else 
        plus = plus < 3 and plus or plus - 8 * c_pos
    end 

    self.pre_view_list:ForceJumpVertical(plus)
    -- self.pre_view_list:AutoSlide(self.pre_view_list:GetItem(plus):GetData(),false)
end

-- 等级连接汇总
AccrssoryDestinyLinks = AccrssoryDestinyLinks or DeclareMono("AccrssoryDestinyLinks", UIWidgetBaseItem)
function AccrssoryDestinyLinks:FlushStarCfg()

end 

-- 等级结点
AccrssoryDestinyPointMaster = AccrssoryDestinyPointMaster or DeclareMono("AccrssoryDestinyPointMaster", UIWidgetBaseItem)
function AccrssoryDestinyPointMaster:SetData(data)

end

-- 等级点
AccrssoryDestinyPoint = AccrssoryDestinyPoint or DeclareMono("AccrssoryDestinyPoint", UIWidgetBaseItem)
function AccrssoryDestinyPoint:SetData(data)

end

-- 自动将光条连接到下一个点上
function AccrssoryDestinyPoint:AutoLinkLinks()
    if self.next_point then 
        local pos_self = self.gameObject:GetLocalPosition()
        local pos_tar = self.next_point.gameObject:GetLocalPosition()
    
        -- 三角函数
        local line_x = math.abs(pos_tar.x - pos_self.x)
        local line_y = math.abs(pos_tar.y - pos_self.y)
        local line_light = math.sqrt(line_x*line_x + line_y*line_y)

        -- 检查倾斜方案
        local is_left = (pos_tar.x - pos_self.x) < 0
        local is_bottom = (pos_tar.y - pos_self.y ) < 0
     
        local dir = math.deg(math.asin(line_y/line_light ))
        local dir = is_left and 180 - dir or dir
        local dir = is_bottom and - dir or dir

        UH.LocalEulerAngles(self.line:GetComponent(typeof(UnityEngine.RectTransform)), Vector3(0,0,dir))
        UH.SizeDelta(self.line, Vector3.New(line_light, 8))
    end
end 

function AccrssoryDestinyPoint:ActLight(tuji,flag)
    if not self.EffectLightPoint.isActiveAndEnabled then return end 
    if self.light_point_eff ~= nil then 
        self.EffectLightPoint:Stop(self.light_point_eff)
        
        if self.line_lights ~= nil then 
            for i = 1,5 do 
                self.line_lights[i]:SetActive(false)
            end 
        end 
    end 

    if flag then 
        local eff_id,eff_point_id = AccrssoryData.Instance:GetDenstinyEffectiD(tuji)
        self.light_point_eff = self.EffectLightPoint:Play(eff_point_id)

        if self.next_point ~= nil then 
            for i = 1,5 do 
                self.line_lights[i]:SetActive(i == tuji)
            end  
        end 
    end 
    
end

function AccrssoryDestinyPoint:ActLightReady(tuji,flag)
    if not self.EffectLight.isActiveAndEnabled then return end 
    if self.light_eff ~= nil then 
        self.EffectLight:Stop(self.light_eff)
    end 

    if flag then 
        local eff_id,eff_point_id = AccrssoryData.Instance:GetDenstinyEffectiD(tuji)
        
        self.light_eff = self.EffectLight:Play(eff_id)
    end 
end

AccrssoryDestinyPreViewCell = AccrssoryDestinyPreViewCell or DeclareMono("AccrssoryDestinyPreViewCell", UIWidgetBaseItem)
function AccrssoryDestinyPreViewCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SpriteName(self.bg_sp, "destiny_YuLan"..data.bg+1)
    UH.SetText(self.pre_name, data.str)
end

AccrssoryDestinyAttrViewCell = AccrssoryDestinyAttrViewCell or DeclareMono("AccrssoryDestinyAttrViewCell", UIWidgetBaseItem)
function AccrssoryDestinyAttrViewCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.attr_str,self.data.str_per)
    UH.SetText(self.up_plus,self.data.next_per)
    self.attr_plus:SetActive(self.data.next_per ~= "")

    UH.SpriteName(self.attr_bg, self.data.index % 2 == 0 and "destiny_ShuXingQian" or "destiny_ShuXingShen")
    -- UH.SetText(self.attr_str,self.data.str_per)
end 