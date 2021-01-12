#pragma once

#include "VSRTL/core/vsrtl_component.h"
#include "VSRTL/core/vsrtl_constant.h"
#include "VSRTL/core/vsrtl_register.h"

#include "../riscv.h"

#include "../rv5s/rv5s_idex.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

/**
 * @brief The RV5S_IDEX class
 * A specialization of the default IDEX stage separating register utilized by the rv5s_no_fw_hz processor. Storage of register
 * read indices is added, which are required by the forwarding unit.
 */
class RV5S_STATIC_BR_PRED_IDEX : public RV5S_IDEX {
public:
    RV5S_STATIC_BR_PRED_IDEX(std::string name, SimComponent* parent) : RV5S_IDEX(name, parent) {
        CONNECT_REGISTERED_CLEN_INPUT(do_jal, clear, enable);
        CONNECT_REGISTERED_CLEN_INPUT(btt_valid, clear, enable);
    }

    REGISTERED_CLEN_INPUT(do_jal, 1);
    REGISTERED_CLEN_INPUT(btt_valid, 1);
};

}  // namespace core
}  // namespace vsrtl
