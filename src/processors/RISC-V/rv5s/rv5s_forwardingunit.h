#pragma once

#include "../riscv.h"

#include "VSRTL/core/vsrtl_component.h"

namespace Ripes {
Enum(ForwardingSrc, IdStage, MemStage, WbStage);
Enum(UnBrFwSrc, IdStage, ExStage, MemStage, WbStage);
}

namespace vsrtl {
namespace core {
using namespace Ripes;

class ForwardingUnit : public Component {
public:
    ForwardingUnit(std::string name, SimComponent* parent) : Component(name, parent) {
        alu_reg1_forwarding_ctrl << [=] {
            const auto idx = id_reg1_idx.uValue();
            if (idx == 0) {
                return ForwardingSrc::IdStage;
            } else if (idx == mem_reg_wr_idx.uValue() && mem_reg_wr_en.uValue()) {
                return ForwardingSrc::MemStage;
            } else if (idx == wb_reg_wr_idx.uValue() && wb_reg_wr_en.uValue()) {
                return ForwardingSrc::WbStage;
            } else {
                return ForwardingSrc::IdStage;
            }
        };

        alu_reg2_forwarding_ctrl << [=] {
            const auto idx = id_reg2_idx.uValue();
            if (idx == 0) {
                return ForwardingSrc::IdStage;
            } else if (idx == mem_reg_wr_idx.uValue() && mem_reg_wr_en.uValue()) {
                return ForwardingSrc::MemStage;
            } else if (idx == wb_reg_wr_idx.uValue() && wb_reg_wr_en.uValue()) {
                return ForwardingSrc::WbStage;
            } else {
                return ForwardingSrc::IdStage;
            }
        };

        un_br_reg1_ctrl << [=] { 
            const auto idx = decode_reg1_idx.uValue();
            if (idx == 0) {
                return UnBrFwSrc::IdStage;
            } else if (idx == ex_reg_wr_idx.uValue() && ex_reg_wr_en.uValue()) {
                return UnBrFwSrc::ExStage;
            } else if (idx == mem_reg_wr_idx.uValue() && mem_reg_wr_en.uValue()) {
                return UnBrFwSrc::MemStage;
            } else if (idx == wb_reg_wr_idx.uValue() && wb_reg_wr_en.uValue()) {
                return UnBrFwSrc::WbStage;
            } else {
                return UnBrFwSrc::IdStage;
            }
        };

        alures_mem_ctrl << [=] {
            switch (mem_op.uValue()) {
                case MemOp::LB:
                case MemOp::LBU:
                case MemOp::LH:
                case MemOp::LHU:
                case MemOp::LW:
                    return LoadOp::Load;
                default:
                    return LoadOp::Other;
            }
        };
    }

    INPUTPORT(id_reg1_idx, RV_REGS_BITS);
    INPUTPORT(id_reg2_idx, RV_REGS_BITS);

    INPUTPORT(ex_reg_wr_idx, RV_REGS_BITS);
    INPUTPORT(ex_reg_wr_en, 1);

    INPUTPORT(mem_reg_wr_idx, RV_REGS_BITS);
    INPUTPORT(mem_reg_wr_en, 1);

    INPUTPORT(wb_reg_wr_idx, RV_REGS_BITS);
    INPUTPORT(wb_reg_wr_en, 1);

    INPUTPORT(decode_reg1_idx, RV_REGS_BITS);

    INPUTPORT(mem_op, MemOp::width());

    OUTPUTPORT_ENUM(alu_reg1_forwarding_ctrl, ForwardingSrc);
    OUTPUTPORT_ENUM(alu_reg2_forwarding_ctrl, ForwardingSrc);

    OUTPUTPORT_ENUM(un_br_reg1_ctrl, UnBrFwSrc);
    OUTPUTPORT_ENUM(alures_mem_ctrl, LoadOp);
};
}  // namespace core
}  // namespace vsrtl
