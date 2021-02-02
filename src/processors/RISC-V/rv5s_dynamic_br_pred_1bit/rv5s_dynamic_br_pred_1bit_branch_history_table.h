#pragma once

#include "../riscv.h"

#include "VSRTL/core/vsrtl_component.h"
#include <stack>
namespace vsrtl {
namespace core {
using namespace Ripes;

class Dynamic1BitBranchEntry : public BranchEntry {
public:
    unsigned int prediction_bit;

    Dynamic1BitBranchEntry() : BranchEntry() { 
        prediction_bit = 0;
    }
};

class BRANCH_HISTORY_TABLE_1BIT : public Component {
public:
    BRANCH_HISTORY_TABLE_1BIT(std::string name, SimComponent* parent) : Component(name, parent) {
        // clang-format off
        target_addr << [=] {
            // check btt to see if instruction is in there
            bool isExist = false;
            int i;
            for (i = 0; i < BTT_SIZE; i++) {
                auto pcif = pc_if.uValue();
                if (btt[i].isValid && btt[i].pc == pc_if.uValue()) {
                    isExist = true;
                    break;
                }
            }
            if (isExist) {
                // instruction is in branch target table
                // set aging counter to 0 for this instruction
                // increase aging counter for rest of the table
                // then return the branch target address
                for (int j = 0; j < BTT_SIZE; j++) {
                    if (i == j) {
                        btt[j].age = 0;
                    }
                    else {
                        if (btt[j].isValid)
                            btt[j].age++;
                    }
                }
                if (btt[i].prediction_bit == 1)
                    return btt[i].address;
                else
                    return 0xdeadbeef;
            }

            if (do_jal.uValue() == 1 || do_branch.uValue() == 1 || do_br_with_flag.uValue() == 1) {
                for (i = 0; i < BTT_SIZE; i++) {
                    if (btt[i].isValid && btt[i].address == address.uValue() && btt[i].pc == pc_ex.uValue()) {
                        // instruction is already in table dont do anything
                        for (int j = 0; j < BTT_SIZE; j++) {
                            if (btt[j].isValid)
                                btt[j].age++;
                        }
                        if (do_jal.uValue() == 1 || branch_decision.uValue() == 1 || branch_decision_flag.uValue() == 1)
                            btt[i].prediction_bit = 1;
                        else
                            btt[i].prediction_bit = 0;
                        return 0xdeadbeef;
                    }
                }
                bool allValid = true;
                for (i = 0; i < BTT_SIZE; i++) {
                    if (!btt[i].isValid) {
                        allValid = false;
                        break;
                    }
                }
                // table has empty slot
                // add new branch instruction to first empty slot
                if (!allValid) {
                    for (int j = 0; j < BTT_SIZE; j++) {
                        if (i == j) {
                            btt[j].pc = pc_ex.uValue();
                            btt[j].address = address.uValue();
                            btt[j].age = 0;
                            btt[j].isValid = true;
                            if (do_jal.uValue() == 1 || branch_decision.uValue() == 1 || branch_decision_flag.uValue() == 1)
                                btt[i].prediction_bit = 1;
                            else
                                btt[i].prediction_bit = 0;
                        }
                        else {
                            if (btt[j].isValid)
                                btt[j].age++;
                        }
                    }
                    // printBtt();
                    return 0xdeadbeef;
                }
                // table is not empty
                // find oldest entry in the table and replace it
                unsigned int oldest = 0;
                for (i = 0; i < BTT_SIZE; i++) {
                    if (btt[i].age > oldest)
                        oldest = btt[i].age;
                }
                for (i = 0; i < BTT_SIZE; i++) {
                    if (btt[i].age == oldest)
                        break;
                }
                for (int j = 0; j < BTT_SIZE; j++) {
                    if (i == j) {
                        btt[j].pc = pc_ex.uValue();
                        btt[i].address = address.uValue();
                        btt[i].age = 0;
                        btt[i].isValid = true;
                        if (do_jal.uValue() == 1 || branch_decision.uValue() == 1 || branch_decision_flag.uValue() == 1)
                            btt[i].prediction_bit = 1;
                        else
                            btt[i].prediction_bit = 0;
                    }
                    else {
                        btt[j].age++;
                    }
                }
                
            }
            else {
                for (int j = 0; j < BTT_SIZE; j++) {
                    if (btt[j].isValid)
                        btt[j].age++;
                }
            }
            return 0xdeafbeef;
        };

        valid << [=] {
            for (int i = 0; i < BTT_SIZE; i++) {
                if (btt[i].isValid && btt[i].pc == pc_if.uValue() && btt[i].prediction_bit == 1) {
                    return 1;
                }
            }
            return 0;
        };

        pc_select << [=] {
            unsigned int vif = 0;
            for (int i = 0; i < BTT_SIZE; i++) {
                if (btt[i].isValid && btt[i].pc == pc_if.uValue() && btt[i].prediction_bit == 1) {
                    vif = 1;
                    break;
                }
            }
            auto vex = valid_ex.uValue();
            auto jmp = do_jump.uValue();
            auto jal = do_jal.uValue();
            auto br_dec = branch_decision.uValue();
            auto br_with_flag_dec = branch_decision_flag.uValue();
            auto do_br = do_branch.uValue();
            auto do_br_w_flag = do_br_with_flag.uValue();
            auto br_taken = branch_decision.uValue() | branch_decision_flag.uValue();
            auto branch = do_branch.uValue() | do_br_with_flag.uValue();

            if (jmp == 0) {
                if (vex == 0 && vif == 0 && br_taken == 0 && branch == 0)
                    return PcSelect::PC4_IF;
                else if (vex == 0 && vif == 0 && br_taken == 0 && branch == 1)
                    return PcSelect::PC4_IF;
                else if (vex == 0 && vif == 0 && br_taken == 1 && branch == 1)
                    return PcSelect::ALU;
                else if (vex == 0 && vif == 1 && br_taken == 0 && branch == 0)
                    return PcSelect::BTT;
                else if (vex == 0 && vif == 1 && br_taken == 0 && branch == 1)
                    return PcSelect::BTT;
                else if (vex == 0 && vif == 1 && br_taken == 1 && branch == 1)
                    return PcSelect::ALU;
                else if (vex == 1 && vif == 0 && br_taken == 0 && branch == 1)
                    return PcSelect::PC4_EX;
                else if (vex == 1 && vif == 0 && br_taken == 1 && branch == 1)
                    return PcSelect::PC4_IF;
                else if (vex == 1 && vif == 1 && br_taken == 0 && branch == 1)
                    return PcSelect::PC4_EX;
                else if (vex == 1 && vif == 1 && br_taken == 1 && branch == 1)
                    return PcSelect::BTT;
                else
                    throw std::runtime_error("should not suppose to go here");
            } else {
                if (jal == 1) {
                    if (vex == 0)
                        return PcSelect::ALU;
                    else
                        return PcSelect::PC4_IF;
                } else {
                    return PcSelect::ALU;
                }
            }
        };
        // clang-format on
    }

