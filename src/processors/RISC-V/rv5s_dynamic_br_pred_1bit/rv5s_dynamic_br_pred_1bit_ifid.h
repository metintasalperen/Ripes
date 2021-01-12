#pragma once

#include "VSRTL/core/vsrtl_component.h"
#include "VSRTL/core/vsrtl_register.h"

#include "../riscv.h"
#include "../rv5s_no_fw_hz/rv5s_no_fw_hz_ifid.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class RV5S_DYNAMIC_BR_PRED_1BIT_IFID : public IFID {
public:
    RV5S_DYNAMIC_BR_PRED_1BIT_IFID(std::string name, SimComponent* parent) : IFID(name, parent) {
        CONNECT_REGISTERED_CLEN_INPUT(btt_valid, clear, enable);
    }

    REGISTERED_CLEN_INPUT(btt_valid, 1);
};

}  // namespace core
}  // namespace vsrtl
