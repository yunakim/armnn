//
// Copyright © 2017 Arm Ltd. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "NeonPermuteWorkload.hpp"
#include "backends/NeonTensorHandle.hpp"
#include "backends/ArmComputeTensorUtils.hpp"

#include <arm_compute/core/Error.h>

namespace armnn
{

arm_compute::Status NeonPermuteWorkloadValidate(const TensorInfo& input,
                                                const TensorInfo& output,
                                                const PermuteDescriptor& descriptor)
{
    const arm_compute::TensorInfo aclInputInfo = armcomputetensorutils::BuildArmComputeTensorInfo(input);
    const arm_compute::TensorInfo aclOutputInfo = armcomputetensorutils::BuildArmComputeTensorInfo(output);
    const armnn::PermutationVector& mappings = descriptor.m_DimMappings;

    return arm_compute::NEPermute::validate(&aclInputInfo, &aclOutputInfo,
                                      armcomputetensorutils::BuildArmComputePermutationVector(mappings));
}

template <armnn::DataType... DataTypes>
NeonPermuteWorkload<DataTypes...>::NeonPermuteWorkload(const PermuteQueueDescriptor& descriptor,
                                               const WorkloadInfo& info)
        : TypedWorkload<PermuteQueueDescriptor, DataTypes...>(descriptor, info)
{
    using armcomputetensorutils::BuildArmComputePermutationVector;

    m_Data.ValidateInputsOutputs(GetName(), 1, 1);

    const arm_compute::ITensor& input = static_cast<INeonTensorHandle*>(m_Data.m_Inputs[0])->GetTensor();
    arm_compute::ITensor& output = static_cast<INeonTensorHandle*>(m_Data.m_Outputs[0])->GetTensor();
    const armnn::PermutationVector& mappings = m_Data.m_Parameters.m_DimMappings;

    // Run the layer.
    m_PermuteFunction.configure(&input, &output, BuildArmComputePermutationVector(mappings));
}

template <armnn::DataType... DataTypes>
void NeonPermuteWorkload<DataTypes...>::Execute() const
{
    ARMNN_SCOPED_PROFILING_EVENT_NEON(GetName() + "_Execute");
    m_PermuteFunction.run();
}

template class NeonPermuteWorkload<DataType::Float16, DataType::Float32>;
template class NeonPermuteWorkload<DataType::QuantisedAsymm8>;

} // namespace armnn
