#pragma once

#include "../riscv.h"

#include "VSRTL/core/vsrtl_component.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class RV5S_STATIC_BR_PRED_PC_CONTROL : public Component {
public:
    RV5S_STATIC_BR_PRED_PC_CONTROL(std::string name, SimComponent* parent) : Component(name, parent) {
        pc_select << [=] {
            auto vex = valid_ex.uValue();
            auto vif = valid_if.uValue();
            auto vid = valid_id.uValue();
            auto jmp_ex = do_jump_ex.uValue();
            auto jmp_id = do_jump_id.uValue();
            auto cond_br_dec = br_decision.uValue();
            auto br_taken = br_decision.uValue() | br_with_flag_decision.uValue();
            if (valid_ex.uValue() == 0 && valid_if.uValue() == 0 && br_taken == 0 &&
                do_jump_id.uValue() == 0) {
                return PcSelect::PC4_IF;
            }
            if (valid_ex.uValue() == 0 && valid_if.uValue() == 0 && br_taken == 1 &&
                do_jump_id.uValue() == 0) {
                return PcSelect::ALU;
            }
            if (valid_ex.uValue() == 0 && valid_if.uValue() == 1 && br_taken == 0 &&
                do_jump_id.uValue() == 0) {
                return PcSelect::BTT;
            }
            if (valid_ex.uValue() == 0 && valid_if.uValue() == 1 && br_taken == 1 &&
                do_jump_id.uValue() == 0) {
                return PcSelect::ALU;
            }
            if (valid_ex.uValue() == 1 && valid_if.uValue() == 0 && br_taken == 0 &&
                do_jump_id.uValue() == 0) {
                return PcSelect::PC4_EX;
            }
            if (valid_ex.uValue() == 1 && valid_if.uValue() == 0 && br_taken == 1 &&
                do_jump_id.uValue() == 0) {
                return PcSelect::PC4_IF;
            }
            if (valid_ex.uValue() == 1 && valid_if.uValue() == 1 && br_taken == 0 &&
                do_jump_id.uValue() == 0) {
                return PcSelect::PC4_EX;
            }
            if (valid_ex.uValue() == 1 && valid_if.uValue() == 1 && br_taken == 1 &&
                do_jump_id.uValue() == 0) {
                return PcSelect::BTT;
            }
            if (valid_id.uValue() == 0 && do_jump_id.uValue() == 1) {
                return PcSelect::ALU;
            }
            if (valid_id.uValue() == 1 && do_jump_id.uValue() == 1) {
                return PcSelect::PC4_IF;
            }
        };
    }
    INPUTPORT(valid_ex, 1);
    INPUTPORT(valid_if, 1);
    INPUTPORT(valid_id, 1);
    INPUTPORT(do_jump_id, 1);
    INPUTPORT(do_jump_ex, 1);
    INPUTPORT(br_decision, 1);
    INPUTPORT(br_with_flag_decision, 1);

    OUTPUTPORT_ENUM(pc_select, PcSelect);
};
}  // namespace core
}  // namespace vsrtl
