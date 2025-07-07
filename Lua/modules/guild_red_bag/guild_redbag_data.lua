GuildRedbagData = GuildRedbagData or BaseClass()
function GuildRedbagData:__init()
    if GuildRedbagData.Instance ~= nil then
        Debuger.LogError("[GuildRedbagData] attempt to create singleton twice!")
        return
    end
    GuildRedbagData.Instance = self
    self.family_redbag_get_list_info = SmartData.New({count = 0})
    self.family_redbag_send_list_info = SmartData.New({count = 0 ,redbag_send_list = {}})
    self.family_redbag_get_one_info = SmartData.New({})
    self.family_redbag_last_info = {}
    self.family_cur_redbag_info = {}
    self.family_redbag_info_cfg = {}  ---缓存打开的红包
    self.redbag_remove_list = {}
    self:ClearHongBaoGetList()
end
---家族红包--
function GuildRedbagData:SetHongBaoPoint() ----主界面小红包
    if self.family_redbag_get_list_info.count > 0 then 
        if ViewMgr:IsOpen(GuildHongBaoTipView) then  ---避免跨服被隐藏
          GuildHongBaoPanel.Instance.hide.count = 1 
        else
          ViewMgr:OpenView(GuildHongBaoTipView)
        end
    end
end

function GuildRedbagData:SetSendHongBaoType(hongbao_type)
    self.family_cur_redbag_info.send_hongbao_type = hongbao_type
end
function GuildRedbagData:GetSendHongBaoType()
    return self.family_cur_redbag_info.send_hongbao_type
end
function GuildRedbagData:ShowHongBaoDetail(hongbao_id)  --显示红包类型
    self.family_redbag_cur_hong_bao_id = hongbao_id
    self:SetCurGetHongBaoIdInfo(self.family_redbag_info_cfg[hongbao_id])
end

function GuildRedbagData:SetCurGetHongBaoIdInfo(hongbao_info)
    local info = {}
    info.hong_bao_id = hongbao_info.hong_bao_id
    info.send_avatar_type = hongbao_info.send_avatar_type
    info.avatar_id = hongbao_info.avatar_id
    info.avatar_quality = hongbao_info.avatar_quality
    self.family_cur_redbag_info.get_hongbao_info = info
end
function GuildRedbagData:GetCurGetHongBaoInfo()
    return self.family_cur_redbag_info.get_hongbao_info
end

function GuildRedbagData:HongBaoPointNum()
    --[[if LoginData.Instance:IsOnCrossSever() then  --跨服返回false
        return 0
    end--]]
    if self.family_redbag_get_list_info.count > 0 or self.family_redbag_send_list_info.count > 0 then
        return 1
    else
        return 0
    end
end

---是否可以领取及发送
function GuildRedbagData:IsCanUseHongBao() 

     if GuildData.Instance.quit_count ~= 0 then
      local time = GuildData.Instance:GetGuildInfo().join_time
      local tb = {}
      tb.hour = tonumber(os.date("%H",time))
      if TimeCtrl.Instance:GetServerDate().hour - tb.hour > 8 then return true end 
      return false
    end
    return true
  end

--清除
function GuildRedbagData:ClearHongBaoGetList()
    self.family_redbag_get_list_info.count = 0
    self.family_redbag_send_list_info.count = 0
    self.family_redbag_last_info = {}
    self.family_redbag_get_one_info.bag_get_name_list = {}
    self.family_redbag_get_list_info.redbag_get_list = {}
    if ViewMgr:IsOpen(GuildHongBaoTipView) then
         ViewMgr:CloseView(GuildHongBaoTipView)
    end
    if ViewMgr:IsOpen(OpenRedbagView) then  --避免断网重新连接
        ViewMgr:CloseView(OpenRedbagView)
    end
end
---可领取列表
function GuildRedbagData:GetHongBaoGetList()
    return self.family_redbag_get_list_info
