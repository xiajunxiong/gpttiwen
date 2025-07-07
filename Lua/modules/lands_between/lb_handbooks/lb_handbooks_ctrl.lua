VIEW_REQ("modules/lands_between/lb_handbooks/lb_handbooks_view")

LBHandBooksCtrl = LBHandBooksCtrl or BaseClass(BaseCtrl)
function LBHandBooksCtrl:__init()
	if LBHandBooksCtrl.Instance ~= nil then
		Debuger.LogError("[LBHandBooksCtrl] attempt to create singleton twice!")
		return
	end
	LBHandBooksCtrl.Instance = self

    self.data = LBHandBooksData.New()

    self:RegisterAllProtocols()
end 

function LBHandBooksCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCGodSpaceGuideInfo,"RecvGodSpaceGuideInfo")
end

function LBHandBooksCtrl:RecvGodSpaceGuideInfo(protocol)
    LogDG("聪哥哥！这是神域指南的下发哦！5610",protocol)
    self.data:SetGuideInfo(protocol)
end

function LBHandBooksCtrl:OnInit()

end 

-- 请求已经看过了
function LBHandBooksCtrl:SendClicked(act_id)
    LandsBetweenCtrl.Instance:GodSpaceOperate({
        oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_FINISH_GUIDE_TASK,
        param_1 = act_id or 0,
    })
end

-- 请求奖励
function LBHandBooksCtrl:SendReward(act_id)
    LandsBetweenCtrl.Instance:GodSpaceOperate({
        oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_RECEIVE_GUIDE_REWARD,
        param_1 = act_id or 0,
    })
end

LBHandBooksData = LBHandBooksData or BaseClass()

function LBHandBooksData:__init()
    if LBHandBooksData.Instance ~= nil then
        Debuger.LogDG("[LBHandBooksData] attempt to create singleton twice!")
        return
    end
    LBHandBooksData.Instance = self

    self:InitConfig()

    self.operating = SmartData.New({flush = 0}) -- 操作同步
    self.info_list = SmartData.New({flush = 0}) 
end 

function LBHandBooksData:InitConfig()
    self.desc_cfg = Config.divine_domain_system_auto.shenyu_zhinan
    self.reward_cfg = Config.divine_domain_system_auto.zhinan_reward

    self.fix_desc = {}
    for k,v in pairs(self.desc_cfg) do 
        if self.fix_desc[v.act] == nil then 
            local vo = {}

            vo.act = v.act
            vo.act_title =  v.act_title
            vo.act_icon =  v.paging
            vo.small_title = v.small_title
            vo.paging_list = {}
            vo.reward_list = {}
            for i,j in pairs(self.reward_cfg) do 
                if v.act == j.act then 
                    vo.reward_list = BagData.Instance:ItemListBuild(j.reward_item)
                    
                    local quintessence_num = j.quintessence
                    local item_quintessence = Item.Init(CommonItemId.GodSpaceEssence,quintessence_num)
                    if quintessence_num > 0 then
                        table.insert(vo.reward_list,item_quintessence)
                    end
            
                    local military_exploits_num = j.military_exploits
                    local item_military_exploits = Item.Init(CommonItemId.GodSpaceExploits,military_exploits_num)
                    if military_exploits_num > 0 then
                        table.insert(vo.reward_list,item_military_exploits)
                    end
                end
            end 

           
            self.fix_desc[v.act] = vo
        end 

        local eo = {}
        eo.paging = v.paging
        eo.paging_pic = v.paging_pic
        eo.paging_dec = v.paging_dec
        table.insert(self.fix_desc[v.act].paging_list,eo)
    end 
end 

function LBHandBooksData:GetDescList()
    return self.fix_desc
end

function LBHandBooksData:SetSelect(index)
    self.operating.select = index
    self.operating.flush = self.operating.flush + 1
end

function LBHandBooksData:SetGuideInfo(protocol)
    self.info_list.guide_finish_flag = protocol.guide_finish_flag
    self.info_list.guide_receive_flag = protocol.guide_receive_flag
    self.info_list.flush = self.info_list.flush + 1
end

function LBHandBooksData:CheckIsReaded(act)
    return self.info_list.guide_finish_flag ~= nil and self.info_list.guide_finish_flag[act] == 1
end

function LBHandBooksData:CheckIsRewareded(act)
    return self.info_list.guide_receive_flag ~= nil and  self.info_list.guide_receive_flag[act] == 1
end

