#pragma once

#include "../riscv.h"

#include "VSRTL/core/vsrtl_component.h"
#include "../rv_control.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class RV5S_CONTROL : public Control {
public:
    RV5S_CONTROL(std::string name, SimComponent* parent) : Control(name, parent) {
        // clang-format off
        do_jalr << [=] {
            switch (opcode.uValue()) {
                case RVInstr::JALR:
                    return 1;
                default:
                    return 0;
            }
        };
        // clang-format on
    }

    OUTPUTPORT(do_jalr, 1);
};
}
}