end
---可领取列表
function GuildRedbagData:SetHongBaoGetList(protocol)
    self.family_redbag_get_list_info.hongbao_sys_type = protocol.hongbao_sys_type 
    self.family_redbag_get_list_info.count = protocol.count
    self.family_redbag_get_list_info.redbag_get_list = protocol.redbag_get_list
    self:CheckHongBaoData()
    self:SetHongBaoPoint()
    GuildData.Instance.guild_trial_point:Notify()
end
---可发送列表
function GuildRedbagData:GetHongBaoSendList()
    return self.family_redbag_send_list_info
end
function GuildRedbagData:SetHongBaoSendList(protocol)
    self.family_redbag_send_list_info.count = protocol.count 
    self.family_redbag_send_list_info.redbag_send_list = protocol.redbag_send_list
    GuildData.Instance.guild_trial_point:Notify()
end
--单个红包
function GuildRedbagData:SetHongBaoGetOneInfo(protocol)
    protocol.bag_get_name_list = protocol.redbag_get_name_list
    self:HongBaoGetOneInfoCopy(protocol)
end
function GuildRedbagData:HongBaoGetOneInfoCopy(hongbao_info)
    self.family_redbag_get_one_info.hong_bao_id = hongbao_info.hong_bao_id
    self.family_redbag_get_one_info.invalid_timestamp = hongbao_info.invalid_timestamp
    self.family_redbag_get_one_info.hongbao_type = hongbao_info.hongbao_type
    self.family_redbag_get_one_info.hongbao_sys_type = hongbao_info.hongbao_sys_type
    self.family_redbag_get_one_info.send_uid = hongbao_info.send_uid
    self.family_redbag_get_one_info.send_name = hongbao_info.send_name
    self.family_redbag_get_one_info.used_val = hongbao_info.used_val 
    self.family_redbag_get_one_info.get_count = hongbao_info.get_count
    self.family_redbag_get_one_info.get_val = hongbao_info.get_val --玩家自己领取的金额
    self.family_redbag_get_one_info.change_type = hongbao_info.change_type 
    self.family_redbag_get_one_info.bag_get_name_list = hongbao_info.bag_get_name_list or {}
    ---缓存红包数据
    if self.family_redbag_info_cfg[hongbao_info.hong_bao_id] == nil then
        local item = TableCopy(self.family_redbag_get_one_info or {})
        table.insert(self.family_redbag_info_cfg,hongbao_info.hong_bao_id,item)
    end
end

function GuildRedbagData:GetHongBaoGetOneInfo()
    return self.family_redbag_get_one_info
end
----通过id获取当个红包消息
function GuildRedbagData:SetHongBaoGetOneInfoById(hongbao_id)
    self:HongBaoGetOneInfoCopy(self.family_redbag_info_cfg[hongbao_id])
end
---红包变化
--红包可发送列表信息变更
function GuildRedbagData:SetHongBaoSendListChange(protocol)
    local hong_bao_info = {} 
    hong_bao_info.type=protocol.type
    local count = 0
    local remove_index = 1
    local len = 0
    if self.family_redbag_send_list_info.redbag_send_list then
      for i,v in ipairs(self.family_redbag_send_list_info.redbag_send_list) do
          if v.type == protocol.type then
          count = count + 1
          remove_index = i
          end
         len = i
      end
    end
    hong_bao_info.index = remove_index
    if count < protocol.num then  --增加红包
        len = len + 1
        hong_bao_info.index = len
        table.insert(self.family_redbag_send_list_info.redbag_send_list,hong_bao_info)
    else
        len = len - 1
        table.remove(self.family_redbag_send_list_info.redbag_send_list,remove_index)
    end
    self.family_redbag_send_list_info.count = len
    GuildData.Instance.guild_trial_point:Notify()
end

