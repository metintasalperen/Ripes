#pragma once

#include "../riscv.h"

#include "VSRTL/core/vsrtl_component.h"
#include "../rv_alu.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class RV5S_ALU : public ALU {
public:
    SetGraphicsType(ALU);
    RV5S_ALU(std::string name, SimComponent* parent) : ALU(name, parent) {
        

        // Zero = 1
        // Non-zero = 0
        zero_flag << [=] {
            switch (ctrl.uValue()) {
                case ALUOp::ADD:
                    return ((op1.uValue() + op2.uValue()) == 0 ? 1 : 0);
                case ALUOp::SUB:
                    return ((op1.uValue() - op2.uValue()) == 0 ? 1 : 0);
                case ALUOp::MUL:
                    return ((static_cast<uint32_t>(op1.sValue() * op2.sValue())) == 0 ? 1 : 0);
                case ALUOp::MULH: {
                    const auto res = static_cast<int64_t>(op1.sValue()) * static_cast<int64_t>(op2.sValue());
                    return ((static_cast<uint32_t>(res >> 32)) == 0 ? 1 : 0);
                }
                case ALUOp::MULHU: {
                    const auto res = static_cast<uint64_t>(op1.uValue()) * static_cast<uint64_t>(op2.uValue());
                    return ((static_cast<uint32_t>(res >> 32)) == 0 ? 1 : 0);
                }
                case ALUOp::MULHSU: {
                    const int64_t res = static_cast<int64_t>(op1.sValue()) * static_cast<uint64_t>(op2.uValue());
                    return ((static_cast<uint32_t>(res >> 32)) == 0 ? 1 : 0);
                }
                case ALUOp::DIV:
                    if (op2.sValue() == 0) {
                        return ((static_cast<uint32_t>(-1)) == 0 ? 1 : 0);
                    } else if (op1.sValue() == static_cast<int64_t>(-(std::pow(2, 32 - 1))) && op2.sValue() == -1) {
                        // Overflow
                        return ((static_cast<uint32_t>(0x80000000)) == 0 ? 1 : 0);  // -2^(32-1)
                    } else {
                        return ((static_cast<uint32_t>(op1.sValue() / op2.sValue())) == 0 ? 1 : 0);
                    }

                case ALUOp::DIVU:
                    if (op2.sValue() == 0) {
                        return ((static_cast<uint32_t>(std::pow(2, 32) - 1)) == 0 ? 1 : 0);
                    } else {
                        return ((op1.uValue() / op2.uValue()) == 0 ? 1 : 0);
                    }

                case ALUOp::REM:
                    if (op2.sValue() == 0) {
                        return ((op1.uValue()) == 0 ? 1 : 0);
                    } else if (op1.sValue() == static_cast<int64_t>(-(std::pow(2, 32 - 1))) && op2.sValue() == -1) {
                        // Overflow
                        return ((static_cast<uint32_t>(0)) == 0 ? 1 : 0);
                    } else {
                        return ((static_cast<uint32_t>(op1.sValue() % op2.sValue())) == 0 ? 1 : 0);
                    }

                case ALUOp::REMU:
                    if (op2.uValue() == 0) {
                        return ((op1.uValue()) == 0 ? 1 : 0);
                    } else {
                        return ((op1.uValue() % op2.uValue()) == 0 ? 1 : 0);
                    }

                case ALUOp::AND:
                    return ((op1.uValue() & op2.uValue()) == 0 ? 1 : 0);

                case ALUOp::OR:
                    return ((op1.uValue() | op2.uValue()) == 0 ? 1 : 0);

                case ALUOp::XOR:
                    return ((op1.uValue() ^ op2.uValue()) == 0 ? 1 : 0);

                case ALUOp::SL:
                    return ((op1.uValue() << op2.uValue()) == 0 ? 1 : 0);

                case ALUOp::SRA:
                    return ((static_cast<uint32_t>(op1.sValue() >> op2.uValue())) == 0 ? 1 : 0);

                case ALUOp::SRL:
                    return ((op1.uValue() >> op2.uValue()) == 0 ? 1 : 0);

                case ALUOp::LUI:
                    return ((op2.uValue()) == 0 ? 1 : 0);

                case ALUOp::LT:
                    return ((op1.sValue() - op2.sValue()) == 0 ? 1 : 0);

                case ALUOp::LTU:
                    return ((op1.uValue() - op2.uValue()) == 0 ? 1 : 0);

                case ALUOp::NOP:
                    return 0;

                default:
                    throw std::runtime_error("Invalid ALU opcode");
            }
        };

        // Negative = 1
        // Positive = 0
        sign_flag << [=] {
            // int sign_bit = (result >> 31) & 1;
            switch (ctrl.uValue()) {
                case ALUOp::ADD:
                    return (((op1.uValue() + op2.uValue()) >> 31) & 1);
                case ALUOp::SUB:
                    return (((op1.uValue() - op2.uValue()) >> 31) & 1);
                case ALUOp::MUL:
                    return (((static_cast<uint32_t>(op1.sValue() * op2.sValue())) >> 31) & 1);
                case ALUOp::MULH: {
                    const auto res = static_cast<int64_t>(op1.sValue()) * static_cast<int64_t>(op2.sValue());
                    return (((static_cast<uint32_t>(res >> 32)) >> 31) & 1);
                }
                case ALUOp::MULHU: {
                    const auto res = static_cast<uint64_t>(op1.uValue()) * static_cast<uint64_t>(op2.uValue());
                    return (((static_cast<uint32_t>(res >> 32)) >> 31) & 1);
                }
                case ALUOp::MULHSU: {
                    const int64_t res = static_cast<int64_t>(op1.sValue()) * static_cast<uint64_t>(op2.uValue());
                    return (((static_cast<uint32_t>(res >> 32)) >> 31) & 1);
                }
                case ALUOp::DIV:
                    if (op2.sValue() == 0) {
                        return (((static_cast<uint32_t>(-1)) >> 31) & 1);
                    } else if (op1.sValue() == static_cast<int64_t>(-(std::pow(2, 32 - 1))) && op2.sValue() == -1) {
                        // Overflow
                        return (((static_cast<uint32_t>(0x80000000)) >> 31) & 1);  // -2^(32-1)
                    } else {
                        return (((static_cast<uint32_t>(op1.sValue() / op2.sValue())) >> 31) & 1);
                    }

                case ALUOp::DIVU:
                    if (op2.sValue() == 0) {
                        return (((static_cast<uint32_t>(std::pow(2, 32) - 1)) >> 31) & 1);
                    } else {
                        return (((op1.uValue() / op2.uValue()) >> 31) & 1);
                    }

                case ALUOp::REM:
                    if (op2.sValue() == 0) {
                        return (((op1.uValue()) >> 31) & 1);
                    } else if (op1.sValue() == static_cast<int64_t>(-(std::pow(2, 32 - 1))) && op2.sValue() == -1) {
                        // Overflow
                        return (((static_cast<uint32_t>(0)) >> 31) & 1);
                    } else {
                        return (((static_cast<uint32_t>(op1.sValue() % op2.sValue())) >> 31) & 1);
                    }

                case ALUOp::REMU:
                    if (op2.uValue() == 0) {
                        return (((op1.uValue()) >> 31) & 1);
                    } else {
                        return (((op1.uValue() % op2.uValue()) >> 31) & 1);
                    }

                case ALUOp::AND:
                    return (((op1.uValue() & op2.uValue()) >> 31) & 1);

                case ALUOp::OR:
                    return (((op1.uValue() | op2.uValue()) >> 31) & 1);

                case ALUOp::XOR:
                    return (((op1.uValue() ^ op2.uValue()) >> 31) & 1);

                case ALUOp::SL:
                    return (((op1.uValue() << op2.uValue()) >> 31) & 1);

                case ALUOp::SRA:
                    return (((static_cast<uint32_t>(op1.sValue() >> op2.uValue())) >> 31) & 1);

                case ALUOp::SRL:
                    return (((op1.uValue() >> op2.uValue()) >> 31) & 1);

                case ALUOp::LUI:
                    return (((op2.uValue()) >> 31) & 1);

                case ALUOp::LT:
                    return static_cast<uint32_t>(((op1.sValue() - op2.sValue()) >> 31) & 1);

                case ALUOp::LTU:
                    return (((op1.uValue() - op2.uValue()) >> 31) & 1);

                case ALUOp::NOP:
                    return 0U;

                default:
                    throw std::runtime_error("Invalid ALU opcode");
            }
        };

        carry_flag << [=] {
            switch (ctrl.uValue()) {
                case ALUOp::ADD: {
                    int64_t o1 = op1.uValue();
                    int64_t o2 = op2.uValue();
                    int64_t r = o1 + o2;
                    if (r > MAX_UINT_32) {
                        return 1;
                    }
                    return 0;
                    // bool x = o1 <= (o2 * -1);
                    /*int res = ((o1 <= (o2 * -1)) ? 1 : 0);
                    return res;*/
                    /*auto result = op1.uValue() + op2.uValue();
                    return ((result < op1.uValue() || result < op2.uValue()) ? 1 : 0);*/
                }
                case ALUOp::SUB:
                    return ((op1.uValue() <= op2.uValue()) ? 1 : 0);
                /*case ALUOp::MUL:
                    return static_cast<uint32_t>(op1.sValue() * op2.sValue());
                case ALUOp::MULH: {
                    const auto res = static_cast<int64_t>(op1.sValue()) * static_cast<int64_t>(op2.sValue());
                    return static_cast<uint32_t>(res >> 32);
                }
                case ALUOp::MULHU: {
                    const auto res = static_cast<uint64_t>(op1.uValue()) * static_cast<uint64_t>(op2.uValue());
                    return static_cast<uint32_t>(res >> 32);
                }
                case ALUOp::MULHSU: {
                    const int64_t res = static_cast<int64_t>(op1.sValue()) * static_cast<uint64_t>(op2.uValue());
                    return static_cast<uint32_t>(res >> 32);
                }
                case ALUOp::DIV:
                    if (op2.sValue() == 0) {
                        return static_cast<uint32_t>(-1);
                    } else if (op1.sValue() == static_cast<int64_t>(-(std::pow(2, 32 - 1))) && op2.sValue() == -1) {
                        // Overflow
                        return static_cast<uint32_t>(0x80000000);  // -2^(32-1)
                    } else {
                        return static_cast<uint32_t>(op1.sValue() / op2.sValue());
                    }

                case ALUOp::DIVU:
                    if (op2.sValue() == 0) {
                        return static_cast<uint32_t>(std::pow(2, 32) - 1);
                    } else {
                        return op1.uValue() / op2.uValue();
                    }

                case ALUOp::REM:
                    if (op2.sValue() == 0) {
                        return op1.uValue();
                    } else if (op1.sValue() == static_cast<int64_t>(-(std::pow(2, 32 - 1))) && op2.sValue() == -1) {
                        // Overflow
                        return static_cast<uint32_t>(0);
                    } else {
                        return static_cast<uint32_t>(op1.sValue() % op2.sValue());
                    }

                case ALUOp::REMU:
                    if (op2.uValue() == 0) {
                        return op1.uValue();
                    } else {
                        return op1.uValue() % op2.uValue();
                    }*/

                case ALUOp::SL: {
                    return (int)(op1.uValue() << (op2.uValue() - 1) >> 31) & 1;
                }

                case ALUOp::SRA:
                    return (op1.sValue() >> (op2.uValue() - 1)) & 1;
                    // return static_cast<uint32_t>(op1.sValue() >> op2.uValue());

                case ALUOp::SRL:
                    return (int)(op1.uValue() >> (op2.uValue() - 1)) & 1;

                case ALUOp::LT:
                    return (op1.sValue() < op2.sValue() ? 1 : 0);

                case ALUOp::LTU:
                    return (op1.uValue() < op2.uValue() ? 1 : 0);

                default:
                    return 0;
            }
        };

        overflow_flag << [=] { 
            switch (ctrl.uValue()) {
                case ALUOp::ADD: {
                    int64_t o1 = op1.uValue();
                    int64_t o2 = op2.uValue();
                    int64_t res = o1 + o2;
                    unsigned int res_sign_bit = (res >> 63) & 1;
                    unsigned int op1_sign_bit = (op1.uValue() >> 31) & 1;
                    unsigned int op2_sign_bit = (op2.uValue() >> 31) & 1;
                    if ((res_sign_bit == 1 && op1_sign_bit == 0 && op2_sign_bit == 0) ||
                        (res_sign_bit == 0 && op1_sign_bit == 1 && op2_sign_bit == 1)) {
                        return 1;
                    }
                    return 0;
                }
                case ALUOp::SUB: {
                    unsigned int res_sign_bit = ((op1.uValue() - op2.uValue()) >> 31) & 1;
                    unsigned int op1_sign_bit = (op1.uValue() >> 31) & 1;
                    unsigned int op2_sign_bit = (op2.uValue() >> 31) & 1;
                    if ((res_sign_bit == 1 && op1_sign_bit == 0 && op2_sign_bit == 1) ||
                        (res_sign_bit == 0 && op1_sign_bit == 1 && op2_sign_bit == 0)) {
                        return 1;
                    }
                    return 0;
                }
                /*case ALUOp::MUL:
                    return (((static_cast<uint32_t>(op1.sValue() * op2.sValue())) >> 31) & 1);
                case ALUOp::MULH: {
                    const auto res = static_cast<int64_t>(op1.sValue()) * static_cast<int64_t>(op2.sValue());
                    return (((static_cast<uint32_t>(res >> 32)) >> 31) & 1);
                }
                case ALUOp::MULHU: {
                    const auto res = static_cast<uint64_t>(op1.uValue()) * static_cast<uint64_t>(op2.uValue());
                    return (((static_cast<uint32_t>(res >> 32)) >> 31) & 1);
                }
                case ALUOp::MULHSU: {
                    const int64_t res = static_cast<int64_t>(op1.sValue()) * static_cast<uint64_t>(op2.uValue());
                    return (((static_cast<uint32_t>(res >> 32)) >> 31) & 1);
                }
                case ALUOp::DIV:
                    if (op2.sValue() == 0) {
                        return (((static_cast<uint32_t>(-1)) >> 31) & 1);
                    } else if (op1.sValue() == static_cast<int64_t>(-(std::pow(2, 32 - 1))) && op2.sValue() == -1) {
                        // Overflow
                        return (((static_cast<uint32_t>(0x80000000)) >> 31) & 1);  // -2^(32-1)
                    } else {
                        return (((static_cast<uint32_t>(op1.sValue() / op2.sValue())) >> 31) & 1);
                    }

                case ALUOp::DIVU:
                    if (op2.sValue() == 0) {
                        return (((static_cast<uint32_t>(std::pow(2, 32) - 1)) >> 31) & 1);
                    } else {
                        return (((op1.uValue() / op2.uValue()) >> 31) & 1);
                    }

                case ALUOp::REM:
                    if (op2.sValue() == 0) {
                        return (((op1.uValue()) >> 31) & 1);
                    } else if (op1.sValue() == static_cast<int64_t>(-(std::pow(2, 32 - 1))) && op2.sValue() == -1) {
                        // Overflow
                        return (((static_cast<uint32_t>(0)) >> 31) & 1);
                    } else {
                        return (((static_cast<uint32_t>(op1.sValue() % op2.sValue())) >> 31) & 1);
                    }

                case ALUOp::REMU:
                    if (op2.uValue() == 0) {
                        return (((op1.uValue()) >> 31) & 1);
                    } else {
                        return (((op1.uValue() % op2.uValue()) >> 31) & 1);
                    }*/

                /*case ALUOp::AND:
                    return (((op1.uValue() & op2.uValue()) >> 31) & 1);

                case ALUOp::OR:
                    return (((op1.uValue() | op2.uValue()) >> 31) & 1);

                case ALUOp::XOR:
                    return (((op1.uValue() ^ op2.uValue()) >> 31) & 1);*/

                /*case ALUOp::SL:
                    return (((op1.uValue() << op2.uValue()) >> 31) & 1);

                case ALUOp::SRA:
                    return (((static_cast<uint32_t>(op1.sValue() >> op2.uValue())) >> 31) & 1);

                case ALUOp::SRL:
                    return (((op1.uValue() >> op2.uValue()) >> 31) & 1);*/

                case ALUOp::LT: {
                    unsigned int res_sign_bit = ((op1.uValue() - op2.uValue()) >> 31) & 1;
                    unsigned int op1_sign_bit = (op1.uValue() >> 31) & 1;
                    unsigned int op2_sign_bit = (op2.uValue() >> 31) & 1;
                    if ((res_sign_bit == 1 && op1_sign_bit == 0 && op2_sign_bit == 1) ||
                        (res_sign_bit == 0 && op1_sign_bit == 1 && op2_sign_bit == 0)) {
                        return 1;
                    }
                    return 0;
                }

                case ALUOp::LTU: {
                    unsigned int res_sign_bit = ((op1.uValue() - op2.uValue()) >> 31) & 1;
                    unsigned int op1_sign_bit = (op1.uValue() >> 31) & 1;
                    unsigned int op2_sign_bit = (op2.uValue() >> 31) & 1;
                    if ((res_sign_bit == 1 && op1_sign_bit == 0 && op2_sign_bit == 1) ||
                        (res_sign_bit == 0 && op1_sign_bit == 1 && op2_sign_bit == 0)) {
                        return 1;
                    }
                    return 0;
                }

                /*case ALUOp::NOP:
                    return 0U;*/

                default:
                    return 0;
                    //throw std::runtime_error("Invalid ALU opcode");
            }
        };
    }
    
    OUTPUTPORT(zero_flag, 1);
    OUTPUTPORT(sign_flag, 1);
    OUTPUTPORT(carry_flag, 1);
    OUTPUTPORT(overflow_flag, 1);
};
}
}