#include "common/memory/mempool.h"

#include "rmibackobjdef.h"
REGISTER_MEMPOOL(gamemempool, RMIGetProfBackObjectImpl, 64, "RMIGetProfBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMILoginToAccountBackObjectImpl, 64, "RMIUserLoginBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMILoginToRoleBackObjectImpl, 64, "RMILoginToRoleBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIAddRoleBackObjectImpl, 64, "RMIAddRoleBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIRemoveRoleBackObjectImpl, 64, "RMIRemoveRoleBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIPassAntiWallowBackObjectImpl, 64, "RMIPassAntiWallowBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIDeleteNameInfoBackObjectImpl, 64, "RMIDeleteNameInfoBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIGetRoleInfoBackObjectImpl, 64, "RMIGetRoleInfoBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIGetRoleInfoBackObjectImplRoleReq, 64, "RMIGetRoleInfoBackObjectImplRoleReq")
REGISTER_MEMPOOL(gamemempool, RMICreateRoleBackObjectImpl, 64, "RMICreateRoleBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIDestroyRoleBackObjectImpl, 64, "RMIDestroyRoleBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIForbidBackObjectImpl, 64, "RMIForbidBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIGetTotalRoleNumBackObjectImpl, 64, "RMIGetTotalRoleNumBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIWriteCmdResultBackObjectImpl, 64, "RMIWriteCmdResultBackObjectImpl")

OLD_USE_INTERNAL_NETWORK();
