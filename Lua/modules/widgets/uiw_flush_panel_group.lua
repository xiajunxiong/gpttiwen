UIWFlushPanelGroup = UIWFlushPanelGroup or DeclareMono("UIWFlushPanelGroup",UIWFlushPanel)

local _AF = "_AllFlush"

-----ChildPanels===UIFWFlushPanel[];

function UIWFlushPanelGroup:UIWFlushPanelGroup()
	self.child_map = {}
	self.wait_child_count = 0
	self.fl_key_caches = {}
end

--Setup all child
function UIWFlushPanelGroup:Awake()
	UIWFlushPanel.Awake(self)
	if self.ChildPanels ~= nil then
		for i=1, self.ChildPanels:Length(),1 do
			local ch = self.ChildPanels[i]
			local k = ch:GetKey()
			if k ~= nil then
				self.child_map[k] = ch
			else
				self.wait_child_count  = self.wait_child_count + 1
				Runner.Instance:RunUntilTrue(
						function ()
							if self:Destroyed() then
								return true
							end
							if ch:IsLoaded() then
								local ch_key = ch:GetKey()
								self.child_map[ch_key] = ch
								self.wait_child_count = self.wait_child_count - 1

								if self.fl_key_caches[_AF] or self.fl_key_caches[ch_key] then
									ch:Flush()
								end
								if self.wait_child_count == 0 then
									self.fl_key_caches = nil
								end
								return true
							else
								return false
							end
						end
					)
			end
		end
	end
end

function UIWFlushPanelGroup:Flush(fl_key)

	if StringIsEmpty(fl_key) then
		if self.wait_child_count > 0 then
			self.fl_key_caches[_AF] = true
		end		
		for k,v in pairs(self.child_map) do
			v:Flush()
		end 
		UIWFlushPanel.Flush(self)
	else
		local pos = string.find(fl_key, '/');
		local ch_key = fl_key
		local ch_val = nil
		if pos ~= nil then
			ch_key = string.sub(fl_key,1,pos-1)
			ch_val = string.sub(fl_key,pos + 1,-1)
		end
		local ch = self.child_map[ch_key]
		if ch ~= nil then
			ch:Flush(ch_val)
		elseif self.wait_child_count > 0 and not self.fl_key_caches[_AF] then
			self.fl_key_caches[ch_key] = true
		end
	end
end