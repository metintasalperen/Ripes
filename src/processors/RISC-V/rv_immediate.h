#pragma once

#include "VSRTL/core/vsrtl_component.h"

#include "riscv.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class Immediate : public Component {
public:
    Immediate(std::string name, SimComponent* parent) : Component(name, parent) {
        imm << [=] {
            Switch(opcode, RVInstr) {
                case RVInstr::LUI:
                case RVInstr::AUIPC:
                    return instr.uValue() & 0xfffff000;
                case RVInstr::JAL: {
                    const auto fields = RVInstrParser::getParser()->decodeJInstr(instr.uValue());
                    return static_cast<unsigned>(
                        signextend<int32_t, 21>(fields[0] << 20 | fields[1] << 1 | fields[2] << 11 | fields[3] << 12));
                }
                case RVInstr::JALR: {
                    return static_cast<unsigned>(signextend<int32_t, 12>((instr.uValue() >> 20)));
                }
                case RVInstr::BEQ:
                case RVInstr::BNE:
                case RVInstr::BLT:
                case RVInstr::BGE:
                case RVInstr::BLTU:
                case RVInstr::BGEU: {
                    const auto fields = RVInstrParser::getParser()->decodeBInstr(instr.uValue());
                    return static_cast<unsigned>(signextend<int32_t, 13>((fields[0] << 12) | (fields[1] << 5) |
                                                                         (fields[5] << 1) | (fields[6] << 11)));
                }
                case RVInstr::LB:
                case RVInstr::LH:
                case RVInstr::LW:
                case RVInstr::LBU:
                case RVInstr::LHU:
                case RVInstr::ADDI:
                case RVInstr::SLTI:
                case RVInstr::SLTIU:
                case RVInstr::XORI:
                case RVInstr::ORI:
                case RVInstr::ANDI:
                case RVInstr::SLLI:
                case RVInstr::SRLI:
                case RVInstr::SRAI:
                    return static_cast<unsigned>(signextend<int32_t, 12>((instr.uValue() >> 20)));
                case RVInstr::SB:
                case RVInstr::SH:
                case RVInstr::SW: {
                    return static_cast<unsigned>(signextend<int32_t, 12>(((instr.uValue() & 0xfe000000)) >> 20) |
                                                 ((instr.uValue() & 0xf80) >> 7));
                }
                case RVInstr::BLTF:
                case RVInstr::BGTF:
                case RVInstr::BLEF:
                case RVInstr::BGEF:
                case RVInstr::BEQF:
                case RVInstr::BNEF:
                case RVInstr::BLOF:
                case RVInstr::BHIF:
                case RVInstr::BLSF:
                case RVInstr::BHSF: {
                    /*unsigned int imm = ((instr.uValue() & 0b00000000001111111000000000000000) >> 10 |
                                ((instr.uValue() & 0b00000000000000000000111110000000) >> 7));*/
                    unsigned int imm_higher = (instr.uValue() & 0b00000000001111111000000000000000) >> 10;
                    unsigned int imm_lower = (instr.uValue() & 0b00000000000000000000111110000000) >> 7;
                    unsigned int sign = (imm_higher >> 11) & 1;
                    unsigned int imm;
                    if (sign == 1) {
                        imm = imm_higher | imm_lower | 0xFFFFF000;
                    } else {
                        imm = imm_higher | imm_lower;
                    }
                    auto res = static_cast<unsigned>(signextend<int32_t, 12>(imm));
                    return res;
                }
                default:
                    return unsigned(0xDEADBEEF);
            }
        };
    }

    INPUTPORT_ENUM(opcode, RVInstr);
    INPUTPORT(instr, RV_REG_WIDTH);
    OUTPUTPORT(imm, RV_REG_WIDTH);
};

}  // namespace core
}  // namespace vsrtl
