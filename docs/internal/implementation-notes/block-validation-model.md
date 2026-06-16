# Block Validation Model

Exact goal:
- Convert primitive timing into block-shaped validation-cost models.

Files created:
- `src/block_model.cpp`
- `include/qrs_bench/block_model.h`

Required behavior:
- Compute P2TR key-path input weight.
- Compute P2MR QRS depth-0 input weight.
- Compute max input counts under 4,000,000 WU.
- Compute estimated saturated-block validation time using median, p95, p99, and max.
- Compare:
  - QRS valid saturated block vs individual Schnorr saturated block
  - QRS invalid fixed-length saturated block vs individual Schnorr saturated block
  - QRS valid saturated block vs experimental batch Schnorr saturated block
  - QRS invalid fixed-length saturated block vs experimental batch Schnorr saturated block
  - QRS valid saturated block vs reviewed-public batch Schnorr saturated block, if available
  - QRS invalid fixed-length saturated block vs reviewed-public batch Schnorr saturated block, if available
- Mark all models as simplified per-input saturation approximations.

Implementation constraints:
- Do not include outputs unless a separate realistic transaction-shape model is added.
- State that real blocks include transaction overhead and outputs.
- Keep the model conservative and transparent.

Acceptance commands:

```sh
jq '.block_model.qrs_saturated_block_valid.p99_ms' out/full.json
jq '.block_model.qrs_saturated_block_invalid_fixed_length.p99_ms' out/full.json
jq '.block_model.schnorr_experimental_batch_saturated_block.p99_ms' out/full.json
jq '.block_model.schnorr_batch_saturated_block.p99_ms' out/full.json
```