    INPUTPORT(address, RV_REG_WIDTH);
    INPUTPORT(pc_if, RV_REG_WIDTH);
    INPUTPORT(pc_ex, RV_REG_WIDTH);

    INPUTPORT(branch_decision_flag, 1);
    INPUTPORT(branch_decision, 1);

    INPUTPORT(do_jump, 1);
    INPUTPORT(do_jal, 1);
    INPUTPORT(do_branch, 1);
    INPUTPORT(do_br_with_flag, 1);

    INPUTPORT(valid_ex, 1);

    OUTPUTPORT(target_addr, RV_REG_WIDTH);
    OUTPUTPORT(valid, 1);
    OUTPUTPORT_ENUM(pc_select, PcSelect);

    std::array<Dynamic1BitBranchEntry, BTT_SIZE> btt;

    void printBtt() {
        QString header = "-- Branch History Table --\n";
        SystemIO::printString(header);
        for (int j = 0; j < BTT_SIZE; j++) {
            if (btt[j].isValid) {
                QString entry = QString::number(j) + ") " + 
                    "Branch Instruction Address: " + QString::number(btt[j].pc) +
                    " Target Address: " + QString::number(btt[j].address) +
                    " Age: " + QString::number(btt[j].age) + 
                    " Prediction Bit: " + QString::number(btt[j].prediction_bit) + 
                    "\n";
                SystemIO::printString(entry);
            } else {
                QString entry = QString::number(j) + ") EMPTY\n";
                SystemIO::printString(entry);
            }
        }
        SystemIO::printString("\n\n");
    }
};
}  // namespace core
}  // namespace vsrtl