--红包可领取列表信息变更
function GuildRedbagData:SetHongBaoGetListChange(protocol)
    local change_type = protocol.change_type
    if change_type == 1 then  --移除
        if self.family_redbag_info_cfg[protocol.hong_bao_info.hong_bao_id] then
            self.family_redbag_info_cfg[protocol.hong_bao_info.hong_bao_id].change_type = change_type
        end
        self.family_redbag_get_one_info.change_type = change_type
        self:RemoveHongBaoById(protocol.hong_bao_info.hong_bao_id)
    else
        table.insert(self.family_redbag_get_list_info.redbag_get_list,protocol.hong_bao_info)
        self.family_redbag_get_list_info.count = self.family_redbag_get_list_info.count + 1
    end
    self:SetHongBaoPoint()
    GuildData.Instance.guild_trial_point:Notify()
end
function GuildRedbagData:CheckHongBaoData()
    for i,v in ipairs(self.redbag_remove_list) do
        self:RemoveHongBaoById(v.hong_bao_id)     
    end
end
function GuildRedbagData:RemoveHongBaoById(hongbao_id,is_check)
    local re_index = 0
    for i,v in ipairs(self.family_redbag_get_list_info.redbag_get_list) do
         if v.hong_bao_id == hongbao_id then
             re_index = i
             if is_check then
                 table.insert(self.redbag_remove_list,v) --被移除的红包    移除的红包
             end
         end
    end
    if re_index > 0 then
         self.family_redbag_get_list_info.count = self.family_redbag_get_list_info.count - 1
         table.remove(self.family_redbag_get_list_info.redbag_get_list,re_index)
    end
end

--- 详细信息变更  单个红包
function GuildRedbagData:SetHongBaoBaseInfoChange(protocol)
    ---缓存红包数据
    if self.family_redbag_info_cfg[protocol.hong_bao_id] == nil and protocol.change_type == 1 then
        local item = {}
        item.bag_get_name_list = {}
        item.get_val = 0 --自己领取的金额
        table.insert(self.family_redbag_info_cfg,protocol.hong_bao_id,item)
    end 
    ----已经缓存过的才处理
    if self.family_redbag_info_cfg[protocol.hong_bao_id] then  
        self.family_redbag_info_cfg[protocol.hong_bao_id].hong_bao_id = protocol.hong_bao_id 
        self.family_redbag_info_cfg[protocol.hong_bao_id].used_val = protocol.used_val 
        self.family_redbag_info_cfg[protocol.hong_bao_id].change_type = protocol.change_type 
        self.family_redbag_info_cfg[protocol.hong_bao_id].hongbao_type = protocol.hongbao_type 
        if protocol.change_type ~= 1 then  --1红包时间到 0有人领取红包
            table.insert(self.family_redbag_info_cfg[protocol.hong_bao_id].bag_get_name_list,protocol.get_player_info)
            self.family_redbag_info_cfg[protocol.hong_bao_id].get_count = #self.family_redbag_info_cfg[protocol.hong_bao_id].bag_get_name_list
            if protocol.get_player_info.uid == RoleData.Instance:GetRoleId() then  ---玩家领取
                self.family_redbag_info_cfg[protocol.hong_bao_id].get_val = protocol.get_player_info.val --自己领取的金额
                self:RemoveHongBaoById(protocol.hong_bao_id)
                self.family_redbag_get_list_info:Notify()
            end
        end
        if self.family_redbag_get_one_info.hong_bao_id == protocol.hong_bao_id then
             self:SetHongBaoGetOneInfoById(protocol.hong_bao_id)
        end
    end
    GuildData.Instance.guild_trial_point:Notify()
end

function GuildRedbagData:SendRemind()
    if self:GetHongBaoSendList().count ~= nil and self:GetHongBaoSendList().count ~= 0 then
        return 1
    end
    return 0
end

function GuildRedbagData:ReceiveRemind()
    if self:GetHongBaoGetList().count ~= nil and self:GetHongBaoGetList().count ~= 0 then
        return 1
    end
    return 0
end

function GuildRedbagData:AllRemind()
    if self:SendRemind() > 0 then
        return 1
    elseif self:ReceiveRemind() > 0 then
        return 1
    end
    return 0
end