#pragma once

#include "../riscv.h"

#include "VSRTL/core/vsrtl_component.h"
#include "../rv_control.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class RV5S_STATIC_BR_PRED_CONTROL : public Control {
public:
    RV5S_STATIC_BR_PRED_CONTROL(std::string name, SimComponent* parent) : Control(name, parent) {
        // clang-format off
        comp_flag_ctrl << [=] {
            switch(opcode.uValue()){
                case RVInstr::BLTF: return CompFlagOp::LT;
                case RVInstr::BGTF: return CompFlagOp::GT;
                case RVInstr::BLEF: return CompFlagOp::LTE;
                case RVInstr::BGEF: return CompFlagOp::GTE;
                case RVInstr::BEQF: return CompFlagOp::EQ;
                case RVInstr::BNEF: return CompFlagOp::NE;
                case RVInstr::BLOF: return CompFlagOp::LTU;
                case RVInstr::BHIF: return CompFlagOp::GTU;
                case RVInstr::BLSF: return CompFlagOp::LTEU;
                case RVInstr::BHSF: return CompFlagOp::GTEU;
                default: return CompFlagOp::NOP;
            }
        };

        do_br_with_flag << [=] {
            switch(opcode.uValue()) {
                case RVInstr::BLTF: case RVInstr::BGTF: case RVInstr::BLEF: case RVInstr::BGEF: case RVInstr::BEQF: case RVInstr::BNEF:
                case RVInstr::BLOF: case RVInstr::BHIF: case RVInstr::BLSF: case RVInstr::BHSF:
                    return 1;
                default: return 0;
            }
        };

        do_jal << [=] {
            switch(opcode.uValue()){
                case RVInstr::JAL:
                    return 1;
                default:
                    return 0;
            }
        };
        // clang-format on
    }

    OUTPUTPORT_ENUM(comp_flag_ctrl, CompFlagOp);
    OUTPUTPORT(do_br_with_flag, 1);
    OUTPUTPORT(do_jal, 1);
};
}
}