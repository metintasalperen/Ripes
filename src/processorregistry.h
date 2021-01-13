#pragma once

#include <QMetaType>
#include <map>
#include <memory>

#include "isainfo.h"
#include "processors/ripesprocessor.h"

#include "processors/RISC-V/rv5s/rv5s.h"
#include "processors/RISC-V/rv5s_no_fw_hz/rv5s_no_fw_hz.h"
#include "processors/RISC-V/rv5s_no_hz/rv5s_no_hz.h"
#include "processors/RISC-V/rvss/rvss.h"
#include "processors/RISC-V/rv5s_static_br_pred/rv5s_static_br_pred.h"
#include "processors/RISC-V/rv5s_dynamic_br_pred_1bit/rv5s_dynamic_br_pred_1bit.h"
#include "processors/RISC-V/rv5s_dynamic_br_pred_2bit/rv5s_dynamic_br_pred_2bit.h"

namespace Ripes {
Q_NAMESPACE

// =============================== Processors =================================
enum ProcessorID { RVSS, RV5S, RV5S_NO_HZ, RV5S_NO_FW_HZ, RV5S_STATIC_BR_PRED, 
    RV5S_DYNAMIC_BR_PRED_1BIT, RV5S_DYNAMIC_BR_PRED_2BIT, NUM_PROCESSORS
};
Q_ENUM_NS(Ripes::ProcessorID);  // Register with the metaobject system
// ============================================================================

using RegisterInitialization = std::map<unsigned, uint32_t>;
struct Layout {
    QString name;
    QString file;
    /**
     * @brief stageLabelPositions
     * Stage labels are not a part of the VSRTL processor model, and as such are not serialized within the models
     * layout. stageLabelPositions determines the position of stage labels as a relative distance based on the processor
     * models' width in the VSRTL view. Should be in the range [0;1].
     * Must contain an entry for each stage in the processor model.
     */
    std::vector<double> stageLabelPositions;
    bool operator==(const Layout& rhs) const { return this->name == rhs.name; }
};

struct ProcessorDescription {
    ProcessorID id;
    RegisterInitialization defaultRegisterVals;
    const ISAInfoBase* isa;
    QString name;
    QString description;
    std::vector<Layout> layouts;
};

class ProcessorRegistry {
public:
    using ProcessorMap = std::map<ProcessorID, ProcessorDescription>;
    static const ProcessorMap& getAvailableProcessors() { return instance().m_descriptions; }
    static const ProcessorDescription& getDescription(ProcessorID id) { return instance().m_descriptions[id]; }
    static std::unique_ptr<vsrtl::core::RipesProcessor> constructProcessor(ProcessorID id) {
        switch (id) {
            case ProcessorID::RV5S_NO_FW_HZ:
                return std::make_unique<vsrtl::core::RV5S_NO_FW_HZ>();
            case ProcessorID::RV5S:
                return std::make_unique<vsrtl::core::RV5S>();
            case ProcessorID::RVSS:
                return std::make_unique<vsrtl::core::RVSS>();
            case ProcessorID::RV5S_NO_HZ:
                return std::make_unique<vsrtl::core::RV5S_NO_HZ>();
            case ProcessorID::RV5S_STATIC_BR_PRED:
                return std::make_unique<vsrtl::core::RV5S_STATIC_BR_PRED>();
            case ProcessorID::RV5S_DYNAMIC_BR_PRED_1BIT:
                return std::make_unique<vsrtl::core::RV5S_DYNAMIC_BR_PRED_1BIT>();
            case ProcessorID::RV5S_DYNAMIC_BR_PRED_2BIT:
                return std::make_unique<vsrtl::core::RV5S_DYNAMIC_BR_PRED_2BIT>();
            case ProcessorID::NUM_PROCESSORS:
                Q_UNREACHABLE();
        }
        Q_UNREACHABLE();
    }

private:
    ProcessorRegistry() {
        // Initialize processors

        // RISC-V single cycle
        ProcessorDescription desc;
        desc.id = ProcessorID::RVSS;
        desc.isa = ISAInfo<ISA::RV32IM>::instance();
        desc.name = "Single Cycle Processor";
        desc.description = "A single cycle processor";
        desc.layouts = {{"Extended", ":/layouts/RISC-V/rvss/rv_ss_extended_layout.json", {0.5}}};
        desc.defaultRegisterVals = {{2, 0x7ffffff0}, {3, 0x10000000}};
        m_descriptions[desc.id] = desc;

        // RISC-V 5-Stage
        desc = ProcessorDescription();
        desc.id = ProcessorID::RV5S;
        desc.isa = ISAInfo<ISA::RV32IM>::instance();
        desc.name = "5-Stage Processor";
        desc.description = "A 5-Stage in-order processor with hazard detection/elimination and forwarding.";
        desc.layouts = {{"Extended", ":/layouts/RISC-V/rv5s/rv5s_extended_layout.json", {0.08, 0.28, 0.54, 0.78, 0.9}}};
        desc.defaultRegisterVals = {{2, 0x7ffffff0}, {3, 0x10000000}};
        m_descriptions[desc.id] = desc;

        // RISC-V 5-stage without hazard detection
        desc = ProcessorDescription();
        desc.id = ProcessorID::RV5S_NO_HZ;
        desc.isa = ISAInfo<ISA::RV32IM>::instance();
        desc.name = "5-Stage Processor w/o hazard detection";
        desc.description = "A 5-Stage in-order processor with forwarding but no hazard detection/elimination.";
        desc.layouts = {{"Extended", ":/layouts/RISC-V/rv5s_no_hz/rv5s_no_hz_extended_layout.json", {0.08, 0.28, 0.53, 0.78, 0.9}}};
        desc.defaultRegisterVals = {{2, 0x7ffffff0}, {3, 0x10000000}};
        m_descriptions[desc.id] = desc;

        // RISC-V 5-stage without forwarding or hazard detection
        desc = ProcessorDescription();
        desc.id = ProcessorID::RV5S_NO_FW_HZ;
        desc.isa = ISAInfo<ISA::RV32IM>::instance();
        desc.name = "5-Stage Processor w/o forwarding or hazard detection";
        desc.description = "A 5-Stage in-order processor with no forwarding or hazard detection/elimination.";
        desc.layouts = {{"Extended",
                         ":/layouts/RISC-V/rv5s_no_fw_hz/rv5s_no_fw_hz_extended_layout.json",
                         {0.08, 0.31, 0.56, 0.76, 0.9}}};
        desc.defaultRegisterVals = {{2, 0x7ffffff0}, {3, 0x10000000}};
        m_descriptions[desc.id] = desc;

        // RISC-V 5-Stage with static branch prediction
        desc = ProcessorDescription();
        desc.id = ProcessorID::RV5S_STATIC_BR_PRED;
        desc.isa = ISAInfo<ISA::RV32IM>::instance();
        desc.name = "5-Stage Processor with Static Branch Prediction";
        desc.description = "A 5-Stage in-order processor with static branch prediction. (always predict taken)";
        desc.layouts = {{"Extended", ":/layouts/RISC-V/rv5s_static_br_pred/rv5s_static_br_pred_extended_layout.json", {0.08, 0.28, 0.54, 0.78, 0.9}}};
        desc.defaultRegisterVals = {{2, 0x7ffffff0}, {3, 0x10000000}};
        m_descriptions[desc.id] = desc;

        // RISC-V 5-Stage with dynamic branch prediction - 1 bit
        desc = ProcessorDescription();
        desc.id = ProcessorID::RV5S_DYNAMIC_BR_PRED_1BIT;
        desc.isa = ISAInfo<ISA::RV32IM>::instance();
        desc.name = "5-Stage Processor with Dynamic Branch Prediction - 1 bit";
        desc.description = "A 5-Stage in-order processor with dynamic branch prediction - 1 bit";
        desc.layouts = {{"Extended", 
            ":/layouts/RISC-V/rv5s_dynamic_br_pred_1bit/rv5s_dynamic_br_pred_1bit_extended_layout.json", 
            {0.08, 0.28, 0.54, 0.78, 0.9}}};
        desc.defaultRegisterVals = {{2, 0x7ffffff0}, {3, 0x10000000}};
        m_descriptions[desc.id] = desc;

        // RISC-V 5-Stage with dynamic branch prediction - 2 bit
        desc = ProcessorDescription();
        desc.id = ProcessorID::RV5S_DYNAMIC_BR_PRED_2BIT;
        desc.isa = ISAInfo<ISA::RV32IM>::instance();
        desc.name = "5-Stage Processor with Dynamic Branch Prediction - 2 bit";
        desc.description = "A 5-Stage in-order processor with dynamic branch prediction - 2 bit";
        desc.layouts = {{"Extended", 
            ":/layouts/RISC-V/rv5s_dynamic_br_pred_2bit/rv5s_dynamic_br_pred_2bit_extended_layout.json",
            {0.08, 0.28, 0.54, 0.78, 0.9}}};
        desc.defaultRegisterVals = {{2, 0x7ffffff0}, {3, 0x10000000}};
        m_descriptions[desc.id] = desc;
    }

    static ProcessorRegistry& instance() {
        static ProcessorRegistry pr;
        return pr;
    }

    ProcessorMap m_descriptions;
};
}  // namespace Ripes
