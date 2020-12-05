#pragma once

#include "../riscv.h"

#include "VSRTL/core/vsrtl_component.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class BRANCH_WITH_FLAG : public Component {
public:
    BRANCH_WITH_FLAG(std::string name, SimComponent* parent) : Component(name, parent) {
        // clang-format off
        branch_taken << [=] { 
            switch (comp_op.uValue()) {
                case CompFlagOp::EQ: {
                    // Z
                    if (zero_flag.uValue() == 1) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::NE: {
                    // Z'
                    if (zero_flag.uValue() == 0) {
                        return false;
                    }
                    return true;
                }
                case CompFlagOp::LT: {
                    // NO'Z' + N'OZ'
                    if (overflow_flag.uValue() == 0 && sign_flag.uValue() == 1 && zero_flag.uValue() == 0) {
                        return true;
                    }
                    if (overflow_flag.uValue() == 1 && sign_flag.uValue() == 0 && zero_flag.uValue() == 0) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::LTU: {
                    // CZ'
                    if (carry_flag.uValue() == 1 && zero_flag.uValue() == 0) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::LTE: {
                    // Z + NO' + N'O
                    if (zero_flag.uValue() == 1) {
                        return true;
                    }
                    if (overflow_flag.uValue() == 0 && sign_flag.uValue() == 1) {
                        return true;
                    }
                    if (overflow_flag.uValue() == 1 && sign_flag.uValue() == 0) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::LTEU: {
                    // C + Z
                    if (zero_flag.uValue() == 1 || carry_flag.uValue() == 1) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::GT: {
                    // NOZ' + N'O'Z'
                    if (overflow_flag.uValue() == 1 && sign_flag.uValue() == 1 && zero_flag.uValue() == 0) {
                        return true;
                    }
                    if (overflow_flag.uValue() == 0 && sign_flag.uValue() == 0 && zero_flag.uValue() == 0) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::GTU: {
                    // C'Z'
                    if (carry_flag.uValue() == 0 && zero_flag.uValue() == 0) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::GTE: {
                    // Z + NO + N'O'
                    if (zero_flag.uValue() == 1) {
                        return true;
                    }
                    if (overflow_flag.uValue() == 1 && sign_flag.uValue() == 1) {
                        return true;
                    }
                    if (overflow_flag.uValue() == 0 && sign_flag.uValue() == 0) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::GTEU: {
                    // Z + C'
                    if (zero_flag.uValue() == 1) {
                        return true;
                    }
                    if (carry_flag.uValue() == 0) {
                        return true;
                    }
                    return false;
                }
                case CompFlagOp::NOP: {
                    return false;
                }
                default:
                    return false;
            }
        };
        // clang-format on
    }

    INPUTPORT(comp_op, CompFlagOp::width());
    INPUTPORT(zero_flag, 1);
    INPUTPORT(sign_flag, 1);
    INPUTPORT(carry_flag, 1);
    INPUTPORT(overflow_flag, 1);
    OUTPUTPORT(branch_taken, 1);
};
}  // namespace core
}  // namespace vsrtl
