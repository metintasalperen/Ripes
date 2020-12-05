#pragma once

#include "VSRTL/core/vsrtl_component.h"
#include "VSRTL/core/vsrtl_register.h"

#include "riscv.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class ALU_FLAG_REG : public Component {
public:
    ALU_FLAG_REG(std::string name, SimComponent* parent) : Component(name, parent) {
        CONNECT_REGISTERED_CLEN_INPUT(sign_flag, clear, enable);
        CONNECT_REGISTERED_CLEN_INPUT(zero_flag, clear, enable);
        CONNECT_REGISTERED_CLEN_INPUT(overflow_flag, clear, enable);
        CONNECT_REGISTERED_CLEN_INPUT(carry_flag, clear, enable);
    }

    REGISTERED_CLEN_INPUT(sign_flag, 1);
    REGISTERED_CLEN_INPUT(zero_flag, 1);
    REGISTERED_CLEN_INPUT(overflow_flag, 1);
    REGISTERED_CLEN_INPUT(carry_flag, 1);

    // Register controls
    INPUTPORT(enable, 1);
    INPUTPORT(clear, 1);
};

}  // namespace core
}  // namespace vsrtl